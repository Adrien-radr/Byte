#include "context.h"
#include "event.h"
#include "GL/glfw.h"

/// GLFW Window/Context
typedef struct {
    vec2i   *video_modes;       ///< Video Modes width/height
    int     video_modes_n;      ///< Video Modes count

    vec2i   size;               ///< Context used VideoMode
    u32     multi_samples;      ///< FSAA samples
} Context;

/// Context only instance definition
Context *context = NULL;



/// Default backup Width and Height
static vec2i defaultWinSize = { .x = 800, .y = 600 };



bool Context_init( u32 width, u32 height, bool fullscreen, const char *name, u32 multi_samples ) {
    check( !context, "Context already created!\n" );
    check( width >= 100 && height >= 100, "Width and Height of window must be larger than %dpx\n", 100 );


    context = (Context*)byte_alloc( sizeof( Context ) );
    check_mem( context );

    // Initialize GLFW
    check( glfwInit(), "Could not initialize GLFW\n" );

    // Get available videomodes
    GLFWvidmode vidmodes[64];

    context->video_modes_n = glfwGetVideoModes( vidmodes, 64 );
    context->video_modes = byte_alloc( sizeof(vec2i) * context->video_modes_n );
    bool resolution_found = false;

    for( int i = 0; i < context->video_modes_n; ++i ) {
        context->video_modes[i].x = vidmodes[i].Width;
        context->video_modes[i].y = vidmodes[i].Height;

        if( !resolution_found &&
                context->video_modes[i].x == width && context->video_modes[i].y == height )
            resolution_found = true;
    }

    if( resolution_found || !fullscreen )
        glfwSetWindowSize( width, height );
    else {
        log_warn( "VideoMode %dx%d not available, back to %dx%d\n", width, height, defaultWinSize.x, defaultWinSize.y );
        width = defaultWinSize.x;
        height = defaultWinSize.y;
        glfwSetWindowSize( width, height );
    }

    context->size.x = width;
    context->size.y = height;


    // Set window name
    check( name, "Window name invalid\n" );
    glfwSetWindowTitle( name );

    // OpenGL profile
    //glfwOpenWindowHint( GLFW_OPENGL_VERSION_MAJOR, 2 );
    //glfwOpenWindowHint( GLFW_OPENGL_VERSION_MINOR, 1 );
    //glfwOpenWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
    //glfwOpenWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

    // no resizing
    glfwOpenWindowHint( GLFW_WINDOW_NO_RESIZE, GL_TRUE );

    // Multisamples
    u32 ms = multi_samples;
    if( ms != 2 || ms != 4 || ms != 8 )
        ms = 0;

    context->multi_samples = ms;
    glfwOpenWindowHint( GLFW_FSAA_SAMPLES, ms );


    int init = glfwOpenWindow(  context->size.x, context->size.y,
                                8, 8, 8, 0,
                                24, 8, fullscreen ? GLFW_FULLSCREEN : GLFW_WINDOW );

    check( init, "Could not initialize GLFW Window\n" );

    // set glfw callback functions
        glfwSetWindowSizeCallback( WindowResizeCallback );
        glfwSetKeyCallback( KeyPressedCallback );
        glfwSetCharCallback( CharPressedCallback );
        glfwSetMouseButtonCallback( MouseButtonCallback );
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
        DEL_PTR( context->video_modes );
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

vec2i Context_getSize() {
    vec2i size = { .x = -1, .y = -1 };

    if( context ) {
        size.x = context->size.x;
        size.y = context->size.y;
    }

    return size;
}

void Context_setSize( vec2i size ) {
    if( context ) {
        context->size.x = size.x;
        context->size.y = size.y;
    }
}

void Context_setVSync( bool val ) {
    glfwSwapInterval( val ? 60 : 0 );
}

void Context_showCursor( bool val ) {
    if( val )
        glfwEnable( GLFW_MOUSE_CURSOR );
    else
        glfwDisable( GLFW_MOUSE_CURSOR );
}
