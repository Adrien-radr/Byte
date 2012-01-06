#include "common.h"
#include "keys.h"
#include "color.h"
#include "clock.h"
#include "vector.h"
#include "context.h"
#include "event.h"
#include "shader.h"

#include "GL/glew.h"

void Clean() {
    Context_destroy();
    EventManager_destroy();

    CloseLog();
}

void listener( const Event* pEvent ) {
    if( pEvent->mType == E_CharPressed )
        printf( "%c\n", pEvent->mChar );
}

int main() {
    InitLog();


    printf( "Hello, Byte World!!\n\n" );

    str64 date;
    str16 time;
    GetTime( date, 64, DateFmt );
    GetTime( time, 16, TimeFmt );

    strncat( date, " - ", 3 );
    strncat( date, time, 16 );

    printf( "%s\n", date );


    char title[20];
    MSG( title, 20, "Byte-Project v%d.%d.%d", BYTE_MAJOR, BYTE_MINOR, BYTE_PATCH );

    Context_init( 800, 600, false, title, 0 );
    EventManager_init();

    printf( "\n\n" );


    // vertex data
    const float vertexData[] = {
        -5.f,    -5.f, 1.f,
        -5.f,     5.f, 1.f,
         5.f,     5.f, 1.f,

        -5.f,    -5.f, 1.f,
         5.f,     5.f, 1.f,
         5.f,    -5.f, 1.f,


        1.0f,    0.0f, 0.0f, 1.0f,
        0.0f,    1.0f, 0.0f, 1.0f,
        0.0f,    0.0f, 1.0f, 1.0f,

        1.0f,    0.0f, 0.0f, 1.0f,
        0.0f,    0.0f, 1.0f, 1.0f,
        1.0f,    0.0f, 1.0f, 1.0f,
    };

    u32 vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    u32 vbo;
    glGenBuffers( 1, &vbo );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
        glBufferData( GL_ARRAY_BUFFER, sizeof( vertexData ), vertexData, GL_STATIC_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );



    // shader
    Shader defShader;
    check( Shader_buildFromFile( &defShader, "default.vs", "default.fs" ), "Error in shader creation.\n" );



    mat3 projMatrix;
    mat3_ortho( &projMatrix, 0.f, 800.f, 600.f, 0.f ); 

    vec2 translation = { .x = 100.f, .y = 50.f };

    Shader_bind( &defShader );
    Shader_sendMat3( &defShader, "MVP", &projMatrix );
    Shader_bind( 0 );

    while( !IsKeyUp( K_Escape ) && Context_isWindowOpen() ) {
        EventManager_update();
        Context_update();


        glClear( GL_COLOR_BUFFER_BIT /* | GL_DEPTH_BUFFER_BIT */ );

        
        Shader_bind( &defShader );
        Shader_sendVec2( &defShader, "offset", &translation );

        glBindBuffer( GL_ARRAY_BUFFER, vbo );
        glEnableVertexAttribArray( 0 );
        glEnableVertexAttribArray( 1 );
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
        glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, 0, (void*)(9 * 2 * sizeof(f32)) );
        glDrawArrays( GL_TRIANGLES, 0, 6 );

        glDisableVertexAttribArray( 0 );
        glDisableVertexAttribArray( 1 );
        Shader_bind( 0 );

        Context_swap();
    }

    Clean();

    return 0;

error :
    Clean();
    return -1;
}

