#ifndef BYTE_CONTEXT_H
#define BYTE_CONTEXT_H

#include "common.h"
#include "vector.h"


/// Initialize and returns a Context instance
/// @param pWidth : Window width
/// @param pHeight : Window height
/// @param pFullscreen : Boolean for fullscreen mode
/// @param pName : Window name
/// @param pMultiSamples : Nb of FSAA Samples
bool Context_init( u32 pWidth, u32 pHeight, bool pFullscreen, const char *pName, u32 pMultiSamples );

/// Destroy a context instance
void Context_destroy();

/// Update the context, call this every frame
void Context_update();

/// Swap context/window framebuffer
void Context_swap();

/// (de)Activate Vertical synchronisation
void Context_setVSync( bool pVal );

/// Show or Hide the mouse cursor in the window boundaries
void Context_showCursor( bool pVal ); 

#endif
