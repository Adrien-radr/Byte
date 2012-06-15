#include "agent.h"
#include "game.h"
#include "resource.h"
#include "render/renderer.h"
#include "json/cJSON.h"

//      SHARED CONST VARIABLES
const int human_walkspeed = 75;
const int human_runspeed = 150;





bool Agent_load( Agent *a, const char *file ) {
    cJSON *root  = NULL, *item = NULL, *subitem = NULL;
    char *json_file = NULL;
    bool ret = false;

    if( a && file ) {
        /// read and parse file
        Byte_ReadFile( &json_file, file );
        check( json_file, "Error in agent loading!\n" );

        root = cJSON_Parse( json_file );
        check( root, "JSON parse error [%s] before :\n%s\n", file, cJSON_GetErrorPtr() );

        // get basic agent info
        item = cJSON_GetObjectItem( root, "firstname" );
        check( item, "Agents need a firstname!\n" );

        strncpy( a->firstname, item->valuestring, 32 );
            
        item = cJSON_GetObjectItem( root, "lastname" );
        if( item ) 
            strncpy( a->lastname, item->valuestring, 32 );
        else
            strncpy( a->lastname, "", 32 );

        item = cJSON_GetObjectItem( root, "surname" );
        if( item )
            strncpy( a->surname, item->valuestring, 32 );
        else
            strncpy( a->surname, "", 32 );



        // Load used sprite
        item = cJSON_GetObjectItem( root, "sprite" );
        check( item, "Error while loading agent '%s', need entry 'sprite'.\n", file );

        int sprite_h = ResourceManager_get( item->valuestring );
        check( sprite_h >= 0, "Error while loading agent '%s'. Its sprite '%s' is not a loaded resource!\n", file, item->valuestring );

        // Copy sprite and sets its attributes
        Sprite *s = Renderer_getSprite( sprite_h );
        Sprite_cpy( &a->sprite, s );



        // Get init state data
        // position
        item = cJSON_GetObjectItem( root, "init" );
        check( item, "Error while loading agent '%s', need entry 'init'.\n", file );

        subitem = cJSON_GetObjectItem( item, "position" );
        check( subitem, "Error while loading agent '%s', need subentry 'position' in entry 'init'.\n", file );

        a->location.x = cJSON_GetArrayItem( subitem, 0 )->valueint;
        a->location.y = cJSON_GetArrayItem( subitem, 1 )->valueint;

        // global position from map location
        a->sprite.position = Map_isoToGlobal( &a->location );


        // current animation (none by default)
        a->sprite.animation = NULL;

        // animation
        subitem = cJSON_GetObjectItem( item, "animation" );
        //check( subitem, "Error while loading agent '%s', need subentry 'animation' in entry 'init'.\n", file );

        if( subitem ) {
            Anim *anim = AnimManager_gets( &game->anims, subitem->valuestring );
            check( anim, "Error while loading agent '%s', animation '%s' is not a loaded resource.\n", file, subitem->valuestring );

            a->sprite.animation = anim;
        }

        // path (none by default)
        a->path = NULL;
        a->path_index = 1;
        



        ret = true;
    }

error:
    DEL_PTR( json_file );
    if( root ) cJSON_Delete( root );
    return ret;
}


inline void Agent_destroy( Agent *a ) {
    if( a->path )
        Map_destroyPath( a->path );
}

inline void Agent_setLocation( Agent *a, const vec2i *loc ) {
    if( a && loc )  {
        // set location
        vec2i_cpy( &a->location, loc );

        // check if agent moved to another world tile
        vec2i new_wt = Map_isoToWorld( loc );

        if( !vec2i_eq( &a->world_tile, &new_wt ) ) {
            // alert World that this agent as changed its tile
            World_movedAgent( game->world, a->world_id, &a->world_tile, &new_wt );

            vec2i_cpy( &a->world_tile, &new_wt );
        }


        // set sprite position
        Sprite_setLocation( &a->sprite, loc );
    }
}

inline void Agent_playAnimation( Agent *a, Animation anim ) {
    if( a )
        Sprite_setAnimation( &a->sprite, anim );
}

inline void Agent_moveTo( Agent *a, const vec2i *loc ) {
    if( a ) {
        // create new path. Erase last one if present
        if( a->path ) {
            Map_destroyPath( a->path );
            a->path_index = 1;
        }

        /* TODO : What with OOS AI ?? */
        a->path = Map_createPath( &game->world->active_map, &a->location, loc );
    }
}

void Agent_update( Agent *a, f32 frame_time ) {
    static bool reached_tile = false;

    // update agent current path and position, if one
    if( a->path ) {
        /* TODO : only do IS agents for now. what for OOS? */
        if( a->sprite.used_sprite >= 0 ) {
            vec2i *next = Map_getPathNode( a->path, a->path_index );

            // Get Direction vector from current to next tile
            vec2 nextf = Map_isoToGlobal( next );
            vec2 dir = vec2_sub( &nextf, &a->sprite.position );
            vec2_normalize( &dir );

            // move sprite in this direction
            vec2 newpos;
            vec2_cpy( &newpos, &a->sprite.position );
            newpos.x += dir.x * frame_time * human_runspeed;
            newpos.y += dir.y * frame_time * human_runspeed;

            Sprite_setPosition( &a->sprite, &newpos );

            // update current map tile if necessary
            vec2i newtile = Map_globalToIso( &newpos );
            if( !vec2i_eq( &newtile, &a->location ) ) {
                //      REACHED A NEW TILE HERE
                reached_tile = true;
                vec2i_cpy( &a->location, &newtile );

                // update current world_tile if necessary
                vec2i new_wt = Map_isoToWorld( &newtile );
                if( !vec2i_eq( &a->world_tile, &new_wt ) ) {
                    // alert World that this agent as changed its tile
                    World_movedAgent( game->world, a->world_id, &a->world_tile, &new_wt );

                    vec2i_cpy( &a->world_tile, &new_wt );
                }

            }

            // continue in this tile until approximatively at the center
            if( reached_tile ) {
                if( Abs( nextf.x - newpos.x ) < 5.f &&
                    Abs( nextf.y - newpos.y ) < 2.5f ) {
               
                    //      REACHED TILE CENTER HERE
                    a->path_index++;
                    reached_tile = false;

                    // check if we are at the end of the path
                    if( a->path_index == Map_getPathCount( a->path ) ) {
                        Map_destroyPath( a->path );
                        a->path = NULL;
                        a->path_index = 1;
                    }
                }
            }
        }
    }
}
