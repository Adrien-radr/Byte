#include "device.h"
#include "event.h"
#include "clock.h"
#include "context.h"
#include "renderer.h"

#include "GL/glew.h"

typedef struct s_Device {
    Clock   mClock;             ///< Clock accumulating time since context creation
    f32     mCurrTime;          ///< Current time since context creation
    f32     mFrameTime;         ///< Time elapsed in last frame

    Camera  *mActiveCamera;     ///< Game active camera (one at a time)

    FT_Library  mFreetype;
} Device;

Device *device = NULL;

// Definition of GLFW Window resize function declared in context.h
void WindowResizeCallback( int pWidth, int pHeight ) {
    vec2 newSize = { .x = pWidth, .y = pHeight };
    Context_setSize( newSize );

    glViewport( 0, 0, (GLsizei)pWidth, (GLsizei)pHeight );

    if( device->mActiveCamera ) {
        Camera_calculateProjectionMatrix( device->mActiveCamera );
        Renderer_updateProjectionMatrix( &device->mActiveCamera->mProjectionMatrix );
    }
}

bool Device_init() {
#   ifdef _DEBUG
    MemoryManager_init();
#   endif

    InitLog();

    check( !device, "Device already initialized!\n" );

    device = byte_alloc( sizeof( Device ) );
    check_mem( device );

    // Initialize Context
    str32 title;
    MSG( title, 32, "Byte-Project v%d.%d.%d", BYTE_MAJOR, BYTE_MINOR, BYTE_PATCH );

    check( Context_init( 800, 600, false, title, 0 ), "Error while creating Context!\n" );

    // Initialize Renderer
    check( Renderer_init(), "Error while creating Renderer!\n" );

    // Initialize Event Manager
    check( EventManager_init(), "Error while creating Event Manager!\n" );

    // Initialize Freetype
    check( !FT_Init_FreeType( &device->mFreetype ), "Could not initialize Freetype library!\n" );

    // reset frame clock
    device->mCurrTime = 0.f;
    Clock_reset( &device->mClock );


    printf( "\n" );
    log_info( "Device successfully initialized!\n" );
    printf( "\n\n" );

    return true;
error:
    return false;
}

void Device_destroy() {
    if( device ) {
        Renderer_destroy();
        Context_destroy();
        EventManager_destroy();

        DEL_PTR( device );
    }

    CloseLog();
#   ifdef _DEBUG
    MemoryManager_destroy();
#   endif
}

void Device_beginFrame() {
    // Calculate frametime from device clock
    static f32 fpsAccum = 0.f;

    const f32 now = Clock_getElapsedTime( &device->mClock );
    device->mFrameTime = now - device->mCurrTime;
    device->mCurrTime = now;

    fpsAccum += device->mFrameTime;

    if( fpsAccum > 1.f ) {
        fpsAccum = 0.f;
        log_info( "FPS = %f\n", ( 1.f / device->mFrameTime ) );
    }

    // Update events and inputs
    EventManager_update();




    // Begin render
    Renderer_beginFrame();
}


void Device_endFrame() {
    // swap buffers
    Context_swap();
}

f32 Device_getFrameTime() {
    return device->mFrameTime;
}

void Device_setCamera( Camera *pCamera ) {
    if( device && pCamera ) {
        device->mActiveCamera = pCamera;
        Renderer_updateProjectionMatrix( &pCamera->mProjectionMatrix );
    }
}

FT_Library *Device_getFreetype() {
    if( device ) 
        return &device->mFreetype;
    return NULL;
}
