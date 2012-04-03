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
        hm->mHandles[i].handle = -1;
        hm->mHandles[i].data = NULL;
        hm->mHandles[i].manage = false;
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
        // destroy managed data
        for( u32 i = 0; i < pHm->mSize; ++i )
            if( pHm->mHandles[i].manage )
                DEL_PTR( pHm->mHandles[i].data );
        DEL_PTR( pHm->mHandles );
        DEL_PTR( pHm );
    }
}

void Resize( HandleManager *pHm ) {
    // resize array
    u32 nextHandle = pHm->mSize;

    pHm->mHandles[nextHandle - 1].next_free_index = nextHandle;
    pHm->mFirstFreeIndex = nextHandle;
    pHm->mHandles = byte_realloc( pHm->mHandles, (pHm->mSize *= REALLOC_RATIO) * sizeof( Handle ) );
    for( u32 i = nextHandle; i < pHm->mSize; ++i ) {
        pHm->mHandles[i].next_free_index = i + 1;
        pHm->mHandles[i].handle = -1;
        pHm->mHandles[i].data = NULL;
        pHm->mHandles[i].used = false;
        pHm->mHandles[i].manage = false;
    }
    pHm->mHandles[pHm->mSize - 1].next_free_index = -1;
}

int  HandleManager_addData( HandleManager *pHm, void *pData, bool manage, u32 data_size ) {
    int handle = -1;

    if( pHm ) {
        if( pHm->mFirstFreeIndex < 0 ) 
            Resize( pHm );

        handle = pHm->mFirstFreeIndex;

        // if we need to manage it, alloc it. else, just cpy pointer
        if( manage ) {
            pHm->mHandles[handle].data = byte_alloc( data_size );
            memcpy( pHm->mHandles[handle].data, pData, data_size );
        } else
            pHm->mHandles[handle].data = pData;

        pHm->mHandles[handle].manage = manage;
        pHm->mHandles[handle].used = true;
        pHm->mFirstFreeIndex = pHm->mHandles[handle].next_free_index;

        ++pHm->mCount;
        ++pHm->mMaxIndex;
    }     
    
    return handle;
}

int  HandleManager_addHandle( HandleManager *pHm, u32 pData ) {
    int handle = -1;

    if( pHm ) {
        if( pHm->mFirstFreeIndex < 0 ) 
            Resize( pHm );

        handle = pHm->mFirstFreeIndex;

        pHm->mHandles[handle].handle = pData;
        pHm->mHandles[handle].used = true;
        pHm->mFirstFreeIndex = pHm->mHandles[handle].next_free_index;

        ++pHm->mCount;
        ++pHm->mMaxIndex;
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
        if( pHm->mHandles[pHandle].manage )
            DEL_PTR( pHm->mHandles[pHandle].data );
        else
            pHm->mHandles[pHandle].data = NULL;
        pHm->mHandles[pHandle].handle = -1;
        pHm->mHandles[pHandle].used = false;
        pHm->mHandles[pHandle].manage = false;

        --pHm->mCount;
    }
}

void *HandleManager_getData( HandleManager *pHm, u32 pHandle ) {
    if( pHm && pHandle < pHm->mSize )
        return pHm->mHandles[pHandle].data;
    return NULL;
}

int HandleManager_getHandle( HandleManager *pHm, u32 pHandle ) {
    if( pHm && pHandle < pHm->mSize )
        return pHm->mHandles[pHandle].handle;
    return -1;
}

bool HandleManager_isUsed( HandleManager *pHm, u32 pHandle ) {
    if( pHm && pHandle < pHm->mMaxIndex )
        return pHm->mHandles[pHandle].used;
    return false;
}

void HandleManager_clear( HandleManager *pHm ) {
    if( pHm ) {
        pHm->mCount = pHm->mFirstFreeIndex = pHm->mMaxIndex = 0;
        for( u32 i = 0; i < pHm->mSize; ++i ) {
            pHm->mHandles[i].next_free_index = i + 1;
            pHm->mHandles[i].used = false;
            if( pHm->mHandles[i].manage )
                DEL_PTR( pHm->mHandles[i].data );
            else
                pHm->mHandles[i].data = NULL;
            pHm->mHandles[i].manage = false;
        }

        // last handle does not have a next free index
        pHm->mHandles[pHm->mSize - 1].next_free_index = -1;

    }
}

void HandleManager_print( HandleManager *pHm ) {
    if( pHm ) {
        for( u32 i = 0; i < pHm->mMaxIndex; ++i )
            printf( "[%c] {%p[%c]||%d} (n=%d)\n", (pHm->mHandles[i].used ? 'u' : 'x'), 
                                              pHm->mHandles[i].data, 
                                              pHm->mHandles[i].manage ? 'm' : 'u',
                                              pHm->mHandles[i].handle,
                                              pHm->mHandles[i].next_free_index );
        printf( "First free index : %d\n", pHm->mFirstFreeIndex );
    }
}

