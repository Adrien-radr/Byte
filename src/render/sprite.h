#ifndef BYTE_SPRITE_H
#define BYTE_SPRITE_H

#include "common/handlemanager.h"
#include "common/matrix.h"
#include "anim.h"

/// A Sprite is the visual representation  of a dynamic actor rendered in game
typedef struct {
    int     mesh_id;            ///< Mesh resource ID

    int     tex_n;              ///< Number of used textures
    int     tex_id[2];          ///< Textures resource IDs. 0:albedo
                                ///<                        1:heightmap
    vec2    texcoords_size;     ///< Size of sprite on texture atlas [0..1]^2
    vec2    mesh_size;          ///< Size of sprite globally

    Anim    *animation;         ///< Current animation of Sprite


    vec2    position;           ///< Global position in pixels
    int     depth;              ///< Sprite onscreen depth

    int     used_sprite;        ///< Sprite ID in scene (-1 if none)
} Sprite;

// Array SpritesArray with Sprite data type
SimpleArrayDecl( Sprite, Sprites )

/// Copy a Sprite in another (only textures/sizes references)
void Sprite_cpy( Sprite *dst, const Sprite *src );

/// Loads an Sprite from a JSON file and create its assets
bool Sprite_load( Sprite *s, const char *file );

/// Sets a sprite position from a map tile, and update scene if in it
void Sprite_setPosition( Sprite *s, const vec2i *loc );

/// Sets a sprite current animation
void Sprite_setAnimation( Sprite *s, Animation anim );






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
