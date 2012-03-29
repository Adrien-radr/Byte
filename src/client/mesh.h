#ifndef BYTE_MESH_HPP
#define BYTE_MESH_HPP

#include "common/common.h"
#include "common/vector.h"


/// Mesh structure
typedef struct s_Mesh {
    u32     mVbo;           ///< Vertices Data of mesh
    u32     mIbo;           ///< Indices Data of Mesh

    u32     mIndexCount;    ///< Index count
    u32     mVertexCount;   ///< Vertex count

    f32     *mData;         ///< Mesh vertices array stored in RAM
    u32     *mIndices;      ///< Mesh indices array stored in RAM

    size_t  mTexcoordBegin; ///< Begining position in mData of Texcoord Data
    bool    mUseIndices;    ///< True if we use indices for drawing
} Mesh;

/// Mesh attrib type in VBOs
typedef enum e_MeshAttrib {
    MA_Position = 0,
    MA_Texcoord = 1
} MeshAttrib;

/// Initialize a new mesh pointer
Mesh* Mesh_new();

/// Destroy an existing mesh and delete its GL buffers
void Mesh_destroy( Mesh *pMesh );

/// Copy a mesh (all its RAM stored data) in another
void Mesh_cpy( Mesh *dest, const Mesh *src );

/// Resize the vertices of a mesh
void Mesh_resize( Mesh *pMesh, const vec2 *pSize );


// ###############################################################
//      STATIC MESH
// ###############################################################
    /// Add vertex data(position and texture coords) to a given mesh
    bool Mesh_addVertexData( Mesh *pMesh, vec2 *pPositions, u32 pPositionSize, vec2 *pTexcoords, u32 pTexcoordSize );

    /// Add index data to a given mesh
    bool Mesh_addIndexData( Mesh *pMesh, u32 *pIndices, u32 pIndexSize );

    /// Builds a given mesh. It have to be filled with Vertex and Index data before!
    /// @param pDynamic : tells if the vbo/ibo must be allocated 'dynamically' by OpenGL
    void Mesh_build( Mesh *pMesh, bool pDynamic );



/// Bind the given mesh vbo to the GL state machine
void Mesh_bind( Mesh *pMesh );

/// Unbind all VBOs
void Mesh_unbind();

#endif // BYTE_MESH
