#include <GL/glfw.h>

#include "context.h"
#include "event.h"
#include "clock.h"

/// GLFW Window/Context 
typedef struct s_Context {
    vec2    *mVideoModes;   ///< Video Modes width/height
    int     mVideoModesNb;  ///< Video Modes count

    vec2    mSize;          ///< Context used VideoMode
    u32     mMultiSamples;  ///< FSAA samples


    Clock   mClock;         ///< Clock accumulating time since context creation
    f32     mCurrTime;      ///< Current time
} Context;

/// Context only instance definition
Context *context = NULL;



/// Default backup Width and Height
static vec2 defaultWinSize = { .x = 800, .y = 600 };



/// Window Callback Function
void GLFWCALL WindowResizeCallback( int pWidth, int pHeight ) {
    if( context ) {
        context->mSize.x = pWidth;
        context->mSize.y = pHeight;
    }
}

bool Context_init( u32 pWidth, u32 pHeight, bool pFullscreen, const char *pName, u32 pMultiSamples ) {
    check( !context, "Context already created!\n" );
    check( pWidth >= 100 && pHeight >= 100, "Width and Height of window must be larger than %dpx\n", 100 );


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


    int init = glfwOpenWindow(  context->mSize.x, context->mSize.y, 
                                8, 8, 8, 0,
                                24, 8, pFullscreen ? GLFW_FULLSCREEN : GLFW_WINDOW );

    check( init, "Could not initialize GLFW Window\n" );

    // set glfw callback functions
        glfwSetWindowSizeCallback( WindowResizeCallback );
        glfwSetKeyCallback( KeyPressedCallback );
        glfwSetCharCallback( CharPressedCallback );
        glfwSetMouseButtonCallback( MousePressedCallback );
        glfwSetMouseWheelCallback( MouseWheelCallback );
        glfwSetMousePosCallback( MouseMovedCallback );


    // reset frame clock
    Clock_reset( &context->mClock );
    log_info( "GLFW Window successfully initialized!\n" );
    return true;
        
error:
    if( context ) Context_destroy( context );
    return false;
}


void Context_destroy() {
    glfwTerminate();

    if( context )
        DEL_PTR( context->mVideoModes );
    DEL_PTR( context );
}


void Context_update() {
    static f32 fpsTime = 0.f;

    const f32 now = Clock_getElapsedTime( &context->mClock );
    const f32 frameTime = now - context->mCurrTime;
    context->mCurrTime = now;

    fpsTime += frameTime;

    if( fpsTime > 1.f ) {
        fpsTime = 0.f;
        log_info( "FPS = %f\n", ( 1.f / frameTime ) );
    }
}

void Context_swap() {
    glfwSwapBuffers();
}

void Context_setVSync( bool pVal ) {
    glfwSwapInterval( pVal );
}

void Context_showCursor( bool pVal ) {
    if( pVal )
        glfwEnable( GLFW_MOUSE_CURSOR );
    else
        glfwDisable( GLFW_MOUSE_CURSOR );
}
