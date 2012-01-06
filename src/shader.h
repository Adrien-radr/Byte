#ifndef BYTE_SHADER_HPP
#define BYTE_SHADER_HPP

#include "common.h"
#include "vector.h"
#include "matrix.h"

typedef struct s_Shader {
    u32 mProgram;
} Shader;

/// Build and Link a GLSL program, with a Vertex and a Fragment shader
/// @param pVSrc : string containing the vertex shader source
/// @param pFSrc : string containing the fragment shader source
/// @return : True if everything went well, false otherwise
bool Shader_build( Shader *pShader, const char *pVSrc, const char *pFSrc );

/// Build and Ling a GLSL program, with a Vertex and a Fragment shader
/// @param pVFile : string containing the path to the vertex shader file
/// @param pFFile : string containing the path to the fragment shader file
/// @return : True if everything went well, false otherwise
bool Shader_buildFromFile( Shader *pShader, const char *pVFile, const char *pFFile );

/// Destroy a GLSL shader program
void Shader_destroy( Shader *pShader );

/// Bind a GLSL shader program ( unbind if pShader = 0 or NULL )
void Shader_bind( Shader *pShader );

/// Send a uniform vec2 to the shader
/// @param pVarName : Variable name in shader src
/// @param pVector  : Vector to send
void Shader_sendVec2( Shader *pShader, const char *pVarName, const vec2 *pVector );

/// Send a uniform mat3 to the shader
/// @param pVarName : Variable name in shader src
/// @param pVector  : Matrix to send
void Shader_sendMat3( Shader *pShader, const char *pVarName, const mat3 *pMatrix );

#endif // BYTE_SHADER
