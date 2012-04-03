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
    - Make this type of loop, with before the while the code needed
      to connect the client to the server and init game data.
    - Make multi-client arch.
    - Introduce guaranteed(connection, ...) vs unguaranteed(fast stuff)
      pakets.
    - Implement connection/disconnection
    - Add Heartbeats when no inputs
    - Try implementing a ingame chat to test arch

//              TYPICAL MULTI LOOP

while ( true )
{
    SendPackets( socket );

    while ( true )
    {
        int packetSize = 0;
        unsigned char packet[1024];
        if ( !socket.RecievePacket( packet, packetSize ) )
            break;
        assert( packetSize > 0 );
        Game::ProcessPacket( packet, packetSize );
    }

    float frameTime = Timer::getFrameTime();
    Game::Update( frameTime );
}

*/
GLuint vbo, shader;
mat3 m;
Color c;

void init_callback() {
    Actor man;
    Actor_load( &man, "data/game/actors/man.json" );

    Game_loadActorAssets( &man );
    World_addActor( &man );
    Scene_addSpriteFromActor( game->mScene, &man );


    // tile
    vec2 pts[] = {
        { 50,25 },
        { 0,50 },
        { 50,75 },
        { 100,50 },
        { 50,25 }
    };

    glGenBuffers( 1, &vbo );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData( GL_ARRAY_BUFFER, sizeof(pts), pts, GL_STATIC_DRAW );

    shader = ResourceManager_get( "map_shader.json" );

    c.r = c.g = c.b = 0.8f;
    c.a = 1.f;
}
 
bool frame_callback( f32 frame_time ) {
    Renderer_useShader( shader );
    Shader_sendColor( "iColor", &c );
    Shader_sendInt( "Depth", 9 );

    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glDisableVertexAttribArray( 1 );
    glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, 0 );
    mat3_identity( &m );
    Shader_sendMat3( "ModelMatrix", &m );
    glDrawArrays( GL_LINE_STRIP, 0, 5 );
    mat3_translatef( &m, 50.f, 25.f );
    Shader_sendMat3( "ModelMatrix", &m );
    glDrawArrays( GL_LINE_STRIP, 0, 5 );
    glEnableVertexAttribArray( 1 );

    return true;
}

int main() {
    Clock_sleep( 0.5f );

    net_addr sv_addr = { { 127,0,0,1 }, 1991 };

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

