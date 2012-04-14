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

/*
mat3 m;
Color c;
u32 mesh;
u32 texture;
u32 shader;
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

/*
    mesh = ResourceManager_get( "map.json" );

    shader = ResourceManager_get( "map_shader.json" );
    texture = ResourceManager_get( "map.png" );

    c.r = c.g = c.b = 0.8f;
    c.a = 1.f;
    */
}
 
bool frame_callback( f32 frame_time ) {
    /*
    Renderer_useShader( shader );
    Shader_sendColor( "iColor", &c );
    Shader_sendInt( "Depth", 9 );

    Renderer_useTexture( texture, 0 );

    mat3_identity( &m );
    Shader_sendMat3( "ModelMatrix", &m );
    Renderer_renderMesh( mesh );
    mat3_translatef( &m, 50.f, 25.f );
    Shader_sendMat3( "ModelMatrix", &m );
    Renderer_renderMesh( mesh );
    */

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

