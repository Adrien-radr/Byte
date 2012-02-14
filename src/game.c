#include "game.h"

/// Game instance declaration
Game *game = NULL;

bool Game_init() {
    // Init Game Log
    InitLog();

    check( !game, "Tried to initialize already initialized Game Instance. Aborting.\n" );

    // Init Game Memory Manager if in Debug Mode
#   ifdef _DEBUG
    MemoryManager_init();
#   endif


    // Allocate game instance
    game = byte_alloc( sizeof( Game ) );
    check_mem( game );

    // Init Game Device (Window, Context, Config file)
    check( Device_init(), "Error while creating Game Device. Aborting initialization.\n" );

    // Init Game World and load all resources
    check( World_init(), "Error while creating Game World. Aborting initialization.\n" );
    check( World_loadAllResources(), "Error while loading Game Resources. Aborting initialization.\n");


    // Initialize Game current Scene
    game->mScene = Scene_new();
    check( game->mScene, "Error while creating Game Scene. Aborting initialization.\n" );


    printf( "\n" );
    log_info( "Game successfully initialized!\n" );
    printf( "\n\n" );

    return true;

error:
    Game_destroy();
    return false;
}

void Game_destroy() {
    if( game ) {
        Scene_destroy( game->mScene );

        World_destroy();
        Device_destroy();

        DEL_PTR( game );
    }

    CloseLog();

#   ifdef _DEBUG
    MemoryManager_destroy();
#   endif
}
