#include "renderer.h"
#include "GL/glew.h"

/// Application Renderer
struct s_Renderer {
    u32     *mVaos;
    u32     mVaoCpt;
    u32     mVaoSize;
};

/// Renderer only instance definition
Renderer *renderer = NULL;


bool Renderer_init() {
    check( !renderer, "Renderer already created!\n" );

    renderer = malloc( sizeof( Renderer ) );
    check_mem( renderer );
    
    // initial number of 10 vaos
    renderer->mVaos = malloc( 10 * sizeof( u32 ) );
    check_mem( renderer->mVaos );

    renderer->mVaoCpt = 0;
    renderer->mVaoSize = 10;


    log_info( "Renderer successfully initialized!\n" );

    return true;
error:
    if( renderer ) {
        DEL_PTR( renderer->mVaos );
        DEL_PTR( renderer );
    }
    return false;
}

void Renderer_destroy() {
    if( renderer ) {
        DEL_PTR( renderer->mVaos );
        DEL_PTR( renderer );
    }
}

int Renderer_beginVao() {
    if( renderer ) {
        // check if Vao array is not already full
        if( renderer->mVaoCpt == renderer->mVaoSize ) {
            u32 size = renderer->mVaoSize;
            renderer->mVaoSize = size * 2;

            u32 tmp[size];

            memcpy( tmp, renderer->mVaos, size * sizeof( u32 ) );
            renderer->mVaos = realloc( renderer->mVaos, renderer->mVaoSize * sizeof( u32 ) );
            check_mem( renderer->mVaos );
            
            memcpy( renderer->mVaos, tmp, size * sizeof( u32 ) );
        }

        // create and bind a new VAO
        glGenVertexArrays( 1, &renderer->mVaos[renderer->mVaoCpt] );
        glBindVertexArray( renderer->mVaos[renderer->mVaoCpt] );


        return renderer->mVaoCpt++;
    }   

error:
        return -1;
}

void Renderer_endVao() {
    if( renderer ) {
        glBindVertexArray( 0 );
    }
}

void Renderer_bindVao( u32 pIndex ) {
    glBindVertexArray( renderer->mVaos[pIndex] );
}
