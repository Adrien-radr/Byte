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

typedef enum e_MeshAttrib {
    MA_Position = 0
} MeshAttrib;

Mesh* Mesh_new();
void Mesh_destroy( Mesh *pMesh );
void Mesh_addVbo( Mesh *pMesh, u32 MeshAttrib, vec2 *pArray, u32 pArraySize );
void Mesh_addIbo( Mesh *pMesh, u32 *pArray, u32 pArraySize );

void Mesh_bind( Mesh *pMesh );
void Mesh_unbind();

#endif // BYTE_MESH
