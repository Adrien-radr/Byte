#include "device.h"
#include "event.h"
#include "clock.h"
#include "context.h"
#include "renderer.h"

#include "GL/glew.h"
#include "json/cJSON.h"

const str32 ConfigFile = "config.json";

/// Config structure. Loaded from ConfigFile
/// Stores the different parameters of the engine/game
typedef struct {
    vec2    mWindowSize;
    u32     mMultiSamples;
} Config;


typedef struct {
    Clock       mClock;             ///< Clock accumulating time since context creation
    f32         mCurrTime;          ///< Current time since context creation
    f32         mFrameTime;         ///< Time elapsed in last frame

    Camera      *mActiveCamera;     ///< Game active camera (one at a time)

    FT_Library  mFreetype;          ///< Instance of the Freetype library.

    Config      mConfig;            ///< Device/Game configuration parameters
} Device;

Device *device = NULL;

// listener to window resize callback
void Device_windowResize( const Event *pEvent, void *pData ) {
    Context_setSize( pEvent->v );

    glViewport( 0, 0, (GLsizei)pEvent->v.x, (GLsizei)pEvent->v.y );

    if( device->mActiveCamera ) {
        Camera_calculateProjectionMatrix( device->mActiveCamera );
        Renderer_updateProjectionMatrix( &device->mActiveCamera->mProjectionMatrix );
    }
}

bool LoadConfigItem( cJSON *root, cJSON **item, const char *param ) {
    *item = cJSON_GetObjectItem( root, param );
    check( *item, "Error while getting parameter %s from config file.\n", param );
    return true;
error:
    return false;
}

// Config loading function
bool LoadConfig() {
    if( !device ) return false;

    cJSON *root = NULL, *item = NULL;
    char *json_file = NULL;

    bool return_val = false;

    ReadFile( &json_file, ConfigFile ); 
    check( json_file, "Error when loading config file!\n" );

    root = cJSON_Parse( json_file );
    check( root, "JSON Parse error [%s] before :\n%s\n", ConfigFile, cJSON_GetErrorPtr() );

    // get window size
    if( !LoadConfigItem( root, &item, "iWindowWidth" ) ) goto error;
    device->mConfig.mWindowSize.x = (f32)item->valueint;

    if( !LoadConfigItem( root, &item, "iWindowHeight" ) ) goto error;
    device->mConfig.mWindowSize.y = (f32)item->valueint;

    // get multisamples
    if( !LoadConfigItem( root, &item, "iMultiSamples" ) ) goto error;
    device->mConfig.mMultiSamples = item->valueint;

    return_val = true;

error:
    DEL_PTR( json_file );
    if( root ) cJSON_Delete( root );
    return return_val;
}

bool Device_init() {
#   ifdef _DEBUG
    MemoryManager_init();
#   endif

    InitLog();

    check( !device, "Device already initialized!\n" );

    device = byte_alloc( sizeof( Device ) );
    check_mem( device );

    // Load config file
    LoadConfig();

    // Initialize Context
    str32 title;
    MSG( title, 32, "Byte-Project v%d.%d.%d", BYTE_MAJOR, BYTE_MINOR, BYTE_PATCH );

    check( Context_init( device->mConfig.mWindowSize.x, 
                         device->mConfig.mWindowSize.y, 
                         false, 
                         title, 
                         device->mConfig.mMultiSamples ), "Error while creating Context!\n" );

    // Initialize Renderer
    check( Renderer_init(), "Error while creating Renderer!\n" );

    // Initialize Event Manager
    check( EventManager_init(), "Error while creating Event Manager!\n" );

    EventManager_addListener( LT_ResizeListener, Device_windowResize, NULL );

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
        //log_info( "FPS = %f\n", ( 1.f / device->mFrameTime ) );
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
