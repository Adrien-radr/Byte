#ifndef BYTE_CONTEXT_H
#define BYTE_CONTEXT_H

#include "common.h"
#include "matrix.h"


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

/// Returns the state of window
bool Context_isWindowOpen();

/// Sets the window resize callback function
void Context_setResizeCallback( void (*pFunc)() );

/// (de)Activate Vertical synchronisation
void Context_setVSync( bool pVal );

/// Show or Hide the mouse cursor in the window boundaries
void Context_showCursor( bool pVal ); 


/// Returns the context projection matrix
const mat3* Context_getProjectionMatrix();


#endif
