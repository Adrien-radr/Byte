#include "common/clock.h"
#include "common/vector.h"
#include "client.h"
#include "game.h"
#include "resource.h"
#include "renderer.h"
#include "context.h"

#ifdef USE_GLDL
#include "GL/gldl.h"
#else
#include "GL/glew.h"
#endif

/*                  TODO
    - Try implementing a ingame chat to test arch

*/


Actor *a1;
int a1_h;


void mousecb( const Event *e, void *data ) {
    if( !Widget_callback( root->widget, e ) ) {
        if( e->type == EMouseReleased ) {
            if( e->button == MB_Right ) {
                vec2 newpos = Scene_localToGlobal( game->scene, &e->v );

                mat3 m;
                mat3_translationMatrixfv( &m, &newpos );
                m.x[2] = a1->size.y / 2.f;
                Scene_modifySprite( game->scene, a1_h, SA_Matrix, &m );
            } else if( e->button == MB_Left ) {
                int i = (int)game->mouse_tile.x;
                int j = (int)game->mouse_tile.y;
                if( !Map_isWalkable( &world->local_map, i, j ) ) {
                    SceneMap_redTile( game->scene, i, j );
                    Map_setWalkable( &world->local_map, i, j, false );
                }
            }
        }
    }
}


void init_callback() {
    a1_h = World_loadActor( "data/game/actors/man.json" );
    if( a1_h >= 0 ) {
        a1 = World_getActor( a1_h );

        Game_loadActorAssets( a1 );
        Scene_addSpriteFromActor( game->scene, a1 );
        vec2 pos = { 200, 100 };
        mat3 m;
        mat3_translationMatrixfv( &m, &pos );
        m.x[2] = a1->size.y / 2.f;
        Scene_modifySprite( game->scene, a1_h, SA_Matrix, &m );
    }

    Font* f = Font_get( "DejaVuSans.ttf", 12 );
    Color c;
    Color_set( &c, 1.f, 1.f, 1.f, 1.f );



    // GUI
    root = RootWidget_init();

    window = Widget_init( WT_Window, &(vec2i){ 300, 250 }, "quadmesh.json", "widgettexture.png", -1 );
    window->position = vec2i_c( 100, 100 );
    window->size = vec2i_c( 300, 250 );
    window->depth = -5;

    openWindow = Widget_init( WT_Button, &(vec2i){ 15, 15 }, "quadmesh.json", "winicon.png", -1 );
    vec2i size = Context_getSize();
    openWindow->position = vec2i_c( size.x - 15, 0 );
    openWindow->callback = &openWindowCallback;

    int winName = Scene_addText( game->scene, f, c );
    Scene_modifyText( game->scene, winName, TA_String, "Window" );

    windowHead = Widget_createWindowHead( window, winName );

    int buttonText = Scene_addText( game->scene, f, c );
    Scene_modifyText( game->scene, buttonText, TA_String, "Button" );

    button = Widget_init( WT_Button, &(vec2i){ 192, 38 }, "quadmesh.json", "widgettexture.png", buttonText );
    button->position = vec2i_c( 400, 400 );
    button->textOffset = vec2i_c( 72, 11 );
    button->depth = -6;
    button->callback = &button1Callback;


    Widget_addChild( root->widget, windowHead, false );
    Widget_addChild( root->widget, openWindow, false );
    Widget_addChild( window, button, true );

    Scene_addWidget( game->scene, windowHead );

    EventManager_addListener( LT_MouseListener, mousecb, NULL );
}

bool frame_callback( f32 frame_time ) {
    return true;
}

int main() {
    Clock_sleep( 0.5f );

    net_addr sv_addr = { { 127,0,0,1 }, 1991 };
    //net_addr sv_addr = { { 192,168,1,3 }, 1991 };

    if( !Client_init( &sv_addr ) )
        return 1;

    if( !Game_init( init_callback, frame_callback ) ) {
        Client_shutdown();
        return 1;
    }


    Client_run();

    Game_destroy();

    Client_shutdown();

    return 0;
}

