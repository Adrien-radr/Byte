#include "game.h"

#include "color.h"
#include "actor.h"
#include "renderer.h"

#include "GL/glew.h"

#define ACT_NUM 50

int main() {
    int return_val = -1;

    check( Game_init(), "Error while initializing Game. Exiting program!\n" );


    printf( "Hello, Byte World!!\n" );

    str64 date;
    str16 time;
    GetTime( date, 64, DateFmt );
    GetTime( time, 16, TimeFmt );

    strncat( date, " - ", 3 );
    strncat( date, time, 16 );
    printf( "%s\n\n", date );

    Actor actor1;
    check( Actor_load( &actor1, "data/actors/actor2.json" ), "Error while loading actor1!\n" );
    // ###############################34096
    //      Entities
    int ent2_depth = -1;

    int actor1_entity = Scene_addEntityFromActor( game->mScene, &actor1 );
    check( actor1_entity >= 0, "error creating actor1_entity!\n" );
    Scene_modifyEntity( game->mScene, actor1_entity, EA_Depth, &ent2_depth );
/*
    Actor actors[ACT_NUM*ACT_NUM];
    for( int i = 0; i < ACT_NUM*ACT_NUM; ++i )
        check( Actor_load( &actors[i], "data/actors/actor2.json" ), "Error while loading actors[%d]!\n", i );
    //Actor actor2;

    




    int actor_ent;
    for( int i = 0; i < ACT_NUM; ++i ) {
        for( int j = 0; j < ACT_NUM; ++j ) {
            actor_ent = Scene_addEntityFromActor( game->mScene, &actors[i*ACT_NUM + j] );
            check( actor_ent >= 0, "error creating actor_ent(%d)!\n", i*ACT_NUM+j );
            Scene_modifyEntity( game->mScene, actor_ent, EA_Depth, &ent2_depth );
            Actor_setPositionf( &actors[i*ACT_NUM + j], 50 + i * 10.f, 50 + j * 10.f );
        }
    }
*/
    // ###############################3
    //      TEXT
    Font *f = Font_get( "DejaVuSans.ttf", 12 );
    Color col = { 0.6f, 0.6f, 0.6f, 1.f };
    int text = Scene_addText( game->mScene, f, col );

    check( text >= 0, "error creating text!\n" );

    Scene_modifyText( game->mScene, text, TA_String, "Hello World!!" );




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
        Device_endFrame();
    }

    return_val = 0;

error :

    Game_destroy();
    return return_val;
}

