#include "actor.h"
#include "world.h"
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
        check( item, "Error while loading actor '%s', need entry 'firstname'.\n", pFile );
        strncpy( pActor->mFirstname, item->valuestring, 32 );

        item = cJSON_GetObjectItem( root, "lastname" );
        check( item, "Error while loading actor '%s', need entry 'lastname'.\n", pFile );
        strncpy( pActor->mLastname, item->valuestring, 32 );

        item = cJSON_GetObjectItem( root, "surname" );
        check( item, "Error while loading actor '%s', need entry 'surname'.\n", pFile );
        strncpy( pActor->mSurname, item->valuestring, 32 );

        // get game initial data
        item = cJSON_GetObjectItem( root, "init" );
        check( item, "Error while loading actor '%s', need entry 'init'.\n", pFile );

        subitem = cJSON_GetObjectItem( item, "position" );
        check( subitem, "Error while loading actor '%s', need subentry 'position' in entry 'init'.\n", pFile );

        // calculate object position/orientation into actor matrix
        vec2 position;
        position.x = cJSON_GetArrayItem( subitem, 0 )->valuedouble;
        position.y = cJSON_GetArrayItem( subitem, 1 )->valuedouble;

        mat3_translationMatrixfv( &pActor->mPosition, &position );


        // get rendering data
        item = cJSON_GetObjectItem( root, "entity" );
        check( item, "Error while loading actor '%s', need entry 'entity'.\n", pFile );

        subitem = cJSON_GetObjectItem( item, "mesh" );
        check( subitem, "Error while loading actor '%s', need subentry 'mesh' in entry 'entity'.\n", pFile );

        pActor->mMesh_id = World_getResource( subitem->valuestring );
        check( pActor->mMesh_id >= 0, "Error while loading actor '%s', its mesh '%s' is not a loaded resource.\n", pFile, subitem->valuestring );


        subitem = cJSON_GetObjectItem( item, "texture" );
        check( subitem, "Error while loading actor '%s', need subentry 'texture' in entry 'entity'.\n", pFile );

        pActor->mTexture_id = World_getResource( subitem->valuestring );
        check( pActor->mTexture_id >= 0, "Error while loading actor '%s', its texture '%s' is not a loaded resource.\n", pFile, subitem->valuestring );

        return_val = true;
    }

error:
    DEL_PTR( json_file );
    if( root ) cJSON_Delete( root );
    return return_val;
}
