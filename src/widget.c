#include "widget.h"
#include "renderer.h"

WidgetArray* WidgetArray_init( u32 pSize ){
    WidgetArray *arr = byte_alloc( sizeof( WidgetArray ) );
    check_mem( arr );

    arr->mEntityUsed = HandleManager_init( pSize );
    arr->mTextUsed = HandleManager_init( pSize );
    arr->mTextureMeshes = byte_alloc( pSize * sizeof( u32 ) );
    arr->mTextMeshes = byte_alloc( pSize * sizeof( u32 ) );
    arr->mTextures = byte_alloc( pSize * sizeof( u32* ) );
    arr->mDepth = -2;
    arr->mMatrices = byte_alloc( pSize * sizeof( mat3 ) );
    arr->mBounds = byte_alloc( pSize * sizeof( vec2 ) );
    arr->mFonts = byte_alloc( pSize * sizeof( Font* ) );
    arr->mColors = byte_alloc( pSize * sizeof( Color ) );
    arr->mTextPositions = byte_alloc( pSize * sizeof( vec2 ) );
    arr->mStrings = byte_alloc( pSize * sizeof( char* ) );
    arr->mWidgetTypes = byte_alloc( pSize * sizeof( WidgetType ) );


    arr->mSize = pSize;

error:
    return arr;
}

int WidgetArray_add( WidgetArray* arr, WidgetType pWT ) {
  int handle = -1;
    if( arr ) {
        switch( pWT ) {
            case WT_Text :
                {
                    handle = HandleManager_addHandle( arr->mTextUsed, 1 );

                    if( handle >= 0 ) {
                        // resize our entity array if the handle manager had to be resized
                        if( arr->mTextUsed->mSize != arr->mSize ) {
                                arr->mTextUsed = HandleManager_init( arr->mSize );

                                arr->mTextMeshes = byte_realloc( arr->mTextMeshes, arr->mSize * sizeof( u32 ) );
                                arr->mDepth = -1;
                                arr->mMatrices = byte_realloc( arr->mMatrices, arr->mSize * sizeof( mat3* ) );
                                arr->mBounds = byte_realloc( arr->mBounds, arr->mSize * sizeof( vec2 ) );
                                arr->mFonts = byte_realloc( arr->mFonts, arr->mSize * sizeof( Font* ) );
                                arr->mColors = byte_realloc( arr->mColors, arr->mSize * sizeof( Color ) );
                                arr->mTextPositions = byte_realloc( arr->mTextPositions, arr->mSize * sizeof( vec2 ) );
                                arr->mStrings = byte_realloc( arr->mStrings, arr->mSize * sizeof( char* ) );
                                arr->mWidgetTypes = byte_realloc( arr->mWidgetTypes, arr->mSize * sizeof( WidgetType ) );
                        }

                        // create mesh used by widget
                        check( (arr->mTextMeshes[handle] = Renderer_createDynamicMesh()) >= 0, "Failed to create mesh of Widget!\n" );


                        ++arr->mMaxIndex;
                        ++arr->mCount;

                        return handle;
                    }
                }
                break;
            case WT_Sprite :
                {
                    handle = HandleManager_addHandle( arr->mEntityUsed, 1 );

                    if( handle >= 0 ) {
                        // resize our entity array if the handle manager had to be resized
                        if( arr->mEntityUsed->mSize != arr->mSize ) {
                                arr->mEntityUsed = HandleManager_init( arr->mSize );
                                arr->mTextures = byte_realloc( arr->mTextures, arr->mSize * sizeof( u32 ) );
                                arr->mTextureMeshes = byte_realloc( arr->mTextureMeshes, arr->mSize * sizeof( u32 ) );
                                arr->mDepth = -1;
                                arr->mMatrices = byte_realloc( arr->mMatrices, arr->mSize * sizeof( mat3* ) );
                                arr->mBounds = byte_realloc( arr->mBounds, arr->mSize * sizeof( vec2 ) );
                                arr->mWidgetTypes = byte_realloc( arr->mWidgetTypes, arr->mSize * sizeof( WidgetType ) );

                        }

                        ++arr->mMaxIndex;
                        ++arr->mCount;

                        return handle;
                    }
                }
                break;
            case WT_Button :
                {
                    handle = HandleManager_addHandle( arr->mEntityUsed, 1 );
                    if( handle < 0 )
                        return -1;  //  If the handlemanager could not create the handle for the entity, we can't continue.
                    handle = HandleManager_addHandle( arr->mTextUsed, 1 );

                    if( handle >= 0 ) {
                        // resize our entity array if the handle manager had to be resized
                        if( arr->mTextUsed->mSize != arr->mSize ) {
                                arr->mTextUsed = HandleManager_init( arr->mSize );
                                arr->mEntityUsed = HandleManager_init( arr->mSize );
                                arr->mTextureMeshes = byte_realloc( arr->mTextureMeshes, arr->mSize * sizeof( u32 ) );
                                arr->mTextMeshes = byte_realloc( arr->mTextMeshes, arr->mSize * sizeof( u32 ) );
                                arr->mDepth = -1;
                                arr->mMatrices = byte_realloc( arr->mMatrices, arr->mSize * sizeof( mat3* ) );
                                arr->mBounds = byte_realloc( arr->mBounds, arr->mSize * sizeof( vec2 ) );
                                arr->mFonts = byte_realloc( arr->mFonts, arr->mSize * sizeof( Font* ) );
                                arr->mColors = byte_realloc( arr->mColors, arr->mSize * sizeof( Color ) );
                                arr->mTextPositions = byte_realloc( arr->mTextPositions, arr->mSize * sizeof( vec2 ) );
                                arr->mStrings = byte_realloc( arr->mStrings, arr->mSize * sizeof( char* ) );
                                arr->mWidgetTypes = byte_realloc( arr->mWidgetTypes, arr->mSize * sizeof( WidgetType ) );

                        }

                        // create mesh used by widget
                        check( (arr->mTextMeshes[handle] = Renderer_createDynamicMesh()) >= 0, "Failed to create mesh of Widget!\n" );


                        ++arr->mMaxIndex;
                        ++arr->mCount;

                        return handle;
                    }
                }
                break;
        }
    }

error:
    if( handle >= 0 ) {
        HandleManager_remove( arr->mEntityUsed, handle );
        HandleManager_remove( arr->mTextUsed, handle );
    }
    return -1;
}

