#include <GL/glfw.h>

#include "context.h"

/// Context only instance definition
Context *context = NULL;



/// Default backup Width and Height
static vec2 defaultWinSize = { .x = 800, .y = 600 };



/// Window Callback Function
void GLFWCALL windowCallbackFunction( int pWidth, int pHeight ) {
    if( context ) {
        context->mSize.x = pWidth;
        context->mSize.y = pHeight;
    }
}

void Context_init( u32 pWidth, u32 pHeight, bool pFullscreen, const char *pName, u32 pMultiSamples ) {
    if( context ) {
        log_err( "Context already created!\n" );
        return;
    }

    if( pWidth < 100 || pHeight < 100 ) {
        log_err( "Width and Height of window must be larger than %dpx\n", 100 );
        return;
    }


    context = (Context*)malloc( sizeof( *context ) );
    check_mem( context );

    // Initialize GLFW 
    check( glfwInit(), "Could not initialize GLFW\n" );

    // Get available videomodes
    GLFWvidmode vidmodes[64];

    context->mVideoModesNb = glfwGetVideoModes( vidmodes, 64 );
    context->mVideoModes = malloc( sizeof(vec2) * context->mVideoModesNb );
    bool resolutionFound = false;

    for( int i = 0; i < context->mVideoModesNb; ++i ) {
        context->mVideoModes[i].x = vidmodes[i].Width;
        context->mVideoModes[i].y = vidmodes[i].Height;

        if( !resolutionFound && 
                context->mVideoModes[i].x == pWidth && context->mVideoModes[i].y == pHeight )
            resolutionFound = true;
    }

    if( resolutionFound || !pFullscreen )
        glfwSetWindowSize( pWidth, pHeight );
    else {
        log_warn( "VideoMode %dx%d not available, back to %dx%d\n", pWidth, pHeight, (int)defaultWinSize.x, (int)defaultWinSize.y );
        pWidth = defaultWinSize.x;
        pHeight = defaultWinSize.y;
        glfwSetWindowSize( pWidth, pHeight );
    }

    context->mSize.x = pWidth;
    context->mSize.y = pHeight;


    // Set window name
    check( pName, "Window name invalid\n" );
    glfwSetWindowTitle( pName );

    // OpenGL profile
    glfwOpenWindowHint( GLFW_OPENGL_VERSION_MAJOR, 3 );
    glfwOpenWindowHint( GLFW_OPENGL_VERSION_MINOR, 2 );

    // Multisamples
    u32 ms = pMultiSamples;
    if( ms != 2 || ms != 4 || ms != 8 )
        ms = 0;
    
    context->mMultiSamples = ms;
    glfwOpenWindowHint( GLFW_FSAA_SAMPLES, ms );

    // Resize callback function
    glfwSetWindowSizeCallback( windowCallbackFunction );

    int init = glfwOpenWindow(  context->mSize.x, context->mSize.y, 
                                8, 8, 8, 0,
                                24, 8, pFullscreen ? GLFW_FULLSCREEN : GLFW_WINDOW );

    check( init, "Could not initialize GLFW Window\n" );

    log_info( "GLFW Window succesfully initialized!\n" );
    return;
        
error:
    if( context ) Context_destroy( context );
}

void Context_destroy() {
    glfwTerminate();

    if( context )
        DEL_PTR( context->mVideoModes );
    DEL_PTR( context );
}

