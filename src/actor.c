#include "actor.h"
#include "game.h"
#include "renderer.h"
#include "json/cJSON.h"

bool Actor_load( Actor *pActor, const char *pFile ) {
    cJSON *root = NULL, *item = NULL, *subitem = NULL;
    char *json_file = NULL;
    bool return_val = false;

    if( pActor && pFile ) {
        // read and parse json file
        Byte_ReadFile( &json_file, pFile );
        check( json_file, "Error in actor loading!\n" );

        root = cJSON_Parse( json_file );
        check( root, "JSON parse error [%s] before :\n%s\n", pFile, cJSON_GetErrorPtr() );

        // get basic actor info
        item = cJSON_GetObjectItem( root, "firstname" );
        if( item ) 
            strncpy( pActor->mFirstname, item->valuestring, 32 );
        else
            strncpy( pActor->mFirstname, "???", 32 );

        item = cJSON_GetObjectItem( root, "lastname" );
        if( item ) 
            strncpy( pActor->mLastname, item->valuestring, 32 );
        else
            strncpy( pActor->mLastname, "unknown", 32 );

        item = cJSON_GetObjectItem( root, "surname" );
        if( item )
            strncpy( pActor->mSurname, item->valuestring, 32 );
        else
            strncpy( pActor->mSurname, "", 32 );

        // get game initial data
        item = cJSON_GetObjectItem( root, "init" );
        check( item, "Error while loading actor '%s', need entry 'init'.\n", pFile );

        subitem = cJSON_GetObjectItem( item, "position" );
        check( subitem, "Error while loading actor '%s', need subentry 'position' in entry 'init'.\n", pFile );

        pActor->mPosition.x = cJSON_GetArrayItem( subitem, 0 )->valuedouble;
        pActor->mPosition.y = cJSON_GetArrayItem( subitem, 1 )->valuedouble;


        // get rendering data
            item = cJSON_GetObjectItem( root, "sprite" );
            check( item, "Error while loading actor '%s', need entry 'sprite'.\n", pFile );

            subitem = cJSON_GetObjectItem( item, "width" );
            check( subitem, "Error while loading actor '%s', need subentry 'width' in entry 'sprite'.\n", pFile );

            pActor->mSize.x = subitem->valueint;

            subitem = cJSON_GetObjectItem( item, "height" );
            check( subitem, "Error while loading actor '%s', need subentry 'height' in entry 'sprite'.\n", pFile );

            pActor->mSize.y = subitem->valueint;

            // get mesh and resize it to the sprite size of the actor
            str256 mesh_str;
            str16 mesh_size;
            MSG( mesh_size, 16, "%d.%d", (int)pActor->mSize.x, (int)pActor->mSize.y );

            subitem = cJSON_GetObjectItem( item, "mesh" );
            check( subitem, "Error while loading actor '%s', need subentry 'mesh' in entry 'sprite'.\n", pFile );

            strcpy( mesh_str, subitem->valuestring );
            strcat( mesh_str, mesh_size );

            pActor->mMesh_id = World_getResource( mesh_str );
            // if correctly sized mesh is not yet loaded, create it
            if( -1 == pActor->mMesh_id ) {
                pActor->mMesh_id = World_getResource( subitem->valuestring );
                check( pActor->mMesh_id >= 0, "Error while loading actor '%s', its mesh '%s' is not a loaded resource.\n", pFile, subitem->valuestring );

                // resize
                int scaled_mesh = Renderer_createRescaledMesh( pActor->mMesh_id, &pActor->mSize );
                check( scaled_mesh >= 0, "Error while creating scaled mesh for actor '%s'.\n", pFile );

                pActor->mMesh_id = scaled_mesh;

                // add it to the resource manager
                World_addResource( mesh_str, scaled_mesh );
            }

                

            subitem = cJSON_GetObjectItem( item, "texture" );
            check( subitem, "Error while loading actor '%s', need subentry 'texture' in entry 'sprite'.\n", pFile );

            pActor->mTexture_id = World_getResource( subitem->valuestring );
            check( pActor->mTexture_id >= 0, "Error while loading actor '%s', its texture '%s' is not a loaded resource.\n", pFile, subitem->valuestring );

        pActor->mUsedSprite = -1;

        return_val = true;
    }

error:
    DEL_PTR( json_file );
    if( root ) cJSON_Delete( root );
    return return_val;
}

void Actor_move( Actor *pActor, vec2 *v ) {
    if( pActor ) {
        pActor->mPosition = vec2_add( &pActor->mPosition, v );
        Actor_setPositionfv( pActor, &pActor->mPosition );
    }
}

void Actor_setPositionf( Actor *pActor, f32 x, f32 y ) {
    vec2 v = { x, y };
    Actor_setPositionfv( pActor, &v );
}

void Actor_setPositionfv( Actor *pActor, vec2 *v ) {
    if( pActor ) {
        vec2_cpy( &pActor->mPosition, v );
        if( pActor->mUsedSprite >= 0 ) {
            mat3 m;
            mat3_translationMatrixfv( &m, &pActor->mPosition );
            Scene_modifySprite( game->mScene, pActor->mUsedSprite, SA_Matrix, &m );
        }
    }
}
