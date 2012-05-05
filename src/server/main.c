#include "server.h"
#include "common/clock.h"

int main() {
    if( !Server_init() )
        return 1;

    Server_run();

    Server_shutdown();

#ifdef BYTE_WIN32
    system("PAUSE");
#endif

    return 0;
}

