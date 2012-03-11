#ifndef BYTE_SPRITE_HPP
#define BYTE_SPRITE_HPP

#include "matrix.h"
#include "handlemanager.h"

/// Data-oriented array storing all sprites existing in the scene
typedef struct {
    HandleManager   *mUsed;     ///< Handlemanager telling if an index is used

    u32             *mMeshes;
    u32             *mTextures;
    u32             *mDepths;
    mat3            *mMatrices;

    u32             mCount,
                    mSize,
                    mMaxIndex;
} SpriteArray;

/// Differents attributes of a sprite that can be modified
typedef enum {
    SA_Texture,
    SA_Depth,
    SA_Matrix
} SpriteAttrib;

/// Initialize and allocate a new SpriteArray
SpriteArray *SpriteArray_init( u32 pSize );

/// Add a new sprite to the sprite array and returns its handle (or -1 if any error)
int SpriteArray_add( SpriteArray *pSA );

/// Remove a text from the given sprite array, by its index
void SpriteArray_remove( SpriteArray *pSA, u32 pIndex );

/// Clears the whole given sprite array
void SpriteArray_clear( SpriteArray *pSA );

/// Destroy and free the given sprite array
void SpriteArray_destroy( SpriteArray *pSA );

#endif // BYTE_SPRITE
