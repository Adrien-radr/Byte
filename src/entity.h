#ifndef BYTE_ENTITY_HPP
#define BYTE_ENTITY_HPP

#include "matrix.h"
#include "handlemanager.h"

// ##########################################################################3
//      Entity Array 
// ##########################################################################3
    /// Data-oriented array storing all text existing in the scene
    typedef struct {
        HandleManager   *mUsed;     ///< Handlemanager saying if an index is used

        u32             *mMeshes;
        u32             *mTextures;
        u32             *mDepths;
        mat3            *mMatrices;

        u32             mCount,
                        mSize,
                        mMaxIndex;
    } EntityArray;

    /// Differents attributes of entity that can be modified
    typedef enum {
        EA_Texture,
        EA_Depth,
        EA_Matrix
    } EntityAttrib;

    /// Initialize and allocate a new EntityArray
    EntityArray *EntityArray_init( u32 pSize );

    /// Add a new entity to the entity array and returns its handle (or -1 if any error)
    int EntityArray_add( EntityArray *pEA );

    /// Remove a text from the given entity array, by its index
    void EntityArray_remove( EntityArray *pEA, u32 pIndex );

    /// Clears the whole given entity array
    void EntityArray_clear( EntityArray *pEA );

    /// Destroy and free the given entity array
    void EntityArray_destroy( EntityArray *pEA );

#endif // BYTE_ENTITY
