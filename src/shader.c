#include "shader.h"
#include "renderer.h"

#include "GL/glew.h"


Shader *Shader_new() {
    Shader *s = byte_alloc( sizeof( Shader ) );

    s->mUseProjectionMatrix = true;

    return s;
}

GLuint BuildShader( const char *pSrc, GLenum pType ) {
    GLuint shader = glCreateShader( pType );

    glShaderSource( shader, 1, &pSrc, NULL );
    glCompileShader( shader );

    GLint status;
    glGetShaderiv( shader, GL_COMPILE_STATUS, &status );

    if( !status ) {
        GLint len;
        glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &len );

        GLchar *log = byte_alloc( len );
        glGetShaderInfoLog( shader, len, NULL, log );

                        
        log_err( "Shader compilation error : \n" 
                 "-----------------------------------------------------\n"
                 "%s"
                 "-----------------------------------------------------\n", log );

        DEL_PTR( log );
        shader = 0;
    }
    return shader;
}

bool Shader_buildFromFile( Shader *pShader, const char *pVFile, const char *pFFile ) {
    check( pShader, "In Shader_buildFromFile(), given shader is uninitialized!\n" );
    check( pVFile, "In Shader_buildFromFile(), Vertex shader file path is uninitialized!\n" );
    check( pFFile, "In Shader_buildFromFile(), Fragment shader file path is uninitialized!\n" );


    // Get shaders source
    char *v_src = NULL, *f_src = NULL;

    u32 v_src_size = ReadFile( &v_src, pVFile );
    check( v_src_size, "Failed to read Vertex Shader file \"%s\"!\n", pVFile );

    u32 f_src_size = ReadFile( &f_src, pFFile );
    check( f_src_size, "Failed to read Fragment Shader file \"%s\"!\n", pFFile );

    // create shader from sources
    check( Shader_build( pShader, v_src, f_src ), "Failed to compile shader program from sources \"%s\" and \"%s\".\n", pVFile, pFFile );


    DEL_PTR( v_src );
    DEL_PTR( f_src ); 

    return 1;

error:
    DEL_PTR( v_src );
    DEL_PTR( f_src );
    return 0;
}


bool Shader_build( Shader *pShader, const char *pVSrc, const char *pFSrc ) {
    GLuint v_shader = 0, f_shader = 0;

    check( pShader, "In Shader_build(), given shader is uninitialized!\n" );
    check( pVSrc, "In Shader_build(), given Vertex Shader source is uninitialized!\n" );
    check( pFSrc, "In Shader_build(), given Fragment Shader source is uninitialized!\n" );


    pShader->mID = glCreateProgram();

    // Compile vertex & fragment shaders
    v_shader = BuildShader( pVSrc, GL_VERTEX_SHADER );
    check( v_shader, "Failed to build Vertex shader.\n" );

    f_shader = BuildShader( pFSrc, GL_FRAGMENT_SHADER );
    check( f_shader, "Failed to build Fragment shader.\n" );

    glAttachShader( pShader->mID, v_shader );
    glAttachShader( pShader->mID, f_shader );

    glDeleteShader( v_shader );
    glDeleteShader( f_shader );
    

    // Check link status
    glLinkProgram( pShader->mID );

    GLint status;
    glGetProgramiv( pShader->mID, GL_LINK_STATUS, &status );

    if( !status ) {
        GLint len;
        glGetProgramiv( pShader->mID, GL_INFO_LOG_LENGTH, &len );

        GLchar *log = byte_alloc( len );
        glGetProgramInfoLog( pShader->mID, len, NULL, log );

        log_err( "Shader Program link error : \n"
                 "-----------------------------------------------------\n"
                 "%s"
                 "-----------------------------------------------------\n", log );

        
        DEL_PTR( log );
    }
    CheckGLError();

    return 1;
error:
    glDeleteShader( v_shader );
    glDeleteShader( f_shader );
    glDeleteProgram( pShader->mID );
    return 0;
}

void Shader_destroy( Shader *pShader ) {
    if( pShader )
        glDeleteProgram( pShader->mID );
    DEL_PTR( pShader );
}

void Shader_bind( Shader *pShader ) {
    GLuint program = ( pShader ? pShader->mID : 0 );
    glUseProgram( program );
}

void Shader_sendVec2( const char *pVarName, const vec2 *pVector ) {
    glUniform2fv( glGetUniformLocation( Renderer_currentGLProgram(), pVarName), 1, &pVector->x );
    CheckGLError();
}

void Shader_sendMat3( const char *pVarName, const mat3 *pMatrix ) {
    glUniformMatrix3fv( glGetUniformLocation( Renderer_currentGLProgram(), pVarName), 1, GL_FALSE, pMatrix->x );
    CheckGLError();
}

void Shader_sendInt( const char *pVarName, int pValue ) {
    glUniform1i( glGetUniformLocation( Renderer_currentGLProgram(), pVarName), pValue );
    CheckGLError();
}
