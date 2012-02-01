#include "color.h"
#include "device.h"
#include "scene.h"
#include "world.h"
#include "renderer.h"
#include "context.h"
#include "actor.h"



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

    Actor actor1;
    check( Actor_load( &actor1, "data/actors/actor1.json" ), "Error while loading actor1!\n" );
    



    // ###############################3
    //      MATRICES
    mat3 ModelMatrix, MM;
    mat3_identity( &ModelMatrix );
    mat3_rotatef( &ModelMatrix, 45.f );
    mat3_scalef( &ModelMatrix, 2.f, 1.f );

    mat3_translatef( &ModelMatrix, 400.f, 300.f );

    mat3_cpy( &MM, &ModelMatrix );
    mat3_translatef( &MM, 9.f, 3.f );


    Scene *scene = Scene_new();

    int t1 = World_getResource( "crate.jpg" );
    int texture = World_getResource( "img_test.png" );
    int mesh = World_getResource( "quadmesh.json" );
    

    int ent1 = Scene_addEntity( scene, mesh, t1, &ModelMatrix );
    check( ent1 >= 0, "error creating ent1!\n" );


    int ent2 = Scene_addEntity( scene, mesh, texture, &MM );
    check( ent2 >= 0, "error creating ent2!\n" );

    int ent2_depth = -2;

    Scene_modifyEntity( scene, ent2, EA_Depth, &ent2_depth );

    mat3 actor1_mat;
    mat3_translationMatrixfv( &actor1_mat, &actor1.mPosition );
    int actor1_entity = Scene_addEntity( scene, actor1.mMesh_id, actor1.mTexture_id, &actor1_mat );
    check( actor1_entity >= 0, "error creating actor1_entity!\n" );


    // ###############################3
    //      TEXT
    Font *f = Font_get( "DejaVuSans.ttf", 12 );
    Color col = { 0.6f, 0.6f, 0.6f, 1.f };
    int text = Scene_addText( scene, f, col );

    check( text >= 0, "error creating text!\n" );

    Scene_modifyText( scene, text, TA_String, "Hello World!!" );




    ////////////////////////////////////

    int cpt = 0;
    f32 accum = 0;

    while( !IsKeyUp( K_Escape ) && Context_isWindowOpen() ) {
        Device_beginFrame();
            Scene_update( scene );

            // stuff
            accum += Device_getFrameTime();
            if( accum > 1.f ) {
                ++cpt;
                accum = 0.f;

                str64 fps_str;
                MSG( fps_str, 64, "FPS : %4.0f", (1.f/Device_getFrameTime()) );

                Scene_modifyText( scene, text, TA_String, fps_str );
            }


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

