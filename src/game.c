#include "game.h"
#include "resource.h"
#include "render/context.h"
#include "render/renderer.h"

#include "json/cJSON.h"

// #############################################################
//      GAME EVENT HANDLING

void Game_mouseListener( const Event *event, void *data ) {
    if( event->type == EMouseMoved ) {
        str32 text;
        snprintf( text, 32, "X : %d, Y : %d", event->v.x, event->v.y );
        Scene_modifyText( game->scene, game->mousepos_text, TA_String, text );

        game->mouse_tile = Scene_screenToIso( game->scene, &event->v );
        snprintf( text, 32, "TileX: %d, TileY : %d", game->mouse_tile.x, game->mouse_tile.y );
        Scene_modifyText( game->scene, game->mousetile_text, TA_String, text );
    }


    // if in game mode
    if( game->mode == EGame ) {
        // TODO : send event to GUI, and if not intercepted, 
        // send event to World
        World_receiveEvent( game->world, event );
        Scene_receiveEvent( game->scene, event ); 
    }
}

void Game_keyListener( const Event *event, void *data ) {
    if( event->type == EKeyPressed && event->key == K_F1 ) {
        game->mode = game->mode == EEditor ? EGame : EEditor;
    }

    // if in game mode
    if( game->mode == EGame ) {
        // TODO : send event to GUI, and if not intercepted, 
        // send event to World
        World_receiveEvent( game->world, event );
        Scene_receiveEvent( game->scene, event ); 
    }
}

/*
void Game_windowResize( const Event *event, void *data ) {
    Context_setSize( event->v );

    glViewport( 0, 0, (GLsizei)event->v.x, (GLsizei)event->v.y );

    if( game->scene ) {
        Camera_calculateProjectionMatrix( game->scene->camera );
        Renderer_updateProjectionMatrix( ECamera, &game->scene->camera->mProjectionMatrix );
    }
}
*/


// #############################################################
//      CONFIG
const str32 config_file = "config.json";

bool LoadConfigItem( cJSON *root, cJSON **item, const char *param ) {
    *item = cJSON_GetObjectItem( root, param );
    check( *item,
           "Error while getting parameter %s from config file.\n", param );
    return true;
error:
    return false;
}

// Config loading function
bool LoadConfig() {
    if( !game ) return false;

    cJSON *root = NULL, *item = NULL;
    char *json_file = NULL;

    bool return_val = false;

    Byte_ReadFile( &json_file, config_file );
    check( json_file, "Error when loading config file!\n" );

    root = cJSON_Parse( json_file );
    check( root, "JSON Parse error [%s] before :\n%s\n", config_file, cJSON_GetErrorPtr() );

    // get fullscreen state
    if( !LoadConfigItem( root, &item, "bFullscreen" ) ) 
        game->config.fullscreen = 0;
    else 
        game->config.fullscreen = item->type > 0;

    // get window size
    if( !LoadConfigItem( root, &item, "iWindowWidth" ) ) 
        game->config.window_size.x = 800;
    else 
        game->config.window_size.x = (f32)item->valueint;

    if( !LoadConfigItem( root, &item, "iWindowHeight" ) ) 
        game->config.window_size.y = 600;
    else
        game->config.window_size.y = (f32)item->valueint;

    // get multisamples
    if( !LoadConfigItem( root, &item, "iMultiSamples" ) ) 
        game->config.multisamples = 0;
    else 
        game->config.multisamples = item->valueint;

    return_val = true;

error:
    DEL_PTR( json_file );
    if( root ) cJSON_Delete( root );
    return return_val;
}


// #############################################################
//      GAME


/// Game instance declaration
Game *game = NULL;

