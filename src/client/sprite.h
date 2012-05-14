#ifndef BYTE_SPRITE_H
#define BYTE_SPRITE_H

#include "common/handlemanager.h"
#include "common/matrix.h"
#include "common/anim.h"

/// Data-oriented array storing all sprites existing in the scene
typedef struct {
    HandleManager   *mUsed;     ///< Handlemanager telling if an index is used

    u32             *mMeshes;
    u32             *mTextures0;    ///< Texture to bind on target 0
    int             *mTextures1;    ///< Texture to bind on target 1 (-1 = no tex)
    mat3            *mAttributes;   ///< Attributes of sprite, grouped in a matrix
                                    ///<  pos.x      |   pos.y    | depth
                                    ///<  size.x     |  size.y    |   X  
                                    ///<  frameoff.x | frameoff.y |   X  
    Anim            *anims;         ///< current anim of sprite (cpy of one of AnimMgr)

    u32             mCount,
                    mSize,
                    mMaxIndex;
} SpriteArray;

/// Differents attributes of a sprite that can be modified
typedef enum {
    SA_Texture0,
    SA_Texture1,
    SA_Depth,
    SA_Position,
    SA_Animation
} SpriteAttrib;

/// Initialize and allocate a new SpriteArray
SpriteArray *SpriteArray_init( u32 pSize );

/// Add a new sprite to the sprite array and returns its handle (or -1 if any error)
int SpriteArray_add( SpriteArray *pSA );

/// Remove a sprite from the given sprite array, by its index
void SpriteArray_remove( SpriteArray *pSA, u32 pIndex );

/// Clears the whole given sprite array
void SpriteArray_clear( SpriteArray *pSA );

/// Destroy and free the given sprite array
void SpriteArray_destroy( SpriteArray *pSA );

#endif // BYTE_SPRITE
