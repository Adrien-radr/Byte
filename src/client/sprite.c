#include "sprite.h"

SpriteArray *SpriteArray_init( u32 pSize ) {
    SpriteArray *arr = byte_alloc( sizeof( SpriteArray ) );
    check_mem( arr );

    arr->mUsed = HandleManager_init( pSize );
    arr->mMeshes = byte_alloc( pSize * sizeof( u32 ) );
    arr->mTextures0 = byte_alloc( pSize * sizeof( u32 ) );
    arr->mTextures1 = byte_alloc( pSize * sizeof( int ) );
    arr->mDepths = byte_alloc( pSize * sizeof( u32 ) );
    arr->mMatrices = byte_alloc( pSize * sizeof( mat3* ) );

    arr->mSize = pSize;

error:
    return arr;
}

int SpriteArray_add( SpriteArray *arr ) {
    int handle = -1;
    if( arr ) {
        handle = HandleManager_addHandle( arr->mUsed, 1 );

        if( handle >= 0 ) {
            // resize our sprite array if the handle manager had to be resized
            if( arr->mUsed->mSize != arr->mSize ) {
                arr->mSize = arr->mUsed->mSize;
                arr->mMeshes = byte_realloc( arr->mMeshes, arr->mSize * sizeof( u32 ) );
                arr->mTextures0 = byte_realloc( arr->mTextures0, arr->mSize * sizeof( u32 ) );
                arr->mTextures1 = byte_realloc( arr->mTextures1, arr->mSize * sizeof( int ) );
                arr->mDepths = byte_realloc( arr->mDepths, arr->mSize * sizeof( u32 ) );
                arr->mMatrices = byte_realloc( arr->mMatrices, arr->mSize * sizeof( mat3* ) );
            }

            ++arr->mMaxIndex;
            ++arr->mCount;
        }
    }

    return handle;
}

void SpriteArray_remove( SpriteArray *arr, u32 pIndex ) {
    if( arr && pIndex < arr->mUsed->mMaxIndex )  {
        HandleManager_remove( arr->mUsed, pIndex );
        --arr->mCount;
    }
}

void SpriteArray_clear( SpriteArray *arr ) {
    if( arr ) {
        HandleManager_clear( arr->mUsed );
        arr->mMaxIndex = 0;
        arr->mCount = 0;
    }
}

void SpriteArray_destroy( SpriteArray *arr ) {
    if( arr ) {
        HandleManager_destroy( arr->mUsed );
        DEL_PTR( arr->mTextures0 );
        DEL_PTR( arr->mTextures1 );
        DEL_PTR( arr->mMeshes );
        DEL_PTR( arr->mDepths );
        DEL_PTR( arr->mMatrices );
        DEL_PTR( arr );
    }
}
