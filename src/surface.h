#ifndef BYTE_SURFACE_H
#define BYTE_SURFACE_H

#include "renderer.h"

/// A Surface is a texture that is bound to the camera, which means it can only be moved
/// on the camera plane.
typedef struct {
    int mTexture;
    int mString;
    int mFont;
    u32 mStringSize;

    static int mMesh;   // This is the plane on which every Surface will be rendered, which is the camera plane.
} Surface;

/// Load a texture onto the surface.
Surface* Surface_loadTexture(const char* file);


/// Set a string to be rendered if a font has been loaded.
void Surface_setString(const char*  string);

/// Set a size for the string if a font has been loaded.
void Surface_setStringSize(u32 size);

/// Load a font for further rendering of a string.
void Surface_loadFont(const char* fontName);

/// Load a font and set a string and the size on the surface.
Surface* Surface_loadText(const char* string, const char* fontName, u32 size);





#endif // BYTE_SURFACE
