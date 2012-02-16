#include "resource.h"
#include "renderer.h"
#include "json/cJSON.h"

#include <dirent.h>

const char ShaderDirectory[] = "data/shaders/";
const char MeshDirectory[] = "data/meshes/";
const char TextureDirectory[] = "data/textures/";
const char FontDirectory[] = "data/fonts/";

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

bool ResourceManager_addEntry( ResourceManager *pRM, u32 pHash, u32 pHandle ) {
    if( pRM )
        if( u32Array_checkSize( &pRM->mHashes ) && u32Array_checkSize( &pRM->mHandles ) ) {
            u32 index = pRM->mHashes.cpt++;
            if( index != (pRM->mHandles.cpt++) ) {
                log_err( "Error in Resource Manager, arrays are not parallel anymore!\n" );
            } else {
                pRM->mHashes.data[index] = pHash;
                pRM->mHandles.data[index] = pHandle;
                u32 tmp1, tmp2;
                // sort array after 2nd insertion, in increasing order
                if( 1 < pRM->mHashes.cpt )
                    for( int i = pRM->mHashes.cpt-2; i >= 0; --i )
                        if( pRM->mHashes.data[i] > pRM->mHashes.data[i+1] ) {
                            // switch two  instances
                            tmp1 = pRM->mHashes.data[i];
                            tmp2 = pRM->mHandles.data[i];
                            pRM->mHashes.data[i] = pRM->mHashes.data[i+1];
                            pRM->mHandles.data[i] = pRM->mHandles.data[i+1];
                            pRM->mHashes.data[i+1] = tmp1;
                            pRM->mHandles.data[i+1] = tmp2;
                        } else
                            break;
                return true;
            }
        }
    return false;
}


// internally used function (in ResourceManager_load)
int LoadShader( const char *pFile ) {
    char *json_file = NULL;
    cJSON *root = NULL;
    int handle = -1;

    // read and parse json shader file
    Byte_ReadFile( &json_file, pFile );
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
    handle = Renderer_createShader( v_path, f_path );
    check( handle >= 0, "Error while loading shader from \"%s\".\n", pFile );

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


error:
    DEL_PTR(json_file);
    if( root ) cJSON_Delete( root );

    return handle;
}

int LoadMesh( const char *pFile ) {
    char *json_file = NULL;
    cJSON *root = NULL;
    int handle = -1;

    // read and parse json file
    Byte_ReadFile( &json_file, pFile );
    check( json_file, " " );

    root = cJSON_Parse( json_file );
    check( root, "JSON parse error [%s] before :\n%s\n", pFile, cJSON_GetErrorPtr() );

    cJSON *positions = cJSON_GetObjectItem( root, "positions" );
    cJSON *texcoords = cJSON_GetObjectItem( root, "texcoords" );
    cJSON *indices = cJSON_GetObjectItem( root, "indices" );

    check( positions && texcoords , "Invalid mesh file. Needs positions and texcoords!\n" );

    // prepare mesh arrays
    vec2 *positions_v = NULL, *texcoords_v = NULL;
    u32 *indices_v = NULL;

    int position_n = cJSON_GetArraySize( positions );
    positions_v = byte_alloc( position_n * sizeof( vec2 ) );
    texcoords_v = byte_alloc( position_n * sizeof( vec2 ) );

    for( u32 i = 0; i < position_n; ++i ) {
        cJSON *pos = cJSON_GetArrayItem( positions, i );
        cJSON *tex = cJSON_GetArrayItem( texcoords, i );

        vec2 pos_v = { cJSON_GetArrayItem( pos, 0 )->valuedouble, cJSON_GetArrayItem( pos, 1 )->valuedouble };
        positions_v[i] = pos_v;

        vec2 tex_v = { cJSON_GetArrayItem( tex, 0 )->valuedouble, cJSON_GetArrayItem( tex, 1 )->valuedouble };
        texcoords_v[i] = tex_v;
    }

    // if we got indices, get em
    int indice_n = 0;
    if( indices ) {
        indice_n = cJSON_GetArraySize( indices );
        indices_v = byte_alloc( indice_n * sizeof( u32 ) );

        for( u32 i = 0; i < indice_n; ++i ) {
            indices_v[i] = cJSON_GetArrayItem( indices, i )->valuedouble;
        }
    }

    // create mesh (vbo) from the renderer
    handle = Renderer_createStaticMesh( indices_v, indice_n * sizeof( u32 ), positions_v, position_n * sizeof( vec2 ), texcoords_v, position_n * sizeof( vec2 ) );
    check( handle >= 0, "Error while creating mesh!\n" );

error:
    DEL_PTR( json_file );
    DEL_PTR( positions_v );
    DEL_PTR( texcoords_v );
    DEL_PTR( indices_v );
    if( root ) cJSON_Delete( root );
    return handle;
}