void WidgetArray_remove( WidgetArray *arr, u32 pIndex ) {
    if( arr && pIndex < arr->mEntityUsed->mMaxIndex )  {
        if( HandleManager_isUsed( arr->mEntityUsed, pIndex ))
            HandleManager_remove( arr->mEntityUsed, pIndex );
        if( HandleManager_isUsed( arr->mTextUsed, pIndex ))
            HandleManager_remove( arr->mTextUsed, pIndex );
        DEL_PTR( arr->mStrings[pIndex] );
        --arr->mCount;
    }
}

void WidgetArray_clear( WidgetArray *arr ) {
    if( arr ) {
        arr->mMaxIndex = 0;
        HandleManager_clear( arr->mEntityUsed );
        HandleManager_clear( arr->mTextUsed );
        for( u32 i = 0; i < arr->mMaxIndex; ++i ) {
            DEL_PTR( arr->mStrings[i] );
        }
    }
}

void WidgetArray_destroy( WidgetArray *arr ) {
    if( arr ) {
        HandleManager_destroy( arr->mEntityUsed );
        HandleManager_destroy( arr->mTextUsed );
        DEL_PTR( arr->mTextureMeshes );
        DEL_PTR( arr->mTextMeshes);
        DEL_PTR( arr->mTextures );
        DEL_PTR( arr->mMatrices );
        DEL_PTR( arr->mBounds );
        DEL_PTR( arr->mFonts );
        DEL_PTR( arr->mColors );
        DEL_PTR( arr->mTextPositions );
        for( u32 i = 0; i < arr->mMaxIndex; ++i ) {
            DEL_PTR( arr->mStrings[i] );
        }
        DEL_PTR( arr->mStrings );
        DEL_PTR( arr->mWidgetTypes );
        DEL_PTR( arr );
    }
}
