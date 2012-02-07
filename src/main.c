#include "color.h"
#include "device.h"
#include "scene.h"
#include "world.h"
#include "renderer.h"
#include "context.h"
#include "actor.h"

#include "GL/glew.h"


int main() {
    int return_val = -1;

    check( Device_init(), "Error while creating Device, exiting program.\n" );


    printf( "Hello, Byte World!!\n" );

    str64 date;
    str16 time;
    GetTime( date, 64, DateFmt );
    GetTime( time, 16, TimeFmt );

    strncat( date, " - ", 3 );
    strncat( date, time, 16 );
    printf( "%s\n\n", date );



    // World initialization
    check( World_init(), "Error in world creation. Exiting program\n" );
    check( World_loadAllResources(), "Error in resource loading, exiting program!\n" );

    Scene *scene = Scene_new();



    Actor actor1;
    check( Actor_load( &actor1, "data/actors/actor1.json" ), "Error while loading actor1!\n" );

    Actor actor2;
    check( Actor_load( &actor2, "data/actors/actor2.json" ), "Error while loading actor2!\n" );

    



    // ###############################3
    //      Entities
    mat3 ModelMatrix, MM;
    mat3_identity( &ModelMatrix );
    mat3_rotatef( &ModelMatrix, 45.f );
    mat3_scalef( &ModelMatrix, 2.f, 1.f );

    mat3_translatef( &ModelMatrix, 400.f, 300.f );

    mat3_cpy( &MM, &ModelMatrix );
    mat3_translatef( &MM, 9.f, 3.f );

    int ent2_depth = -1;

    int actor1_entity = Scene_addEntityFromActor( scene, &actor1 );
    check( actor1_entity >= 0, "error creating actor1_entity!\n" );
    Scene_modifyEntity( scene, actor1_entity, EA_Depth, &ent2_depth );

    int actor2_entity = Scene_addEntityFromActor( scene, &actor2 );
    check( actor2_entity >= 0, "error creating actor2_entity!\n" );
    Scene_modifyEntity( scene, actor2_entity, EA_Depth, &ent2_depth );


    // ###############################3
    //      TEXT
    Font *f = Font_get( "DejaVuSans.ttf", 12 );
    Color col = { 0.6f, 0.6f, 0.6f, 1.f };
    int text = Scene_addText( scene, f, col );

    check( text >= 0, "error creating text!\n" );

    Scene_modifyText( scene, text, TA_String, "Hello World!!" );




    ////////////////////////////////////

    int cpt = 0;
    f32 accum = 0, accum2 = 0;
    f32 frame_time;

    while( !IsKeyUp( K_Escape ) && Context_isWindowOpen() ) {
        Device_beginFrame();
            Scene_update( scene );

            // Timer stuff
            frame_time = Device_getFrameTime();
            accum += frame_time;

            if( accum > 1.f ) {
                ++cpt;
                accum = 0.f;

                str64 fps_str;
                MSG( fps_str, 64, "FPS : %4.0f", (1.f/frame_time) );

                Scene_modifyText( scene, text, TA_String, fps_str );
            }
            if( accum2 > 3.f && accum2 < 4.f ) {
                mat3_cpy( &actor1.mPosition, &MM );
                accum2 = 5.f;
            } else
                accum2 += frame_time;

            ////////////
            //  object move (ent2)
            vec2 ent2_move = { 0, 0 };
            if( IsKeyDown( K_Up ) )
                ent2_move.y -= 0.01;
            if( IsKeyDown( K_Down ) )
                ent2_move.y += 0.01;
            if( IsKeyDown( K_Left ) )
                ent2_move.x -= 0.01;
            if( IsKeyDown( K_Right ) )
                ent2_move.x += 0.01;
            if( ent2_move.x != 0 || ent2_move.y != 0 )
                mat3_translatefv( &MM, &ent2_move ); 

            Scene_render( scene );
        Device_endFrame();
    }

    return_val = 0;

error :

    Scene_destroy( scene );
    World_destroy();
    Device_destroy();
    return return_val;
}

