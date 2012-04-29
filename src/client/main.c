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

void mousecb( const Event *e, void *data ) {
    if( e->Type == EMouseReleased ) {
        vec2 newpos = vec2_add( &game->scene->camera->global_position, &e->v );
        mat3 m;
        mat3_translationMatrixfv( &m, &newpos );
        m.x[2] = a1->size.y / 2.f;
        Scene_modifySprite( game->scene, a1_h, SA_Matrix, &m );
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

