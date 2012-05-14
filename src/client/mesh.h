#ifndef BYTE_MESH_H
#define BYTE_MESH_H

#include "common/common.h"
#include "common/vector.h"


/// Mesh structure
typedef struct s_Mesh {
    u32     vbo;            ///< Vertices Data of mesh
    u32     ibo;            ///< Indices Data of Mesh

    u32     index_count;    ///< Index count
    u32     vertex_count;   ///< Vertex count

    f32     *data;          ///< Mesh vertices array stored in RAM
    u32     *indices;       ///< Mesh indices array stored in RAM

    size_t  texcoord_begin; ///< Begining position in mData of Texcoord Data
    bool    use_indices;    ///< True if we use indices for drawing

    u32     mode;           ///< Drawing mode. Default = GL_TRIANGLES
} Mesh;

/// Mesh attrib type in VBOs
typedef enum e_MeshAttrib {
    MA_Position = 0,
    MA_Texcoord = 1
} MeshAttrib;

/// Mesh reconstruction methods ( additives with | operator )
typedef enum {
    EUpdateVbo  = 1,
    EUpdateIbo  = 2,
    ERebuildVbo = 4,
    ERebuildIbo = 8
} MeshConstruction;

/// Initialize a new mesh pointer
Mesh* Mesh_new( u32 mode );

/// Destroy an existing mesh and delete its GL buffers
void Mesh_destroy( Mesh *mesh );

/// Copy a mesh (all its RAM stored data) in another
void Mesh_cpy( Mesh *dest, const Mesh *src );

/// Resize the vertices of a mesh
void Mesh_resize( Mesh *mesh, const vec2 *pos_size, const vec2 *texcoords_size );


// ###############################################################
//      STATIC MESH
// ###############################################################
    /// Add vertex data(position and texture coords) to a given mesh
    /// @return : whether or not the mesh->data has been rescaled
    bool Mesh_addVertexData( Mesh *mesh, f32 *positions, u32 position_size, f32 *texcoords, u32 texcoord_size );

    /// Add vertex data(combined pos and texcoords) to a given mesh
    /// Texcoords are assumed to begin at the middle of array
    /// @return : whether or not the mesh->data has been rescaled
    bool Mesh_addVertexDataBlock( Mesh *mesh, f32 *data, u32 size );

    /// Add index data to a given mesh
    /// @return : whether or not the mesh->indices has been rescaled
    bool Mesh_addIndexData( Mesh *mesh, u32 *indices, u32 index_size );

    /// Builds a given mesh. It have to be filled with Vertex and Index data before!
    /// @param method : is the vbo and/or the ibo rebuild and/or reuploaded ?
    /// @param dynamic : tells if the vbo/ibo must be allocated 'dynamically' by OpenGL
    void Mesh_build( Mesh *mesh, MeshConstruction method, bool dynamic );



/// Bind the given mesh vbo to the GL state machine
void Mesh_bind( Mesh *mesh );

/// Unbind all VBOs
void Mesh_unbind();

#endif // BYTE_MESH
