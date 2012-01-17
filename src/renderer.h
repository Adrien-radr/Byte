#ifndef BYTE_RENDERER_HPP
#define BYTE_RENDERER_HPP

#include "common.h"
#include "vector.h"
#include "matrix.h"

typedef struct s_Renderer Renderer;

/// Initialize a renderer and returns it
bool Renderer_init();

/// Destroy a given renderer
void Renderer_destroy();

/// Function called at the begining of every rendering phase
void Renderer_beginFrame();

/// Update the projection matrix used to render in shaders
void Renderer_updateProjectionMatrix( const mat3 *pm );


// ##########################################################################3
//      MESH UTILS
// ##########################################################################3
    /// Begin a new VAO in the given Renderer
    /// @return : the indice in the Renderer VAO array of the created VAO
    int  Renderer_beginVao();

    /// End the current VAO in the current Renderer
    void Renderer_endVao();

    /// Bind a Vao given its index
    void Renderer_bindVao( u32 pIndex );

    /// Create a new mesh and return its ID
    int  Renderer_createMesh( u32 *pIndices, u32 pIndicesSize, vec2 *pPositions, u32 pPositionsSize, vec2 *pTexcoords, u32 pTexcoordsSize );

    /// Render a mesh given its identifier
    void Renderer_renderMesh( u32 pIndex );


// ##########################################################################3
//      SHADER UTILS
// ##########################################################################3
    /// Create a new shader and return its ID
    int  Renderer_createShader( const char *pVFile, const char *pFFile );

    /// Sets the renderer to use a given shader
    /// @param pShader : shader index in renderer shader array. -1 to unbind all shaders
    void Renderer_useShader( int pShader );

    /// Returns the currently bound shader
    int  Renderer_currentShader();

    /// Returns the currently bound GL program
    u32  Renderer_currentGLProgram();


// ##########################################################################3
//      TEXTURE UTILS
// ##########################################################################3
    /// Create a new texture from file and return its ID
    int  Renderer_createTexture( const char *pTFile );

    /// Sets the renderer to use a given texture in the given unit
    void Renderer_useTexture( int pTexture, u32 pTarget );


// OpenGL Error Handling
    /// Check general Opengl Error and log it
    void CheckGLError_func( const char *pFile, u32 pLine );
#   define CheckGLError() CheckGLError_func( __FILE__, __LINE__ )


#endif // BYTE_RENDERER
