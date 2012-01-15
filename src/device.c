#include "device.h"
#include "context.h"
#include "renderer.h"
#include "event.h"

typedef struct s_Device {
    int dummy;
} Device;

Device *device = NULL;

void ResizeCallback() {
    const mat3 *pm = Context_getProjectionMatrix();
    if( pm ) 
        Renderer_updateProjectionMatrix( pm );
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
    Context_setResizeCallback( ResizeCallback );

    // Initialize Renderer
    check( Renderer_init(), "Error while creating Renderer!\n" );

    // Initialize Event Manager
    check( EventManager_init(), "Error while creating Event Manager!\n" );


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
    EventManager_update();
    Context_update();

    Renderer_beginFrame();
}


void Device_endFrame() {
    Context_swap();
}
