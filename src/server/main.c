//#include "game.h"

//#include "renderer.h"
#include "server.h"
//#include "client.h"
#include "common/clock.h"

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


int main() {
    if( !Server_init() )
        return 1;

    Server_run();

    Server_shutdown();

    return 0;

    /*
    int return_val = -1;

    pthread_t th;
    pthread_create( &th, NULL, sv_connection, NULL );

    Clock_sleep( 0.5f );
    printf( "Launching client...\n" );

    if( !Client_init() ) {
        pthread_cancel( th );
        goto error;
    }

    Client_run();

    pthread_join( th, NULL );

    return_val = 0;

error:
    Client_shutdown();

    return return_val;
*/


    /*

    printf( "Hello, Byte World!!\n" );

    str64 date;
    str16 time;
    GetTime( date, 64, DateFmt );
    GetTime( time, 16, TimeFmt );

    strncat( date, " - ", 3 );
    strncat( date, time, 16 );
    printf( "%s\n\n", date );


    Actor man;
    check( Actor_load( &man, "data/actors/man.json" ), "Error while loading man!\n" );

    int man_sprite = Scene_addSpriteFromActor( game->mScene, &man );
    check( man_sprite >= 0, "man error\n" );

    


    vec2 pts[] = {
        { 50,25 },
        { 0,50 },
        { 50,75 },
        { 100,50 },
        { 50,25 }
    };

    GLuint vbo;
    glGenBuffers( 1, &vbo );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData( GL_ARRAY_BUFFER, sizeof(pts), pts, GL_STATIC_DRAW );

    u32 shader = World_getResource( "map_shader.json" );

    // ###############################3
    //      TEXT
    Font *f = Font_get( "DejaVuSans.ttf", 12 );
    Color col = { 0.6f, 0.6f, 0.6f, 1.f };
    int text = Scene_addText( game->mScene, f, col );

    check( text >= 0, "error creating text!\n" );

    Scene_modifyText( game->mScene, text, TA_String, "Hello World!!" );



    Color c = { 0.8f, 0.8f, 0.8f, 1.f };
    mat3 m;
    mat3_identity( &m );
    ////////////////////////////////////

    int cpt = 0;
    f32 accum = 0;
    f32 frame_time;

    f32 p_update = 0.f;
    f32 dt = 0.01f;     // IA/Physics updated at 100 FPS


    while( !IsKeyUp( K_Escape ) && Context_isWindowOpen() ) {
        Device_beginFrame();
            // Timer stuff
            frame_time = Device_getFrameTime();
            accum += frame_time;
            p_update += frame_time;


            // AI GAMEPLAY LOOP (fixed at 1/dt FPS)
            while( p_update >= dt ) {
                vec2 ent2_move = { 0, 0 };
                if( IsKeyDown( K_Up ) )
                    ent2_move.y -= 1;
                if( IsKeyDown( K_Down ) )
                    ent2_move.y += 1;
                if( IsKeyDown( K_Left ) )
                    ent2_move.x -= 1;
                if( IsKeyDown( K_Right ) )
                    ent2_move.x += 1;
            //    if( ent2_move.x != 0 || ent2_move.y != 0 ) 
             //       Actor_move( &actors[0], &ent2_move );

                Scene_update( game->mScene );

                p_update -= dt;
            }

            // Stuff hapenning each 1 second
            if( accum > 1.f ) {
                ++cpt;
                accum = 0.f;

                str64 fps_str;
                MSG( fps_str, 64, "FPS : %4.0f", (1.f/frame_time) );
                Scene_modifyText( game->mScene, text, TA_String, fps_str );
            }

            // Render Frame
            Scene_render( game->mScene );

            Renderer_useShader( shader );
            Shader_sendColor( "iColor", &c );

            glBindBuffer( GL_ARRAY_BUFFER, vbo );
            //glEnableVertexAttribArray( 0 );
            glDisableVertexAttribArray( 1 );
            glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, 0 );
            mat3_identity( &m );
            Shader_sendMat3( "ModelMatrix", &m );
            glDrawArrays( GL_LINE_STRIP, 0, 5 );
            mat3_translatef( &m, 50.f, 25.f );
            Shader_sendMat3( "ModelMatrix", &m );
            glDrawArrays( GL_LINE_STRIP, 0, 5 );
            glEnableVertexAttribArray( 1 );
        Device_endFrame();
    }

    return_val = 0;

error :

    Game_destroy();

    return return_val;
  */ 
}

