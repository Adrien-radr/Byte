#include "widget.h"
#include "renderer.h"

WidgetArray* WidgetArray_init( u32 pSize ){
    WidgetArray *arr = byte_alloc( sizeof( WidgetArray ) );
    check_mem( arr );


    arr->mUsed = HandleManager_init( pSize );
    arr->mEntityUsed = HandleManager_init( pSize );
    arr->mTextUsed = HandleManager_init( pSize );
    arr->mTextureMeshes = byte_alloc( pSize * sizeof( u32 ) );
    arr->mTextMeshes = byte_alloc( pSize * sizeof( u32 ) );
    arr->mTextures = byte_alloc( pSize * sizeof( u32* ) );
    arr->mDepths = byte_alloc( pSize * sizeof( int ) );
    arr->mMatrices = byte_alloc( pSize * sizeof( mat3 ) );
    arr->mBounds = byte_alloc( pSize * sizeof( vec2 ) );
    arr->mFonts = byte_alloc( pSize * sizeof( Font* ) );
    arr->mColors = byte_alloc( pSize * sizeof( Color ) );
    arr->mTextPositions = byte_alloc( pSize * sizeof( vec2 ) );
    arr->mStrings = byte_alloc( pSize * sizeof( char* ) );
    arr->mChildren = byte_alloc( pSize * sizeof( HandleManager ) );
    for( u32 i = 0; i < pSize - 1; i++ )
        arr->mChildren[i] = HandleManager_init( 1 );   // We set a default value of 1 child per widget, but there can be more or less.

    arr->mWidgetTypes = byte_alloc( pSize * sizeof( WidgetType ) );


    arr->mSize = pSize;

error:
    return arr;
}

int WidgetArray_add( WidgetArray* arr, WidgetType pWT, int pMother ) {
  int handle = -1;
    if( arr ) {
        switch( pWT ) {
            case WT_Master :
                {
                    handle = HandleManager_addHandle( arr->mUsed, 1 );
                    HandleManager_addHandle( arr->mEntityUsed, 0 ); //  We won't use an entity, so we set its handle to 0
                    HandleManager_addHandle( arr->mTextUsed, 0 );   //  Same for the text

                    if( handle >= 0 ) {
                        // resize our entity array if the handle manager had to be resized
                        if( arr->mUsed->mSize != arr->mSize ) {
                                arr->mSize = arr->mUsed->mSize;
                                arr->mWidgetTypes = byte_realloc( arr->mWidgetTypes, arr->mSize * sizeof( WidgetType ) );
                                arr->mChildren = byte_realloc( arr->mChildren, arr->mSize * sizeof( u32 ) );
                        }

                        ++arr->mMaxIndex;
                        ++arr->mCount;
                    }

                    return handle;
                }
                break;
            case WT_Text :
                {
                    handle = HandleManager_addHandle( arr->mUsed, 1 );
                    HandleManager_addHandle( arr->mEntityUsed, 0 ); //  We won't use an entity
                    HandleManager_addHandle( arr->mTextUsed, 1 );   //  We will use a text

                    if( handle >= 0 ) {
                        // resize our entity array if the handle manager had to be resized
                        if( arr->mUsed->mSize != arr->mSize ) {
                                arr->mSize = arr->mUsed->mSize;
                                arr->mTextMeshes = byte_realloc( arr->mTextMeshes, arr->mSize * sizeof( u32 ) );
                                arr->mDepths = byte_realloc( arr->mDepths, arr->mSize * sizeof( int ) );
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
                    handle = HandleManager_addHandle( arr->mUsed, 1 );
                    HandleManager_addHandle( arr->mEntityUsed, 1 ); //  We will use an entity
                    HandleManager_addHandle( arr->mTextUsed, 0 );   //  We won't use a text

                    if( handle >= 0 ) {
                        // resize our entity array if the handle manager had to be resized
                        if( arr->mUsed->mSize != arr->mSize ) {
                                arr->mSize = arr->mUsed->mSize;
                                arr->mTextures = byte_realloc( arr->mTextures, arr->mSize * sizeof( u32 ) );
                                arr->mTextureMeshes = byte_realloc( arr->mTextureMeshes, arr->mSize * sizeof( u32 ) );
                                arr->mDepths = byte_realloc( arr->mDepths, arr->mSize * sizeof( int ) );
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
                    handle = HandleManager_addHandle( arr->mUsed, 1 );
                    HandleManager_addHandle( arr->mEntityUsed, 1 ); //  We will use both an entity and a text
                    HandleManager_addHandle( arr->mTextUsed, 1 );

                    if( handle >= 0 ) {
                        // resize our entity array if the handle manager had to be resized
                        if( arr->mTextUsed->mSize != arr->mSize || arr->mEntityUsed->mSize != arr->mSize ) {
                                arr->mSize = arr->mEntityUsed->mSize;
                                arr->mTextureMeshes = byte_realloc( arr->mTextureMeshes, arr->mSize * sizeof( u32 ) );
                                arr->mTextMeshes = byte_realloc( arr->mTextMeshes, arr->mSize * sizeof( u32 ) );
                                arr->mDepths = byte_realloc( arr->mDepths, arr->mSize * sizeof( int ) );
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

void WidgetArray_addChild( WidgetArray* pWA, u32 pMother, u32 pChild ) {
    if( HandleManager_isUsed( pWA->mUsed, pMother ) && HandleManager_isUsed( pWA->mUsed, pChild ) ) {
        HandleManager_addHandle( pWA->mChildren[pMother], pChild );
    }
}

void WidgetArray_removeChild( WidgetArray* pWA, u32 pMother, u32 pChild ) {
    if( HandleManager_isUsed( pWA->mUsed, pMother ) && HandleManager_isUsed( pWA->mUsed, pChild ) ) {
        if( HandleManager_isUsed( pWA->mChildren[pMother], pChild ) ) {
           HandleManager_remove( pWA->mChildren[pMother], pChild );
        }
    }
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
        arr->mCount = 0;
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
