//#include "GL/glew.h"
#ifdef USE_GLDL
#include "GL/gldl.h"
#else
#include "GL/glew.h"
#endif

#include "GL/glfw.h"

#include "context.h"
#include "event.h"
#include "vector.h"

/// GLFW Window/Context 
typedef struct {
    vec2    *mVideoModes;       ///< Video Modes width/height
    int     mVideoModesNb;      ///< Video Modes count

    vec2    mSize;              ///< Context used VideoMode
    u32     mMultiSamples;      ///< FSAA samples


    //bool    mSizeChanged;       ///< True if size has been changed
    //mat3    mProjectionMatrix;  ///< Projection matrix used 
} Context;

/// Context only instance definition
Context *context = NULL;



/// Default backup Width and Height
static vec2 defaultWinSize = { .x = 800, .y = 600 };



bool Context_init( u32 pWidth, u32 pHeight, bool pFullscreen, const char *pName, u32 pMultiSamples ) {
    check( !context, "Context already created!\n" );
    check( pWidth >= 100 && pHeight >= 100, "Width and Height of window must be larger than %dpx\n", 100 );


    context = (Context*)byte_alloc( sizeof( Context ) );
    check_mem( context );

    // Initialize GLFW 
    check( glfwInit(), "Could not initialize GLFW\n" );

    // Get available videomodes
    GLFWvidmode vidmodes[64];

    context->mVideoModesNb = glfwGetVideoModes( vidmodes, 64 );
    context->mVideoModes = byte_alloc( sizeof(vec2) * context->mVideoModesNb );
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
    //context->mSizeChanged = true;


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

    glfwEnable( GLFW_KEY_REPEAT );




    log_info( "GLFW Window successfully initialized!\n" );
    return true;
        
error:
    if( context ) Context_destroy( context );
    return false;
}


void Context_destroy() {
    glfwTerminate();

    if( context ) {
        DEL_PTR( context->mVideoModes );
        DEL_PTR( context );
    }
}

bool Context_isInitialized() {
    return NULL != context;
}

void Context_swap() {
    glfwSwapBuffers();
}

bool Context_isWindowOpen() {
    return ( glfwGetWindowParam( GLFW_OPENED ) > 0 );
}

vec2 Context_getSize() {
    vec2 size = { .x = -1, .y = -1 };

    if( context ) {
        size.x = context->mSize.x;
        size.y = context->mSize.y;
    }
        
    return size;
}

void Context_setSize( vec2 pSize ) {
    if( context ) {
        context->mSize.x = pSize.x;
        context->mSize.y = pSize.y;
    }
}

void Context_setVSync( bool pVal ) {
    glfwSwapInterval( pVal ? 60 : 0 );
}

void Context_showCursor( bool pVal ) {
    if( pVal )
        glfwEnable( GLFW_MOUSE_CURSOR );
    else
        glfwDisable( GLFW_MOUSE_CURSOR );
}
