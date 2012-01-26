#ifndef BYTE_RENDERER_HPP
#define BYTE_RENDERER_HPP

#include "common.h"
#include "vector.h"
#include "matrix.h"
#include "shader.h"
#include "text.h"

typedef struct s_Renderer Renderer;

/// Initialize a renderer and returns it
bool Renderer_init();

/// Destroy a given renderer
void Renderer_destroy();

/// Function called at the begining of every rendering phase
void Renderer_beginFrame();

/// Update the projection matrix used to render in shaders
void Renderer_updateProjectionMatrix( const mat3 *pm );

/// Returns whether or not the renderer is initialized
bool Renderer_isInitialized();


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

    /// Create a new static mesh and return its ID
    /// This mesh will be set one time and the corresponding VBO will be built directly
    /// @param pIndices : Mesh Index data
    /// @param pPositions : Mesh Position data
    /// @param pTexcoords : Mesh Texture coordinates data
    /// @param p*Size : Size of each array
    /// @return : the handle to the created mesh.
    int  Renderer_createStaticMesh( u32 *pIndices, u32 pIndicesSize, vec2 *pPositions, u32 pPositionsSize, vec2 *pTexcoords, u32 pTexcoordsSize );

    /// Make a dynamic mesh (dont build it yet
    int  Renderer_createDynamicMesh();

    /// Change the data of given mesh, and rebuild VBO
    /// @param pVData : Vertex data (position + texcoords)
    /// @param pIData : Index data
    /// @return : true if everything went well, false otherwise.
    bool Renderer_setDynamicMeshData( u32 pMesh, f32 *pVData, u32 pVSize, u32 *pIData, u32 pISize );

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

    /// Returns a pointer to the shader with the given handle
    Shader *Renderer_getShader( u32 pShader );

    /// Returns the currently bound shader
    int  Renderer_currentShader();

    /// Returns the currently bound GL program
    u32  Renderer_currentGLProgram();


// ##########################################################################3
//      TEXTURE UTILS
// ##########################################################################3
    /// Create a new texture from file and return its ID
    int  Renderer_createTexture( const char *pTFile, bool pMipmaps );

    /// Allocates and register an empty texture to be used afterwards
    int  Renderer_allocateEmptyTexture( Texture **pTexture );

    /// Sets the renderer to use a given texture in the given unit
    void Renderer_useTexture( int pTexture, u32 pTarget );


// ##########################################################################3
//      FONT UTILS
// ##########################################################################3
    /// Create a new font from file and returns its ID
    int  Renderer_createFont( const char *pFile, u32 pSize );

    /// Returns a pointer to an indexed font
    Font *Renderer_getFont( u32 pHandle );


// OpenGL Error Handling
    /// Check general Opengl Error and log it
    void CheckGLError_func( const char *pFile, u32 pLine );
#   define CheckGLError() CheckGLError_func( __FILE__, __LINE__ )


#endif // BYTE_RENDERER
