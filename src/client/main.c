#include "common/clock.h"
#include "common/vector.h"
#include "client.h"
#include "game.h"
#include "resource.h"
#include "renderer.h"

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
Path *p = NULL;
int path_index = 1;
f32 up_time = 0.f;

int master, button1;

void mousecb( const Event *e, void *data ) {
    if( e->type == EMouseReleased ) {
        if( e->button == MB_Right ) {
            vec2i dest = Scene_screenToIso( game->scene, &e->v );

            // create path
            f32 begin = Client_getElapsedTime();

            if( p ) {
                Map_destroyPath( p );
                path_index = 1;
                up_time = 0.f;
            }

            p = Map_createPath( &world->local_map, &a1->position, &dest );

            printf( "Path creation time : %f\n", Client_getElapsedTime() - begin );

        } else if( e->button == MB_Left ) {
            bool walkable = Map_isWalkable( &world->local_map, &game->mouse_tile );
            SceneMap_redTile( game->scene, &game->mouse_tile, walkable );
            Map_setWalkable( &world->local_map, &game->mouse_tile, !walkable );
        }
    }
}


void init_callback() {
    a1_h = World_loadActor( "data/game/actors/man.json" );
    if( a1_h >= 0 ) {
        a1 = World_getActor( a1_h );

        Game_loadActorAssets( a1 );
        Scene_addSpriteFromActor( game->scene, a1 );
        Game_setActorPosition( a1, &(vec2i){ 11, 4 } );
    }

    // GUI
    Widget button;
    button.position = (vec2i){ 3, 3 };
    button.size = (vec2i){ 120, 50 };
    button.depth = -5;
    Widget_init( &button, "quadmesh.json", "widgettexture.png" );

    button1 = Scene_addWidget( game->scene, &button );

    EventManager_addListener( LT_MouseListener, mousecb, NULL );
}
 
bool frame_callback( f32 frame_time ) {
    if( p ) {
        // path step
        if( up_time >= 0.15f ) {
            vec2i *next = Map_getPathNode( p, path_index++ );
            Game_setActorPosition( a1, next );
            up_time = 0.f;
        } else
            up_time += frame_time;


        if( path_index == Map_getPathCount( p ) ) {
            Map_destroyPath( p );
            p = NULL;
            up_time = 0.f;
            path_index = 1;
        }
    }

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

