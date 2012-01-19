#include "scene.h"

Scene *Scene_new() {
    Scene *s = NULL;
    
    s = byte_alloc( sizeof( Scene ) );
    check_mem( s );

    s->mMeshes = HandleManager_init( 50 );
    s->mShaders = HandleManager_init( 50 );
    s->mTextures = HandleManager_init( 50 );
        
error:
    return s;
}

void Scene_destroy( Scene *pScene ) {
    if( pScene ) {
        HandleManager_destroy( pScene->mMeshes );
        HandleManager_destroy( pScene->mShaders );
        HandleManager_destroy( pScene->mTextures );
        DEL_PTR( pScene );
    }   
}

/*
bool Scene_parseResource( Scene *pScene, ResourceType pType, const char *pFile ) {
    cJSON *root = NULL;
    if( pScene && pFile ) {
        char *json_file = NULL;
        ReadFile( &json_file, pFile );
        check( json_file, "Error while opening file \"%s\"!", pFile );
        
        root = cJSON_Parse( json_file );
        DEL_PTR( json_file );
        check( root, "JSON error before : \n%s.", cJSON_GetErrorPtr() );

        switch( pType ) {
            case RT_Shader :

                break;
            case RT_Entity :
                break;
            case RT_Texture :
                break;
        }

        cJSON_Delete( root );
        return true;
    }
error:
    if( root ) cJSON_Delete( root );
    return false;
}
*/
