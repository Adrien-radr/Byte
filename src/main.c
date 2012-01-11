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

#include "GL/glew.h"

Shader defShader;

void Clean() {
    Renderer_destroy();
    Context_destroy();
    EventManager_destroy();

    CloseLog();
}

void listener( const Event* pEvent ) {
    if( pEvent->mType == E_CharPressed )
        printf( "%c\n", pEvent->mChar );
}

void ResizeCallback() {
    const mat3 *pm = Context_getProjectionMatrix();
    if( pm ) {
        Shader_bind( &defShader );
            Shader_sendMat3( &defShader, "ProjectionMatrix", pm );
        Shader_bind( 0 );
    }
}

int main() {
    InitLog();


    printf( "Hello, Byte World!!\n" );

    str64 date;
    str16 time;
    GetTime( date, 64, DateFmt );
    GetTime( time, 16, TimeFmt );

    strncat( date, " - ", 3 );
    strncat( date, time, 16 );

    printf( "%s\n\n", date );


    str32 title;
    MSG( title, 32, "Byte-Project v%d.%d.%d", BYTE_MAJOR, BYTE_MINOR, BYTE_PATCH );

    check( Context_init( 800, 600, false, title, 0 ), "Error while creating Context!\n" );
    Context_setResizeCallback( ResizeCallback );

    check( Renderer_init(), "Error while creating Renderer!\n" );

    EventManager_init();
    EventManager_addListener( LT_KeyListener, listener );

    printf( "\n\n" );




    vec2 data[] = {
        { .x = -5.f, .y = -5.f },
        { .x = -5.f, .y = 5.f },
        { .x = 5.f, .y = 5.f },

        { .x = -5.f, .y = -5.f },
        { .x = 5.f, .y = 5.f },
        { .x = 5.f, .y = -5.f }
    };



    Mesh *m = NULL;


    // shader
    check( Shader_buildFromFile( &defShader, "default.vs", "default.fs" ), "Error in shader creation.\n" );

    Shader_bind( &defShader );
    Shader_sendMat3( &defShader, "ProjectionMatrix", Context_getProjectionMatrix() );
    Shader_bind( 0 );


    int vao = Renderer_beginVao();
    check( vao >= 0, "Could not create vao!\n" );
    glEnableVertexAttribArray( 0 );
    //glEnableVertexAttribArray( 1 );

    m = Mesh_new();
    Mesh_addVbo( m, MA_Position, data, sizeof( data ) );
    
    mat3 ModelMatrix, MM;
    mat3_scalef( &ModelMatrix, 2.f, 2.f );
    mat3_rotatef( &ModelMatrix, 45.f );

    mat3_translatef( &ModelMatrix, 50.f, 200.f );

    mat3_scalef( &MM, 3.f, 3.f );
    mat3_translatef( &MM, 500.f, 400.f );


    while( !IsKeyUp( K_Escape ) && Context_isWindowOpen() ) {
        EventManager_update();
        Context_update();


        glClear( GL_COLOR_BUFFER_BIT );

        
        Shader_bind( &defShader );
            Shader_sendMat3( &defShader, "ModelMatrix", &MM );

            Mesh_bind( m );
            glDrawArrays( GL_TRIANGLES, 0, m->mVertexCount );

            Shader_sendMat3( &defShader, "ModelMatrix", &ModelMatrix );
            glDrawArrays( GL_TRIANGLES, 0, m->mVertexCount );
        Shader_bind( 0 );

        Context_swap();
    }

    Mesh_destroy( m );
    Clean();

    return 0;

error :
    Mesh_destroy( m );
    Clean();
    return -1;
}