bool Game_init( void (*init_func)(), bool (*frame_func)(f32) ) {
    if( game ) {
        printf( "Fatal Error : Tried to initialize already initialized Game instance. Aborting.\n" );
        return false;
    }

    // Init Game Memory Manager if in Debug Mode
#   ifdef _DEBUG
    MemoryManager_init();
#   endif

    // Initialize random seed
    srand( time(NULL) );

    // Allocate game instance
    game = byte_alloc( sizeof( Game ) );
    check_mem( game );


    // Initialize game clock
    Clock_reset( &game->clock );
    
    // Initialize game log
    InitLog( "Byte.log", &game->clock );


    // Load config file
    LoadConfig();

    // Begining game mode : Game
    game->mode = EGame;

    // Initialize Context
    str32 title;
    MSG( title, 32, "Byte-Project v%d.%d.%d", BYTE_MAJOR, BYTE_MINOR, BYTE_PATCH );

    check( Context_init( game->config.window_size.x,
                         game->config.window_size.y,
                         game->config.fullscreen,
                         title,
                         game->config.multisamples ), "Error while creating Context!\n" );

    // Initialize Renderer
    check( Renderer_init(), "Error while creating Renderer!\n" );

    // Initialize Event Manager
    check( EventManager_init(), "Error while creating Event Manager!\n" );

    // Initialize Freetype
    check( !FT_Init_FreeType( &game->freetype_lib ), "Could not initialize Freetype library!\n" );



    // Load all resources game
    check( ResourceManager_init(), "Error while initializing resource manager. Aborting initialization.\n" );
    check( ResourceManager_loadAllResources(), "Error while loading Game Resources. Aborting initialization.\n");


    // Init Game World 
    check( World_init( &game->world ), "Error while creating Game World. Aborting initialization.\n" );


    // Initialize Game current Scene
    check( Scene_init( &game->scene ), "Error while creating Game Scene. Aborting initialization.\n" );


    /// Load all animations
    AnimManager_loadAll( &game->anims );


    // Register Game listeners
    //EventManager_addListener( LT_ResizeListener, Game_windowResize, NULL );
    EventManager_addListener( LT_MouseListener, Game_mouseListener, NULL );
    EventManager_addListener( LT_KeyListener, Game_keyListener, NULL );



    printf( "\n" );
    log_info( "Game successfully initialized!\n" );
    printf( "\n\n" );

    // call init func
    if( init_func )
        init_func();


    // Initialize scene tiles to upper left world
    Scene_setLocation( game->scene, wmap_initX, wmap_initY );


    Font *f = Font_get( "DejaVuSans.ttf", 12 );
    Color col = { 0.9f, 0.9f, 0.9f, 1.f };
    int text_depth = -10;
    vec2i pos = { 6, 4 };

    // Load fps text
    game->fps_text = Scene_addText( game->scene, f, col );
    Scene_modifyText( game->scene, game->fps_text, TA_Position, &pos );
    Scene_modifyText( game->scene, game->fps_text, TA_String, "FPS : 0" );
    Scene_modifyText( game->scene, game->fps_text, TA_Depth, &text_depth );

    // Load mousepos text
    game->mousepos_text = Scene_addText( game->scene, f, col );
    pos.y += 15;
    Scene_modifyText( game->scene, game->mousepos_text, TA_Position, &pos );
    Scene_modifyText( game->scene, game->mousepos_text, TA_String, "X  : 0, Y : 0" );
    Scene_modifyText( game->scene, game->mousepos_text, TA_Depth, &text_depth );

    // Load mousetile text
    game->mousetile_text = Scene_addText( game->scene, f, col );
    pos.y += 15;
    Scene_modifyText( game->scene, game->mousetile_text, TA_Position, &pos );
    Scene_modifyText( game->scene, game->mousetile_text, TA_String, "X  : 0, Y : 0" );
    Scene_modifyText( game->scene, game->mousetile_text, TA_Depth, &text_depth );


    // Update all created shaders about their projection matrices
    Scene_updateShadersProjMatrix( game->scene );


    // register main.c frame function
    game->frame_func = frame_func;


    return true;

error:
    return false;
}

void Game_destroy() {
    if( game ) {
        Scene_destroy( game->scene );
        World_destroy( game->world );

        AnimManager_unloadAll( &game->anims );
        ResourceManager_destroy();

        Renderer_destroy();
        Context_destroy();
        EventManager_destroy();

        DEL_PTR( game );
    }

#   ifdef _DEBUG
    MemoryManager_destroy();
#   endif
}

void Game_run() {
    // clock & time record variables
    f32 start_t = 0.f, frame_t = 0.f;

    // gameplay/IA update variables
    const f32 phy_dt = 0.01f;
    f32 phy_update = 0.f, one_sec = 0.f;


    // MAIN LOOP
    while( true ) {
        // sample frame start time
        start_t = Clock_getElapsedTime( &game->clock );

        // check for game termination
        if( IsKeyUp( K_Escape ) || !Context_isWindowOpen() )
            break;

        
        // update events and inputs
        EventManager_update();

        // GAMEPLAY
            phy_update += frame_t;
            one_sec += frame_t;

            // custom frame callback. return false immediatly if frame_func said it
            if( game->frame_func && !game->frame_func( frame_t ) )
                break;

            // AI GAMEPLAY LOOP (fixed at 1/phy_dt FPS)
            while( phy_update >= phy_dt ) {
                Scene_update( game->scene, phy_dt, game->mode );
                /* TODO : WORLD UPDATE ! */
                phy_update -= phy_dt;
            }

            // EACH 1 SECOND STUFF, (temp stuff)
            if( one_sec >= 1.f ) {
                str32 fps_str;
                snprintf( fps_str, 32, "FPS : %4.0f", (1.f/frame_t) );
                Scene_modifyText( game->scene, game->fps_text, TA_String, fps_str );
                one_sec = 0.f;
            }

        // RENDERING
            Renderer_beginFrame();
            Scene_render( game->scene );
            Context_swap();
    

        // calculate frame time
        frame_t = Clock_getElapsedTime( &game->clock ) - start_t;
    }

    // End game and free RAM/VRAM resources
    Game_destroy();
}

FT_Library *Game_getFreetype() {
    if( game )
        return &game->freetype_lib;
    return NULL;
}


inline f32 Game_getElapsedTime() {
    return Clock_getElapsedTime( &game->clock );
}
