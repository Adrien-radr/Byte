#include "actor.h"
#include "json/cJSON.h"

bool Actor_load( Actor *actor, const char *file ) {
    cJSON *root = NULL, *item = NULL, *subitem = NULL;
    char *json_file = NULL;
    bool return_val = false;

    if( actor && file ) {
        // read and parse json file
        Byte_ReadFile( &json_file, file );
        check( json_file, "Error in actor loading!\n" );

        root = cJSON_Parse( json_file );
        check( root, "JSON parse error [%s] before :\n%s\n", file, cJSON_GetErrorPtr() );

        // get basic actor info
        item = cJSON_GetObjectItem( root, "firstname" );
        if( item ) 
            strncpy( actor->firstname, item->valuestring, 32 );
        else
            strncpy( actor->firstname, "???", 32 );

        item = cJSON_GetObjectItem( root, "lastname" );
        if( item ) 
            strncpy( actor->lastname, item->valuestring, 32 );
        else
            strncpy( actor->lastname, "unknown", 32 );

        item = cJSON_GetObjectItem( root, "surname" );
        if( item )
            strncpy( actor->surname, item->valuestring, 32 );
        else
            strncpy( actor->surname, "", 32 );

        // get game initial data
        item = cJSON_GetObjectItem( root, "init" );
        check( item, "Error while loading actor '%s', need entry 'init'.\n", file );

        subitem = cJSON_GetObjectItem( item, "position" );
        check( subitem, "Error while loading actor '%s', need subentry 'position' in entry 'init'.\n", file );

        actor->position.x = cJSON_GetArrayItem( subitem, 0 )->valueint;
        actor->position.y = cJSON_GetArrayItem( subitem, 1 )->valueint;

        subitem = cJSON_GetObjectItem( item, "animation" );
        if( subitem )
            strncpy( actor->assets.anim_str, subitem->valuestring, 128 );
        else
            strcpy( actor->assets.anim_str, "" );


        // get rendering data
            item = cJSON_GetObjectItem( root, "sprite" );
            check( item, "Error while loading actor '%s', need entry 'sprite'.\n", file );

            subitem = cJSON_GetObjectItem( item, "width" );
            check( subitem, "Error while loading actor '%s', need subentry 'width' in entry 'sprite'.\n", file );

            actor->assets.mesh_size.x = subitem->valueint;

            subitem = cJSON_GetObjectItem( item, "height" );
            check( subitem, "Error while loading actor '%s', need subentry 'height' in entry 'sprite'.\n", file );

            actor->assets.mesh_size.y = subitem->valueint;

            // get mesh and resize it to the sprite size of the actor
            subitem = cJSON_GetObjectItem( item, "mesh" );
            check( subitem, "Error while loading actor '%s', need subentry 'mesh' in entry 'sprite'.\n", file );

            strncpy( actor->assets.mesh, subitem->valuestring, 128 );

            // actor textures
            subitem = cJSON_GetObjectItem( item, "texture" );
            actor->assets.tex_n = cJSON_GetArraySize( subitem );
            check( actor->assets.tex_n > 0, "Error while loading actor '%s', need subentry(array) 'texture' in entry 'sprite'.\n", file );

            for( int i = 0; i < actor->assets.tex_n; ++i ) {
                cJSON *tex = cJSON_GetArrayItem( subitem, i );
                if( tex ) 
                    strncpy( actor->assets.texture[i], tex->valuestring, 128 );
            }

        actor->used_sprite = -1;

        return_val = true;
    }

error:
    DEL_PTR( json_file );
    if( root ) cJSON_Delete( root );
    return return_val;
}

/*
void Actor_move( Actor *actor, vec2 *v ) {
    if( actor ) {
        actor->mPosition = vec2_add( &actor->mPosition, v );
        Actor_setPositionfv( actor, &actor->mPosition );
    }
}

void Actor_setPositionf( Actor *actor, f32 x, f32 y ) {
    vec2 v = { x, y };
    Actor_setPositionfv( actor, &v );
}

void Actor_setPositionfv( Actor *actor, vec2 *v ) {
    if( actor ) {
        vec2_cpy( &actor->mPosition, v );
        if( actor->used_sprite >= 0 ) {
            mat3 m;
            mat3_translationMatrixfv( &m, &actor->mPosition );
            //Scene_modifySprite( game->mScene, actor->mUsedSprite, SA_Matrix, &m );
        }
    }
}
*/
