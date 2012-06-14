#include "agent.h"
#include "game.h"
#include "resource.h"
#include "render/renderer.h"
#include "json/cJSON.h"

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

            // global position from map location
            a->sprite.position = Map_isoToGlobal( &a->location );

            // current animation
            a->sprite.animation = NULL;



        // Get init state data
        // position
        item = cJSON_GetObjectItem( root, "init" );
        check( item, "Error while loading agent '%s', need entry 'init'.\n", file );

        subitem = cJSON_GetObjectItem( item, "position" );
        check( subitem, "Error while loading agent '%s', need subentry 'position' in entry 'init'.\n", file );

        a->location.x = cJSON_GetArrayItem( subitem, 0 )->valueint;
        a->location.y = cJSON_GetArrayItem( subitem, 1 )->valueint;

        // animation
        subitem = cJSON_GetObjectItem( item, "animation" );
        //check( subitem, "Error while loading agent '%s', need subentry 'animation' in entry 'init'.\n", file );

        if( subitem ) {
            Anim *anim = AnimManager_gets( &game->anims, subitem->valuestring );
            check( anim, "Error while loading agent '%s', animation '%s' is not a loaded resource.\n", file, subitem->valuestring );

            a->sprite.animation = anim;
        }

        



        ret = true;
    }

error:
    DEL_PTR( json_file );
    if( root ) cJSON_Delete( root );
    return ret;
}


inline void Agent_setPosition( Agent *a, const vec2i *pos ) {
    if( a && pos )  {
        // set location
        vec2i_cpy( &a->location, pos );

        // check if agent moved to another world tile
        vec2i new_wt = Map_globalToWorld( pos );

        if( !vec2i_eq( &a->world_tile, &new_wt ) ) {
            // alert World that this agent as changed its tile
            World_movedAgent( game->world, a->world_id, &a->world_tile, &new_wt );

            vec2i_cpy( &a->world_tile, &new_wt );
        }


        // set sprite position
        Sprite_setPosition( &a->sprite, pos );
    }
}

inline void Agent_playAnimation( Agent *a, Animation anim ) {
    if( a )
        Sprite_setAnimation( &a->sprite, anim );
}
