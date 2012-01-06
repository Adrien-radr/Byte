#include "shader.h"
#include "context.h"

#include "GL/glew.h"

GLuint BuildShader( const char *pSrc, GLenum pType ) {
    GLuint shader = glCreateShader( pType );

    glShaderSource( shader, 1, &pSrc, NULL );
    glCompileShader( shader );

    GLint status;
    glGetShaderiv( shader, GL_COMPILE_STATUS, &status );

    if( !status ) {
        GLint len;
        glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &len );

        GLchar *log = (GLchar*)malloc( len );
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

    check( ReadFile( &v_src, pVFile ), "Failed to read Vertex Shader file \"%s\"!\n", pVFile );
    check( ReadFile( &f_src, pFFile ), "Failed to read Fragment Shader file \"%s\"!\n", pFFile );

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


    pShader->mProgram = glCreateProgram();

    // Compile vertex & fragment shaders
    v_shader = BuildShader( pVSrc, GL_VERTEX_SHADER );
    check( v_shader, "Failed to build Vertex shader.\n" );

    f_shader = BuildShader( pFSrc, GL_FRAGMENT_SHADER );
    check( f_shader, "Failed to build Fragment shader.\n" );

    glAttachShader( pShader->mProgram, v_shader );
    glAttachShader( pShader->mProgram, f_shader );

    glDeleteShader( v_shader );
    glDeleteShader( f_shader );
    

    // Check link status
    glLinkProgram( pShader->mProgram );

    GLint status;
    glGetProgramiv( pShader->mProgram, GL_LINK_STATUS, &status );

    if( !status ) {
        GLint len;
        glGetProgramiv( pShader->mProgram, GL_INFO_LOG_LENGTH, &len );

        GLchar *log = malloc( len );
        glGetProgramInfoLog( pShader->mProgram, len, NULL, log );

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
    glDeleteProgram( pShader->mProgram );
    return 0;
}

void Shader_destroy( Shader *pShader ) {
    if( pShader )
        glDeleteProgram( pShader->mProgram );
}

void Shader_bind( Shader *pShader ) {
    GLuint program = ( pShader ? pShader->mProgram : 0 );
    glUseProgram( program );
}

void Shader_sendVec2( Shader *pShader, const char *pVarName, const vec2 *pVector ) {
    glUniform2fv( glGetUniformLocation( pShader->mProgram, pVarName), 1, &pVector->x );
    CheckGLError();
}

void Shader_sendMat3( Shader *pShader, const char *pVarName, const mat3 *pMatrix ) {
    glUniformMatrix3fv( glGetUniformLocation( pShader->mProgram, pVarName), 1, GL_FALSE, pMatrix->x );
    CheckGLError();
}
