#include "renderer.h"
#include "mesh.h"

#include "GL/glew.h"


// Array VaoArray with u32 data type
SimpleArray( u32, Vao )

// Array MeshArray with Mesh* data type
HeapArray( Mesh*, Mesh )

/// Application Renderer
struct s_Renderer {
    VaoArray    mVaos;
    MeshArray   mMeshes;
};

/// Renderer only instance definition
Renderer *renderer = NULL;


bool Renderer_init() {
    check( !renderer, "Renderer already created!\n" );

    renderer = malloc( sizeof( Renderer ) );
    check_mem( renderer );
    
    // initial number of 10 vaos and 100 meshes
    VaoArray_init( &renderer->mVaos, 10 );
    
    MeshArray_init( &renderer->mMeshes, 100 );


    // GLEW initialisation
    GLenum glerr = glewInit();

    check( GLEW_OK == glerr, "GLEW Error : %s\n", glewGetErrorString( glerr ) );

    log_info( "GLEW v%s successfully initialized!\n", glewGetString( GLEW_VERSION ) );

    // GL Version 
    int majV, minV;
    glGetIntegerv( GL_MAJOR_VERSION, &majV );
    glGetIntegerv( GL_MINOR_VERSION, &minV );
    log_info( "OpenGL %d.%d\n", majV, minV );
    log_info( "GLSL %s\n", glGetString( GL_SHADING_LANGUAGE_VERSION ) );
    log_info( "Hardware : %s - %s\n", glGetString( GL_VENDOR ), glGetString( GL_RENDERER ) );

    // GL state initialisations
    glHint( GL_GENERATE_MIPMAP_HINT, GL_NICEST );

    glDisable( GL_DEPTH_TEST );
    //glDepthFunc( GL_LESS );

    glDisable( GL_CULL_FACE );
    //glCullFace( GL_BACK );
    //glFrontFace( GL_CCW );

    glClearColor( 0.2f, 0.2f, 0.2f, 1.f );

    // clear gl errors
    glGetError();



    log_info( "Renderer successfully initialized!\n" );

    return true;

error:
    Renderer_destroy();

    return false;
}

void Renderer_destroy() {
    if( renderer ) {
        VaoArray_destroy( &renderer->mVaos );
        MeshArray_destroy( &renderer->mMeshes );
        DEL_PTR( renderer );
    }
}

int Renderer_beginVao() {
    if( renderer ) {
        // check if Vao array is not already full
        if( VaoArray_checkSize( &renderer->mVaos ) ) {
            // create and bind a new VAO
            glGenVertexArrays( 1, &renderer->mVaos.data[renderer->mVaos.cpt] );
            glBindVertexArray( renderer->mVaos.data[renderer->mVaos.cpt] );


            return renderer->mVaos.cpt++;
        }
    }   
    return -1;
}

void Renderer_endVao() {
    if( renderer ) 
        glBindVertexArray( 0 );
}

void Renderer_bindVao( u32 pIndex ) {
    if( renderer )
        glBindVertexArray( renderer->mVaos.data[pIndex] );
}









void CheckGLError_func( const char *pFile, u32 pLine ) {
#ifdef _DEBUG
    GLenum error = glGetError();

    if (error != GL_NO_ERROR) {
        str64 errorStr;
        str256 description;

        switch (error)
        {
            case GL_INVALID_ENUM :
            {
                strncpy( errorStr, "GL_INVALID_ENUM", 64 );
                strncpy( description, "An unacceptable value has been specified for an enumerated argument.", 256 );
                break;
            }

            case GL_INVALID_VALUE :
            {
                strncpy( errorStr, "GL_INVALID_VALUE", 64 );
                strncpy( description, "A numeric argument is out of range.", 256 );
                break;
            }

            case GL_INVALID_OPERATION :
            {
                strncpy( errorStr, "GL_INVALID_OPERATION", 64 );
                strncpy( description, "The specified operation is not allowed in the current state.", 256 );
                break;
            }

            case GL_STACK_OVERFLOW :
            {
                strncpy( errorStr, "GL_STACK_OVERFLOW", 64 );
                strncpy( description, "This command would cause a stack overflow.", 256 );
                break;
            }

            case GL_STACK_UNDERFLOW :
            {
                strncpy( errorStr, "GL_STACK_UNDERFLOW", 64 );
                strncpy( description, "This command would cause a stack underflow.", 256 );
                break;
            }

            case GL_OUT_OF_MEMORY :
            {
                strncpy( errorStr, "GL_OUT_OF_MEMORY", 64 );
                strncpy( description, "There is not enough memory left to execute the command.", 256 );
                break;
            }

            case GL_INVALID_FRAMEBUFFER_OPERATION :
            {
                strncpy( errorStr, "GL_INVALID_FRAMEBUFFER_OPERATION_EXT", 64 );
                strncpy( description, "The object bound to FRAMEBUFFER_BINDING is not \"framebuffer complete\".", 256 );
                break;
            }
        }

        log_err( "OpenGL Error (%s [%d])\n"
                 "-- Error          : %s\n"
                 "-- Description    : %s\n", pFile, pLine, errorStr, description );
    }
#endif
}
