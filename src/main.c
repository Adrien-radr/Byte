#include "color.h"
#include "shader.h"
#include "device.h"
#include "camera.h"
#include "GL/glew.h"
#include "scene.h"
#include "world.h"
#include "renderer.h"
#include "context.h"


int main() {
    check( Device_init(), "Error while creating Device, exiting program.\n" );


    printf( "Hello, Byte World!!\n" );

    str64 date;
    str16 time;
    GetTime( date, 64, DateFmt );
    GetTime( time, 16, TimeFmt );

    strncat( date, " - ", 3 );
    strncat( date, time, 16 );
    printf( "%s\n\n", date );




    World *world = World_new();
    check( world, "Error in world creation!\n" );

    check( World_loadAllResources( world ), "Error in resource loading, exiting program!\n" );

    // ###############################3
    //      TEXTURE
    int t1 = World_getResource( world, "crate.jpg" );
    int texture = World_getResource( world, "img_test.png" );
    check( texture >= 0 && t1 >= 0, "Error in texture creation. Exiting program!\n" );
    Renderer_useTexture( t1, 0 );


    // ###############################3
    //      SHADER
    int shader = World_getResource( world, "defaultShader.json" ); 
    check( shader >= 0, "Error in shader creation. Exiting program!\n" );



    // ###############################3
    //      MESH
    vec2 data[] = {
        { .x = -5.f, .y = -5.f },
        { .x = -5.f, .y = 5.f },
        { .x = 5.f, .y = 5.f },
        { .x = 5.f, .y = -5.f }
    };

    vec2 texcoords[] = {
        { .x = 0.f, .y = 0.f },
        { .x = 0.f, .y = 1.f },
        { .x = 1.f, .y = 1.f },
        { .x = 1.f, .y = 0.f }
    };
    
    u32 indices[] = {
        0, 1, 2, 0, 2, 3
    };

    int vao = Renderer_beginVao();
    check( vao >= 0, "Could not create vao!\n" );
    glEnableVertexAttribArray( 0 );
    glEnableVertexAttribArray( 1 );


    u32 mesh = Renderer_createStaticMesh( indices, sizeof( indices ), data, sizeof( data ), texcoords, sizeof( texcoords ) );
    check( mesh >= 0, "Error while creating mesh!\n" );

    
    // ###############################3
    //      MATRICES
    mat3 ModelMatrix, MM;
    mat3_identity( &ModelMatrix );
    mat3_rotatef( &ModelMatrix, 45.f );
    mat3_scalef( &ModelMatrix, 2.f, 1.f );

    mat3_translatef( &ModelMatrix, 400.f, 300.f );

    mat3_cpy( &MM, &ModelMatrix );
    mat3_translatef( &MM, 9.f, 3.f );


    Scene *scene = Scene_new( world );


    int ent1 = Scene_addEntity( scene, mesh, t1, ModelMatrix );
    check( ent1 >= 0, "error creating ent1!\n" );


    int ent2 = Scene_addEntity( scene, mesh, texture, MM );
    check( ent2 >= 0, "error creating ent2!\n" );

    int ent2_depth = -2;

    Scene_modifyEntity( scene, ent2, EA_Depth, &ent2_depth );


    // ###############################3
    //      TEXT
    Font *f = Font_get( world, "DejaVuSans.ttf", 12 );
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


    Scene_destroy( scene );
    World_destroy( world );
    Device_destroy();

    return 0;

error :

    Scene_destroy( scene );
    World_destroy( world );
    Device_destroy();
    return -1;
}

