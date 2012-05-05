//#include "game.h"

//#include "renderer.h"
#include "server.h"
//#include "client.h"
#include "common/clock.h"

#ifdef USE_GLDL
#include "GL/gldl.h"
#else
#include "GL/glew.h"
#endif

int main() {
    if( !Server_init() )
        return 1;

    Server_run();

    Server_shutdown();

    return 0;
}

