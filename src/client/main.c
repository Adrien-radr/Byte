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

void init_callback() {
    int a1_h = World_loadActor( "data/game/actors/man.json" );
    if( a1_h >= 0 ) {
        Actor *a1 = World_getActor( a1_h );

        Game_loadActorAssets( a1 );
        Scene_addSpriteFromActor( game->mScene, a1 );
        vec2 pos = { 50, -25 };
        mat3 m;
        mat3_translationMatrixfv( &m, &pos );
        Scene_modifySprite( game->mScene, a1_h, SA_Matrix, &m );
    }
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