int LoadFont( ResourceManager *pRM, const char *pFile ) {
    // path
    str256 file_path;
    strcpy( file_path, FontDirectory );
    strcat( file_path, pFile );

    // load the font in 12, 20 and 32 sizes
    int f_12_handle = Renderer_createFont( file_path, 12 );
    int f_20_handle = Renderer_createFont( file_path, 20 );
    int f_32_handle = Renderer_createFont( file_path, 32 );

    check( f_12_handle >= 0, "Could not load font \"%s\" in size 12!\n", file_path );
    check( f_20_handle >= 0, "Could not load font \"%s\" in size 20!\n", file_path );
    check( f_32_handle >= 0, "Could not load font \"%s\" in size 32!\n", file_path );

    // add the three fonts sizes to the manager
    size_t f_len = strlen( pFile );
    char *f_name = byte_alloc( f_len + 4 );
    check_mem( f_name );
    strcpy( f_name, pFile );

    // size 12
    strcat( f_name, "/12" );
    u32 hash = Byte_GetHash( f_name );
    ResourceManager_addEntry( pRM, hash, f_12_handle );

    // size 20
    strcpy( f_name + f_len, "/20" );
    hash = Byte_GetHash( f_name );
    ResourceManager_addEntry( pRM, hash, f_20_handle );

    // size 32
    strcpy( f_name + f_len, "/32" );
    hash = Byte_GetHash( f_name );
    ResourceManager_addEntry( pRM, hash, f_32_handle );



    DEL_PTR( f_name );
    return f_20_handle;

error:
    return -1;
}

