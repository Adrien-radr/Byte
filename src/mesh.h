#ifndef BYTE_MESH_HPP
#define BYTE_MESH_HPP

#include "common.h"
#include "vector.h"


/// Mesh structure
typedef struct s_Mesh {
    u32     mVbo[1];        ///< Vbos array (0 = position)
    u32     mIbo;           ///< Indices of Mesh

    u32     mIndexCount;    ///< Index count
    u32     mVertexCount;   ///< Vertex count

    f32     *mData;         ///< Mesh vertices array stored in RAM
    u32     *mIndices;      ///< Mesh indices array stored in RAM
} Mesh;

/// Mesh attrib type in VBOs
typedef enum e_MeshAttrib {
    MA_Position = 0
} MeshAttrib;

/// Initialize a new mesh pointer
Mesh* Mesh_new();

/// Destroy an existing mesh and delete its GL buffers
void Mesh_destroy( Mesh *pMesh );

/// Add a VBO to the given mesh
/// @param MeshAttrib : Attrib type of the data
void Mesh_addVbo( Mesh *pMesh, u32 MeshAttrib, vec2 *pArray, u32 pArraySize );

/// Add an IBO to the given mesh
void Mesh_addIbo( Mesh *pMesh, u32 *pArray, u32 pArraySize );

/// Bind the given mesh vbo to the GL state machine
void Mesh_bind( Mesh *pMesh );

/// Unbind all VBOs
void Mesh_unbind();

#endif // BYTE_MESH
