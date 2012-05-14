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

int root, window1, window1head, window1name, button1, text1;

void mousecb( const Event *e, void *data ) {/*
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
    }*/
    if( e->type == EMouseReleased ) {
        Widget_callback( RootWidget, e );
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

    text1 = Scene_addText( game->scene, f, c );
    Scene_modifyText( game->scene, text1, TA_String, "Button" );

    window1name = Scene_addText( game->scene, f, c );
    Scene_modifyText( game->scene, window1name, TA_String, "Window" );

    // GUI
    vec2i contextsize = Context_getSize();
    RootWidget = Widget_init( WT_Root, &contextsize, NULL, NULL, -1 );
    RootWidget->position = vec2i_c( 0, 0 );


    window = Widget_init( WT_Window, &(vec2i){ 300, 250 }, "quadmesh.json", "widgettexture.png", -1 );
    window->position = vec2i_c( 100, 100 );
    window->size = vec2i_c( 300, 250 );
    window->depth = -5;

    windowHead = Widget_createWindowHead( window, window1name );



    button = Widget_init( WT_Button, &(vec2i){ 192, 38 }, "quadmesh.json", "widgettexture.png", text1 );
    button->position = vec2i_c( 400, 400 );
    button->size = vec2i_c( 192, 38 );
    button->textOffset = vec2i_c( 72, 11 );
    button->depth = -5;
    button->callback = &button1Callback;


    Widget_addChild( RootWidget, windowHead );
    Widget_addChild( RootWidget, button );

    window1head = Scene_addWidget( game->scene, windowHead );
    button1 = Scene_addWidget( game->scene, button );

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

