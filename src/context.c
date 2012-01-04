#include "GL/glew.h"
#include "GL/glfw.h"

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

        glViewport( 0, 0, (GLsizei)pWidth, (GLsizei)pHeight );
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

    glfwEnable( GLFW_KEY_REPEAT );

    // GLEW initialisation
    GLenum glerr = glewInit();

    check( GLEW_OK == glerr, "GLEW Error : %s\n", glewGetErrorString( glerr ) );

    log_info( "GLEW v%s successfully initialized!\n", glewGetString( GLEW_VERSION ) );

    // GL Version 
    int majV, minV;
    glGetIntegerv( GL_MAJOR_VERSION, &majV );
    glGetIntegerv( GL_MINOR_VERSION, &minV );
    log_info( "OpenGL %d.%d\n", majV, minV );
    log_info( "GLSL %s\n", glGetString( GL_SHADING_LANGUAGE_VERSION ) );
    log_info( "Hardware : %s - %s\n", glGetString( GL_VENDOR ), glGetString( GL_RENDERER ) );

    // GL state initialisations
    glHint( GL_GENERATE_MIPMAP_HINT, GL_NICEST );

    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LESS );

//    glEnable( GL_CULL_FACE );
//    glCullFace( GL_BACK );

    glClearColor( 0.2f, 0.2f, 0.2f, 1.f );


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

bool Context_isWindowOpen() {
    return ( glfwGetWindowParam( GLFW_OPENED ) > 0 );
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


void CheckGLError_func( const char *pFile, u32 pLine ) {
#ifdef _DEBUG
    GLenum error = glGetError();

    if (error != GL_NO_ERROR) {
        const str64 errorStr;
        const str256 description;

        switch (error)
        {
            case GL_INVALID_ENUM :
            {
                strncpy( errorStr, 64, "GL_INVALID_ENUM" );
                strncpy( description, 256, "An unacceptable value has been specified for an enumerated argument." );
                break;
            }

            case GL_INVALID_VALUE :
            {
                strncpy( errorStr, 64, "GL_INVALID_VALUE" );
                strncpy( description, 256, "A numeric argument is out of range." );
                break;
            }

            case GL_INVALID_OPERATION :
            {
                strncpy( errorStr, 64, "GL_INVALID_OPERATION" );
                strncpy( description, 256, "The specified operation is not allowed in the current state." );
                break;
            }

            case GL_STACK_OVERFLOW :
            {
                strncpy( errorStr, 64, "GL_STACK_OVERFLOW" );
                strncpy( description, 256, "This command would cause a stack overflow." );
                break;
            }

            case GL_STACK_UNDERFLOW :
            {
                strncpy( errorStr, 64, "GL_STACK_UNDERFLOW" );
                strncpy( description, 256, "This command would cause a stack underflow." );
                break;
            }

            case GL_OUT_OF_MEMORY :
            {
                strncpy( errorStr, 64, "GL_OUT_OF_MEMORY" );
                strncpy( description, 256, "There is not enough memory left to execute the command." );
                break;
            }

            case GL_INVALID_FRAMEBUFFER_OPERATION :
            {
                strncpy( errorStr, 64, "GL_INVALID_FRAMEBUFFER_OPERATION_EXT" );
                strncpy( description, 256, "The object bound to FRAMEBUFFER_BINDING is not \"framebuffer complete\"." );
                break;
            }
        }

        log_err( "OpenGL Error (%s [%d])\n"
                 "-- Error          : %s\n"
                 "-- Description    : %s\n", pFile, pLine, errorStr, description );
    }
#endif
}
