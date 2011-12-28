#ifndef BYTE_CONTEXT_H
#define BYTE_CONTEXT_H

#include "common.h"
#include "vector.h"

/// GLFW Window/Context 
typedef struct s_Context {
    vec2    *mVideoModes;   ///< Video Modes width/height
    int     mVideoModesNb;  ///< Video Modes count

    vec2    mSize;          ///< Context used VideoMode
    u32     mMultiSamples;  ///< FSAA samples
} Context;

/// Context only instance
extern Context *context;




/// Initialize and returns a Context instance
/// @param pWidth : Window width
/// @param pHeight : Window height
/// @param pFullscreen : Boolean for fullscreen mode
/// @param pName : Window name
/// @param pMultiSamples : Nb of FSAA Samples
void Context_init( u32 pWidth, u32 pHeight, bool pFullscreen, const char *pName, u32 pMultiSamples );

/// Destroy a context instance
void Context_destroy();

#endif
