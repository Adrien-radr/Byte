#ifndef BYTE_CONTEXT_H
#define BYTE_CONTEXT_H

#include "common/common.h"
#include "common/matrix.h"


/// Initialize and returns a Context instance
/// @param width : Window width
/// @param height : Window height
/// @param fullscreen : Boolean for fullscreen mode
/// @param name : Window name
/// @param multi_samples : Nb of FSAA Samples
bool Context_init( u32 width, u32 height, bool fullscreen, const char *name, u32 multi_samples );

/// Destroy a context instance
void Context_destroy();

/// Returns true if the context is initialized
bool Context_isInitialized();

/// Swap context/window framebuffer
void Context_swap();

/// Returns the state of window
bool Context_isWindowOpen();

/// Returns the window size
vec2i Context_getSize();

/// Returns whether the window size has changed or not
//bool Context_sizeChanged();

/// Sets the window size
void Context_setSize( vec2i size );

/// (de)Activate Vertical synchronisation
void Context_setVSync( bool val );

/// Show or Hide the mouse cursor in the window boundaries
void Context_showCursor( bool val ); 





/// GLFW Resize callback function (defined in device.c)
extern void WindowResizeCallback( int pWidth, int pHeight );

#endif
