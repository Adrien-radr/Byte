#include "entity.h"

EntityArray *EntityArray_init( u32 pSize ) {
    EntityArray *arr = byte_alloc( sizeof( EntityArray ) );
    check_mem( arr );

    arr->mUsed = HandleManager_init( pSize );
    arr->mMeshes = byte_alloc( pSize * sizeof( u32 ) );
    arr->mTextures = byte_alloc( pSize * sizeof( u32 ) );
    arr->mDepths = byte_alloc( pSize * sizeof( u32 ) );
    arr->mMatrices = byte_alloc( pSize * sizeof( mat3 ) );

    arr->mSize = pSize;

error:
    return arr;
}

int EntityArray_add( EntityArray *arr ) {
    int handle = -1;
    if( arr ) {
        handle = HandleManager_addHandle( arr->mUsed, 1 );

        if( handle >= 0 ) {
            // resize our entity array if the handle manager had to be resized
            if( arr->mUsed->mSize != arr->mSize ) {
                arr->mSize = arr->mUsed->mSize;
                arr->mMeshes = byte_realloc( arr->mMeshes, arr->mSize * sizeof( u32 ) );
                arr->mTextures = byte_realloc( arr->mTextures, arr->mSize * sizeof( u32 ) );
                arr->mDepths = byte_realloc( arr->mDepths, arr->mSize * sizeof( u32 ) );
                arr->mMatrices = byte_realloc( arr->mMatrices, arr->mSize * sizeof( mat3 ) );
            }

            ++arr->mMaxIndex;
            ++arr->mCount;
        }
    }

    return handle;
}

void EntityArray_remove( EntityArray *arr, u32 pIndex ) {
    if( arr && pIndex < arr->mUsed->mMaxIndex )  {
        HandleManager_remove( arr->mUsed, pIndex );
        --arr->mCount;
    }
}

void EntityArray_clear( EntityArray *arr ) {
    if( arr ) {
        arr->mMaxIndex = 0;
        HandleManager_clear( arr->mUsed );
    }
}

void EntityArray_destroy( EntityArray *arr ) {
    if( arr ) {
        HandleManager_destroy( arr->mUsed );
        DEL_PTR( arr->mTextures );
        DEL_PTR( arr->mMeshes );
        DEL_PTR( arr->mDepths );
        DEL_PTR( arr->mMatrices );
        DEL_PTR( arr );
    }
}