int ResourceManager_load( ResourceManager *pRM, ResourceType pType, const char *pFile ) {
    int handle = -1;
    char *r_name = NULL;

    if( pRM && pFile && Renderer_isInitialized() ) {
        // if type is font, search for the font with size 20 in resources.
        if( RT_Font == pType ) {
            r_name = byte_alloc( strlen( pFile ) + 4 );
            strcpy( r_name, pFile );
            strcat( r_name, "/20" );
        } else {
            r_name = byte_alloc( strlen( pFile ) + 1 );
            strcpy( r_name, pFile );
        }

        u32 hash = Byte_GetHash( r_name );

        // search if wanted resource already exist
        for( int i = 0; i < pRM->mHashes.cpt; ++i )  {
            // the array is sorted in increasing order so break if superior
            if( pRM->mHashes.data[i] > hash )
                break;
            else if( pRM->mHashes.data[i] == hash ) {
                // if we find it, return its handle
                handle = pRM->mHandles.data[i];
                log_info( "Resource \"%s\" is already present in resource manager!\n", pFile );
                return handle;
            }
        }

        // get full path to resource (in its corresponding data dir)
        str256 file_path;
        file_path[0] = 0;


        // else, load the data
        switch( pType ) {
            case RT_Texture:
                // get complete file path
                strcat( file_path, TextureDirectory );
                strcat( file_path, pFile );

                // load texture
                handle = Renderer_createTexture( file_path, true );

                // if successfully loaded, add its hash and handle to the manager
                if( handle >= 0 ) {
                    if( ResourceManager_addEntry( pRM, hash, handle ) ) {
                        log_info( "Resource \"%s\" loaded.\n", pFile );
                    } else
                        log_err( "Failed to load resource \"%s\".\n", pFile );
                }
                break;
            case RT_Shader:
                // get complete file path
                strcat( file_path, ShaderDirectory );
                strcat( file_path, pFile );

                // parse json file to get both
                if( CheckExtension( file_path, "json" ) ) {
                    handle = LoadShader( file_path );

                    // if successfully loaded, add its hash and handle to the manager
                    if( handle >= 0 ) {
                        if( ResourceManager_addEntry( pRM, hash, handle ) ) {
                            log_info( "Resource \"%s\" loaded.\n", pFile );
                        } else
                            log_err( "Failed to load resource \"%s\".\n", pFile );
                    }
                } else
                    log_err( "ResourceManager needs a .json file to load a shader!\n" );
                break;
            case RT_Mesh :
                // get complete file path
                strcat( file_path, MeshDirectory );
                strcat( file_path, pFile );

                // load mesh
                if( CheckExtension( file_path, "json" ) ) {
                    handle = LoadMesh( file_path );

                    // if successfully loaded, add its hash and handle to the manager
                    if( handle >= 0 ) {
                        if( ResourceManager_addEntry( pRM, hash, handle ) ) {
                            log_info( "Resource \"%s\" loaded.\n", pFile );
                        } else
                            log_err( "Failed to load resource \"%s\".\n", pFile );
                    }
                } else
                    log_err( "ResourceManager needs a .json file to load a mesh!\n" );

                break;
            case RT_Font:
                // get complete file path
                //strcat( file_path, FontDirectory );
                //strcat( file_path, pFile );

                // load font with freetype
                if( CheckExtension( pFile, "ttf" ) ) {
                    handle = LoadFont( pRM, pFile );

                    if( handle >= 0 ) {
                        log_info( "Resource \"%s\" loaded.\n", pFile );
                    } else
                        log_err( "Failed to load resource \"%s\".\n", pFile );
                } else
                    log_err( "ResourceManager can't handle \"%s\" as a font file!\n", pFile );

                break;
            default:
                break;
        }
    }
    DEL_PTR( r_name );
    return handle;
}

int ResourceManager_getResource( ResourceManager *pRM, const char *pFile ) {
    int handle = -1;
    if( pRM && pFile ) {
        int hash = Byte_GetHash( pFile );
        for( u32 i = 0; i < pRM->mHashes.cpt; ++i ) {
            if( pRM->mHashes.data[i] > hash )
                break;
            else if ( pRM->mHashes.data[i] == hash ) {
                handle = pRM->mHandles.data[i];
                break;
            }
        }
    }

    return handle;
}

bool ResourceManager_loadAllResources( ResourceManager *pRM ) {
    if( pRM ) {
        struct dirent *entry = NULL;

        // Load shaders
        DIR *shader_dir = opendir( ShaderDirectory );

        while( ( entry = readdir( shader_dir ) ) ) {
            const char *shader_file = entry->d_name;

            if( CheckExtension( shader_file, "json" ) )
                check( ResourceManager_load( pRM, RT_Shader, shader_file ) >= 0, "\n" );
        }

        closedir( shader_dir );

        // Load meshes
        // init Renderer VAO
        Renderer_initVao();

        DIR *mesh_dir = opendir( MeshDirectory );

        while( ( entry = readdir( mesh_dir ) ) ) {
            const char *mesh_file = entry->d_name;

            if( CheckExtension( mesh_file, "json" ) )
                check( ResourceManager_load( pRM, RT_Mesh, mesh_file ) >= 0, "\n" );
        }

        closedir( mesh_dir );

        // Load textures
        DIR *texture_dir = opendir( TextureDirectory );

        while( ( entry = readdir( texture_dir ) ) ) {
            const char *texture_file = entry->d_name;

            if( CheckExtension( texture_file, "png" ) || CheckExtension( texture_file, "jpg" ) )
                ResourceManager_load( pRM, RT_Texture, texture_file );
        }

        closedir( texture_dir );

        // Load fonts
        DIR *font_dir = opendir( FontDirectory );

        while( ( entry = readdir( font_dir ) ) ) {
            const char *font_file = entry->d_name;

            if( CheckExtension( font_file, "ttf" ) )
                ResourceManager_load( pRM, RT_Font, font_file );
        }

        closedir( font_dir );

        return true;
    }

error:
    return false;
}
