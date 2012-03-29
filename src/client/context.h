#ifndef BYTE_CONTEXT_H
#define BYTE_CONTEXT_H

#include "common/common.h"
#include "common/matrix.h"


/// Initialize and returns a Context instance
/// @param pWidth : Window width
/// @param pHeight : Window height
/// @param pFullscreen : Boolean for fullscreen mode
/// @param pName : Window name
/// @param pMultiSamples : Nb of FSAA Samples
bool Context_init( u32 pWidth, u32 pHeight, bool pFullscreen, const char *pName, u32 pMultiSamples );

/// Destroy a context instance
void Context_destroy();

/// Returns true if the context is initialized
bool Context_isInitialized();

/// Swap context/window framebuffer
void Context_swap();

/// Returns the state of window
bool Context_isWindowOpen();

/// Returns the window size
vec2 Context_getSize();

/// Returns whether the window size has changed or not
//bool Context_sizeChanged();

/// Sets the window size
void Context_setSize( vec2 pSize );

/// (de)Activate Vertical synchronisation
void Context_setVSync( bool pVal );

/// Show or Hide the mouse cursor in the window boundaries
void Context_showCursor( bool pVal ); 





/// GLFW Resize callback function (defined in device.c)
extern void WindowResizeCallback( int pWidth, int pHeight );

#endif
