#include "common.h"
#include "keys.h"
#include "color.h"
#include "clock.h"
#include "vector.h"
#include "context.h"
#include "event.h"
#include "shader.h"
#include "renderer.h"
#include "mesh.h"
#include "device.h"
#include "handlemanager.h"
#include "texture.h"
#include "GL/glew.h"

/*
            TODO

    - 


*/

void listener( const Event* pEvent ) {
    if( pEvent->mType == E_CharPressed )
        printf( "%c\n", pEvent->mChar );
}


int main() {

    check( Device_init(), "Error while creating Device, exiting program.\n" );

    EventManager_addListener( LT_KeyListener, listener );

    printf( "Hello, Byte World!!\n" );

    str64 date;
    str16 time;
    GetTime( date, 64, DateFmt );
    GetTime( time, 16, TimeFmt );

    strncat( date, " - ", 3 );
    strncat( date, time, 16 );

    printf( "%s\n\n", date );




    // ###############################3
    //      TEXTURE
    int texture = Renderer_createTexture( "img_test.png" );
    Renderer_useTexture( texture, 0 );


    // ###############################3
    //      SHADER
    int shader = Renderer_createShader( "default.vs", "default.fs" );
    check( shader >= 0, "Error in shader creation. Exiting program!\n" );

    Renderer_useShader( shader );
    Shader_sendMat3( "ProjectionMatrix", Context_getProjectionMatrix() );
    Shader_sendTexture( "Texture", 0 );



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


    u32 mesh = Renderer_createMesh( indices, sizeof( indices ), data, sizeof( data ), texcoords, sizeof( texcoords ) );
    check( mesh >= 0, "Error while creating mesh!\n" );

    
    // ###############################3
    //      MATRICES
    mat3 ModelMatrix, MM;
    mat3_scalef( &ModelMatrix, 2.f, 2.f );
    mat3_rotatef( &ModelMatrix, 45.f );

    mat3_translatef( &ModelMatrix, 50.f, 200.f );

    mat3_scalef( &MM, 3.f, 3.f );
    mat3_translatef( &MM, 500.f, 400.f );

    CheckGLError();

    while( !IsKeyUp( K_Escape ) && Context_isWindowOpen() ) {
        Device_beginFrame();
            Renderer_useShader( shader );
                Shader_sendMat3( "ModelMatrix", &MM );
                Renderer_renderMesh( mesh );

                Shader_sendMat3( "ModelMatrix", &ModelMatrix );
                Renderer_renderMesh( mesh );
            Renderer_useShader( -1 );
        Device_endFrame();
    }


    Device_destroy();

    return 0;

error :

    Device_destroy();
    return -1;
}

