#ifndef BYTE_TEXTURE_HPP
#define BYTE_TEXTURE_HPP

#include "common.h"

typedef struct {
    u32 mID;
} Texture;

/// Create and return a new texture
Texture *Texture_new();

/// Destroy and free a given texture
void Texture_destroy( Texture *pTexture );

/// Load a given texture file and store it in given texture handle
/// @param pMipmaps : true if mipmap construction is needed
/// @return : true if everything went well, false otherwise.
bool Texture_loadFromFile( Texture *pTexture, const char *pFile, bool pMipmaps );

/// Binds the texture to the given target ( -1 for NO target switching )
void Texture_bind( Texture *pTexture, int pTarget );

#endif // BYTE_TEXTURE
