#ifndef BYTE_WIDGET_H
#define BYTE_WIDGET_H

#include "handlemanager.h"
#include "vector.h"
#include "matrix.h"
#include "text.h"

//  Those structs are what we have to use as parameters in the creation of a new widget.
typedef struct {
    u32 mMesh;
    u32 mTexture;
    mat3* mMM;
    vec2 mBounds;
}   WidgetSpriteAttributes;

typedef struct {
    const Font* mFont;
    Color mColor;
    vec2 mPosition;
    vec2 mBounds;
}   WidgetTextAttributes;

typedef struct {
    u32 mMesh;
    u32 mTexture;
    mat3* mMM;
    const Font* mFont;
    Color mColor;
    vec2 mBounds;
}   WidgetButtonAttributes;


typedef enum {
    WT_Master,  //  Master of all widgets.
    WT_Text,    //  A floating text.
    WT_Sprite,  //  A floating sprite.
    WT_Button  //  Element containing a text and an entity.
}   WidgetType;

///< The widget structure is basically the mix between an Entity and a Text.
///< It has bounds so we can track mouse interaction.
typedef struct {
    HandleManager *mUsed;
    HandleManager *mEntityUsed;
    HandleManager *mTextUsed;

    u32             *mTextureMeshes;       // Mesh for the texture.
    u32             *mTextMeshes;           //  Mesh for the text.
    u32             *mTextures;
    int             *mDepths;
    mat3           **mMatrices;   //  For each widget there is a pointer to a matrix.
    vec2            *mBounds;      //  Width and Height of each widget, defines their bounding box.

    const Font      **mFonts;   //  For each widget there is a pointer to a Font.
    Color           *mColors;
    vec2            *mTextPositions;

    char            **mStrings;

    void            **mCallbacks;
    HandleManager **mChildren;   //  This will track the handle of each widget's children

    WidgetType *mWidgetTypes;

    u32 mCount,
          mSize,
          mMaxIndex;
} WidgetArray;


typedef enum {
    WA_Texture,
    WA_Matrix,
    WA_Bounds,
    WA_Depth,
    WA_Font,
    WA_Color,
    WA_TextPosition,
    WA_String,
    WA_Callback
} WidgetAttrib;

WidgetArray* WidgetArray_init(u32 pSize);

int WidgetArray_add(WidgetArray* pWA, WidgetType pWT, int pMother);

void WidgetArray_addChild( WidgetArray* pWA, u32 pMother, u32 pChild );

void WidgetArray_remove(WidgetArray* pWA, u32 pIndex);

void WidgetArray_clear(WidgetArray* pWA);

void WidgetArray_destroy(WidgetArray* pWA);

#endif // BYTE_WIDGET
