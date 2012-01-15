#include "handlemanager.h"

HandleManager *HandleManager_init( u32 pSize ) {
    HandleManager *hm = byte_alloc( sizeof( HandleManager ) );
    check_mem( hm );

    hm->mSize = pSize;

    hm->mHandles = byte_alloc( sizeof( Handle ) * pSize );
    check_mem( hm->mHandles );

    // every handle next free index point to the next handle
    for( u32 i = 0; i < pSize; ++i ) {
        hm->mHandles[i].next_free_index = i + 1;
        hm->mHandles[i].used = false;
    }

    // last handle does not have a next free index
    hm->mHandles[pSize - 1].next_free_index = -1;

    return hm;
error:
    HandleManager_destroy( hm );
    return NULL;
}

void HandleManager_destroy( HandleManager *pHm ) {
    if( pHm ) {
        DEL_PTR( pHm->mHandles );
        DEL_PTR( pHm );
    }
}

int  HandleManager_add( HandleManager *pHm, void *pData ) {
    int handle = -1;

    if( pHm ) {
        if( pHm->mFirstFreeIndex < 0 ) {
            // resize array
            u32 nextHandle = pHm->mSize;

            pHm->mHandles[nextHandle - 1].next_free_index = nextHandle;
            pHm->mFirstFreeIndex = nextHandle;
            pHm->mHandles = byte_realloc( pHm->mHandles, (pHm->mSize *= REALLOC_RATIO) * sizeof( Handle ) );
            for( u32 i = nextHandle; i < pHm->mSize; ++i ) {
                pHm->mHandles[i].next_free_index = i + 1;
                pHm->mHandles[i].used = false;
            }
            pHm->mHandles[pHm->mSize - 1].next_free_index = -1;
        }

        handle = pHm->mFirstFreeIndex;

        pHm->mHandles[handle].data = pData;
        pHm->mHandles[handle].used = true;
        pHm->mFirstFreeIndex = pHm->mHandles[handle].next_free_index;
    }     
    
    return handle;
}

void HandleManager_remove( HandleManager *pHm, u32 pHandle ) {
    if( pHm && pHandle < pHm->mSize && pHm->mHandles[pHandle].used ) {
        u32 next = pHm->mFirstFreeIndex;
        
        // first free index becomes the one at the pos of the removed data
        // 
        pHm->mFirstFreeIndex = pHandle;
        pHm->mHandles[pHandle].next_free_index = next;

        // remove the data in place
        pHm->mHandles[pHandle].data = NULL;
        pHm->mHandles[pHandle].used = false;
    }
}

void *HandleManager_get( HandleManager *pHm, u32 pHandle ) {
    if( pHm && pHandle < pHm->mSize )
        return pHm->mHandles[pHandle].data;
    return NULL;
}

void HandleManager_print( HandleManager *pHm ) {
    if( pHm ) {
        for( u32 i = 0; i < pHm->mSize; ++i )
            printf( "[%c]%p (n=%d)\n", (pHm->mHandles[i].used ? 'u' : 'x'), 
                                        pHm->mHandles[i].data, 
                                        pHm->mHandles[i].next_free_index );
        printf( "First free index : %d\n", pHm->mFirstFreeIndex );
    }
}

