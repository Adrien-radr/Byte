#include "resource.h"
#include "renderer.h"

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


int ResourceManager_load( ResourceManager *pRM, ResourceType pType, const char *pFile ) {
    int handle = -1;
    if( pRM && pFile && Renderer_isInitialized() ) {
        u32 hash = GetHash( pFile );

        // search if wanted resource already exist
        for( int i = 0; i < pRM->mHashes.cpt; ++i ) 
            if( pRM->mHashes.data[i] == hash ) {
                handle = pRM->mHandles.data[i];
                break;
            }
        // if we find it, return its handle
        if( handle >= 0 ) { 
            log_info( "Resource \"%s\" is already present in resource manager!\n", pFile );
            return handle;
        }

        // else, load the data
        switch( pType ) {
            case RT_Texture:
                handle = Renderer_createTexture( pFile, true ); 
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
