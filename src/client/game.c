#include "game.h"
#include "resource.h"
#include "renderer.h"

/// Game mouse listener
void gameMouseListener( const Event *event, void *data ) {
    if( event->Type == E_MouseMoved ) {
        str32 text;
        snprintf( text, 32, "X : %d, Y : %d", (int)event->v.x, (int)event->v.y );
        Scene_modifyText( game->mScene, game->mousepos_text, TA_String, text );

        vec2 isomouse = Scene_screenToIso( game->mScene, &event->v );

        snprintf( text, 32, "X : %d, Y : %d", (int)isomouse.x, (int)isomouse.y );
        Scene_modifyText( game->mScene, game->mousetile_text, TA_String, text );
    }
}

/// Game instance declaration
Game *game = NULL;

bool Game_init( void (*init_func)(), bool (*frame_func)(f32) ) {
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

    // Load all resources game
    check( ResourceManager_init(), "Error while initializing resource manager. Aborting initialization.\n" );
    check( ResourceManager_loadAllResources(), "Error while loading Game Resources. Aborting initialization.\n");


    // Init Game World 
    check( World_init(), "Error while creating Game World. Aborting initialization.\n" );



    // Initialize Game current Scene
    game->mScene = Scene_new();
    check( game->mScene, "Error while creating Game Scene. Aborting initialization.\n" );

    // register mouse listener
    EventManager_addListener( LT_MouseListener, gameMouseListener, NULL );

    // Load fps text
    Font *f = Font_get( "DejaVuSans.ttf", 12 );
    Color col = { 0.9f, 0.9f, 0.9f, 1.f };
    game->fps_text = Scene_addText( game->mScene, f, col );
    Scene_modifyText( game->mScene, game->fps_text, TA_String, "FPS : 0" );

    // Load mousepos text
    game->mousepos_text = Scene_addText( game->mScene, f, col );
    vec2 pos = {0,15};
    Scene_modifyText( game->mScene, game->mousepos_text, TA_Position, &pos );
    Scene_modifyText( game->mScene, game->mousepos_text, TA_String, "X  : 0, Y : 0" );

    // Load mousetile text
    game->mousetile_text = Scene_addText( game->mScene, f, col );
    pos.y = 30.f;
    Scene_modifyText( game->mScene, game->mousetile_text, TA_Position, &pos );
    Scene_modifyText( game->mScene, game->mousetile_text, TA_String, "X  : 0, Y : 0" );

    printf( "\n" );
    log_info( "Game successfully initialized!\n" );
    printf( "\n\n" );

    // call init func
    if( init_func )
        init_func();

    game->frame_func = frame_func;

    return true;

error:
    return false;
}

void Game_destroy() {
    if( game ) {
        Scene_destroy( game->mScene );

        World_destroy();
        ResourceManager_destroy();
        Device_destroy();

        DEL_PTR( game );
    }

#   ifdef _DEBUG
    MemoryManager_destroy();
#   endif
}

bool Game_update( f32 frame_time ) {
    static const f32 phy_dt = 0.01f;
    static f32 phy_update = 0.f, one_sec = 0.f;

    // check for game termination
    if( IsKeyUp( K_Escape ) || !Context_isWindowOpen() )
        return false;

    // game frame
    Device_beginFrame();
        phy_update += frame_time;
        one_sec += frame_time;

        // callback. return false immediatly if frame_func said it
        if( game->frame_func && !game->frame_func( frame_time ) )
            return false;

        // AI GAMEPLAY LOOP (fixed at 1/phy_dt FPS)
        while( phy_update >= phy_dt ) {
            Scene_update( game->mScene );
            phy_update -= phy_dt;
        }

        // EACH 1 SECOND STUFF
        if( one_sec >= 1.f ) {
            str32 fps_str;
            snprintf( fps_str, 32, "FPS : %4.0f", (1.f/frame_time) );
            Scene_modifyText( game->mScene, game->fps_text, TA_String, fps_str );
            one_sec = 0.f;
        }

        // render scene
        Scene_render( game->mScene );
    Device_endFrame();

    return true;
}

bool Game_loadActorAssets( Actor *actor ) {
    check( game, "Tried to load actor assets on an ininitialized game instance\n" );

    // load scaled mesh
    str256 scaled_mesh_str;
    str16 mesh_size;
    snprintf( mesh_size, 16, "%d.%d", (int)actor->mSize.x, (int)actor->mSize.y );

    strcpy( scaled_mesh_str, actor->assets.mesh );
    strcat( scaled_mesh_str, mesh_size );

    actor->mMesh_id = ResourceManager_get( scaled_mesh_str );
    // if correctly sized mesh is not yet loaded, create it
    if( -1 == actor->mMesh_id ) {
        actor->mMesh_id = ResourceManager_get( actor->assets.mesh );
        check( actor->mMesh_id >= 0, "Error while loading actor '%s' mesh. Mesh '%s' is not a loaded resource.\n", actor->mFirstname, actor->assets.mesh );

        // resize
        int scaled_mesh = Renderer_createRescaledMesh( actor->mMesh_id, &actor->mSize );
        check( scaled_mesh >= 0, "Error while creating scaled mesh for actor '%s'. \n", actor->mFirstname );
        
        actor->mMesh_id = scaled_mesh;

        // add newly rescaled mesh to resource manager
        ResourceManager_add( scaled_mesh_str, scaled_mesh );
    }   

    // load texture
    actor->mTexture_id = ResourceManager_get( actor->assets.texture );
    check( actor->mTexture_id >= 0, "Error while loading actor '%s' texture. Texture '%s' is not a loaded resource.\n", actor->mFirstname, actor->assets.mesh );

    return true;

error:
    return false;
}
