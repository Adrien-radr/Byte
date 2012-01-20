#include "resource.h"
#include "renderer.h"
#include "json/cJSON.h"

const char ShaderDirectory[] = "data/shaders/";
const char MeshDirectory[] = "data/meshes/";
const char TextureDirectory[] = "data/textures/";

// Array of u32 for the Hashes and Handles array of the resourcemanager
SimpleArray( u32, u32 )

typedef struct s_ResourceManager {
    u32Array    mHashes;
    u32Array    mHandles;

} ResourceManager;


ResourceManager *ResourceManager_new() {
    ResourceManager *rm = NULL;
    rm = byte_alloc( sizeof( ResourceManager ) );
    check_mem( rm );

    u32Array_init( &rm->mHashes, 100 );
    u32Array_init( &rm->mHandles, 100 );

    return rm;

error:
    ResourceManager_destroy( rm );
    return rm;
}

void ResourceManager_destroy( ResourceManager *pRM ) {
    if( pRM ) {
        u32Array_destroy( &pRM->mHashes );
        u32Array_destroy( &pRM->mHandles );
        DEL_PTR( pRM );
    }
}



// internally used function (in ResourceManager_load)
int LoadShader( const char *pFile ) {
    char *json_file = NULL;
    cJSON *root = NULL;

    // read and parse json shader file
    ReadFile( &json_file, pFile );
    check( json_file, " " );

    root = cJSON_Parse( json_file );
    check( root, "JSON parse error [%s] before :\n%s\n", pFile, cJSON_GetErrorPtr() );
    
    const char* v_file = cJSON_GetObjectItem( root, "v_src" )->valuestring;
    const char* f_file = cJSON_GetObjectItem( root, "f_src" )->valuestring;

    check( v_file && f_file, "Could not get Vertex and Fragment shader files fron JSON resource \"%s\".", pFile );

    // get full path to shader files
    str256 v_path, f_path;
    v_path[0] = f_path[0] = 0;

    strcat( v_path, ShaderDirectory );
    strcat( v_path, v_file );

    strcat( f_path, ShaderDirectory );
    strcat( f_path, f_file );

    // create shader from shader files
    int handle = Renderer_createShader( v_path, f_path );
    check( handle >= 0, "Error while loading shader from \"%s\".", pFile );

    // load shader parameters
    Shader *s = Renderer_getShader( handle );
    Renderer_useShader( handle );

    cJSON *params = cJSON_GetObjectItem( root, "params" );

    if( params ) {
        int param_n = cJSON_GetArraySize( params );
        for( u32 i = 0; i < param_n; ++i ) {
            cJSON *item = cJSON_GetArrayItem( params, i );
            cJSON *item_name = cJSON_GetObjectItem( item, "name" );
            cJSON *item_value = cJSON_GetObjectItem( item, "value" );

            if( !strcmp( item_name->valuestring, "Albedo" ) ) {
                if( 0 <= item_value->valueint )
                    Shader_sendInt( "Albedo", item_value->valueint );
            } else if( !strcmp( item_name->valuestring, "UseProjectionMatrix" ) ) {
                if( 0 < item_value->valueint )
                    s->mUseProjectionMatrix = true;
                else
                    s->mUseProjectionMatrix = false;
            }
        }
    } else {
        s->mUseProjectionMatrix = false;

    }

    // free used memory
    DEL_PTR(json_file);
    if( root ) cJSON_Delete( root );

    return handle;

error:
    DEL_PTR(json_file);
    if( root ) cJSON_Delete( root );

    return -1;
}

int ResourceManager_load( ResourceManager *pRM, ResourceType pType, const char *pFile ) {
    int handle = -1;
    if( pRM && pFile && Renderer_isInitialized() ) {
        u32 hash = GetHash( pFile );

        // search if wanted resource already exist
        for( int i = 0; i < pRM->mHashes.cpt; ++i )  {
            // the array is sorted in increasing order so break if superior
            if( pRM->mHashes.data[i] > hash )
                break;
            else if( pRM->mHashes.data[i] == hash ) {
                handle = pRM->mHandles.data[i];
                break;
            }
        }

        // get full path to resource (in its corresponding data dir)
        str256 file_path;
        file_path[0] = 0;

        // if we find it, return its handle
        if( handle >= 0 ) { 
            log_info( "Resource \"%s\" is already present in resource manager!\n", pFile );
            return handle;
        }

        // else, load the data
        switch( pType ) {
            case RT_Texture:
                // get complete file path
                strcat( file_path, TextureDirectory );
                strcat( file_path, pFile );

                // load texture
                handle = Renderer_createTexture( file_path, true ); 
                break;
            case RT_Shader:
                // get complete file path
                strcat( file_path, ShaderDirectory );
                strcat( file_path, pFile );

                // parse json file to get both
                if( CheckExtension( file_path, "json" ) ) {
                    handle = LoadShader( file_path );
                } else 
                    log_err( "ResourceManager needs a .json file to load a shader!\n" );
                break;
            default:
                break;
        }


        // if creation went well, store the hash and the handle
        if( handle >= 0 ) {
            if( u32Array_checkSize( &pRM->mHashes ) && u32Array_checkSize( &pRM->mHandles ) ) {
                u32 index = pRM->mHashes.cpt++;
                if( index != (pRM->mHandles.cpt++) ) {
                    log_err( "Error in Resource Manager, arrays are not parallel anymore!\n" );
                    handle = -1;
                } else {
                    // if 1st resource, just put it in place
                    if( 1 == pRM->mHashes.cpt ) {
                        pRM->mHashes.data[index] = hash;
                        pRM->mHandles.data[index] = handle;
                        log_info( "Resource \"%s\" loaded.\n", pFile );
                    }
                    // else, sort array
                    else {
                        // sort arrays in increasing order
                        for( u32 i = pRM->mHashes.cpt-1; i >= 0; --i )
                            if( pRM->mHashes.data[i-1] > hash ) {
                                pRM->mHashes.data[i] = pRM->mHashes.data[i-1];
                                pRM->mHandles.data[i] = pRM->mHandles.data[i-1];
                            } else {
                                pRM->mHashes.data[i] = hash;
                                pRM->mHandles.data[i] = handle;
                                log_info( "Resource \"%s\" loaded.\n", pFile );
                                break;
                            }
                    }
                }
            }
        }
    }
    return handle;
}
