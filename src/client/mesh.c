#include "mesh.h"

#ifdef USE_GLDL
#include "GL/gldl.h"
#else
#include "GL/glew.h"
#endif

Mesh* Mesh_new( GLenum mode ) {
    Mesh *mesh = byte_alloc( sizeof( Mesh ) );

    if( mesh ) 
        mesh->mode = mode;

    return mesh;
}

void Mesh_destroy( Mesh *mesh ) {
    if( mesh ) {
        glDeleteBuffers( 1, &mesh->vbo );
        glDeleteBuffers( 1, &mesh->ibo );
        DEL_PTR( mesh->data );
        DEL_PTR( mesh->indices );
        DEL_PTR( mesh );
    }
}
 
void Mesh_cpy( Mesh *dest, const Mesh *src ) {
    if( dest && src ) {
        dest->vbo = dest->ibo = 0;
        dest->index_count = src->index_count;
        dest->vertex_count = src->vertex_count;
        dest->mode = src->mode;

        dest->data = byte_realloc( dest->data, src->vertex_count * 4 * sizeof( f32 ) );
        memcpy( dest->data, src->data, src->vertex_count * 4 * sizeof( f32 ) );

        if( src->use_indices ) {
            dest->indices = byte_realloc( dest->indices, src->index_count * sizeof( u32 ) );
            memcpy( dest->indices, src->indices, src->index_count * sizeof( u32 ) );
        }

        dest->texcoord_begin = src->texcoord_begin;
        dest->use_indices = src->use_indices;
    }
}

void Mesh_resize( Mesh *mesh, const vec2 *size ) {
    if( mesh && size ) {
        for( u32 i = 0; i < mesh->vertex_count; ++i ) {
            mesh->data[2*i] *= size->x;
            mesh->data[2*i+1] *= size->y;
        }
    }
}


bool Mesh_addVertexData( Mesh *mesh, f32 *positions, u32 position_size, f32 *texcoords, u32 texcoord_size ) {
    check( positions, "In Mesh_addMeshData : given Position array is NULL!\n" );
    check( texcoords, "In Mesh_addMeshData : given Texcoords array is NULL!\n" );

    if( mesh ) {
        bool ret = false;

        u32 old_size = mesh->vertex_count;
        mesh->vertex_count = position_size / ( 2 * sizeof(f32) );

        // realloc buffer array if too small (not if too large)
        if( old_size < mesh->vertex_count ) {
            mesh->data = byte_realloc( mesh->data, position_size + texcoord_size );
            ret = true;
        } 

        // Copy data from two arrays of f32 into mesh data array of f32
        memcpy( mesh->data, positions, position_size );
        memcpy( mesh->data + (mesh->vertex_count*2), texcoords, texcoord_size );

        mesh->texcoord_begin = position_size;

        return ret;
    }

error:
    return false;
}

bool Mesh_addVertexDataBlock( Mesh *mesh, f32 *data, u32 size ) {
    check( data, "In Mesh_addVertexDataBlock : given data array is NULL!\n" );

    if( mesh ) {
        bool ret = false;

        u32 old_size = mesh->vertex_count;

        // vertex count = 4floats(px,py,tx,ty) by vertex
        mesh->vertex_count = size / ( 4 * sizeof(f32) );

        // realloc buffer array if too small (not if too large)
        if( old_size < mesh->vertex_count ) {
            mesh->data = byte_realloc( mesh->data, size );
            ret = true;
        } 

        memcpy( mesh->data, data, size );

        mesh->texcoord_begin = size/2;

        return ret;
    }

error:
    return false;
}

bool Mesh_addIndexData( Mesh *mesh, u32 *indices, u32 index_size ) {
    check( indices, "In Mesh_addIndexData : given Indices array is NULL!\n" );
    
    if( mesh ) {
        bool ret = false;

        u32 old_size = mesh->index_count;
        mesh->index_count = index_size / sizeof(u32);

        // realloc buffer array if too small (not if too large)
        if( old_size < mesh->index_count ) {
            mesh->indices = byte_realloc( mesh->indices, index_size );
            ret = true;
        } 

        memcpy( mesh->indices, indices, index_size );

        return ret;
    }
error :
    return false;
}

void Mesh_build( Mesh *mesh, MeshConstruction method, bool dynamic ) {
    check( mesh->data, "In Mesh_build, the given mesh is not complete. Please supply at least Vertex and Index Data before building the mesh!\n" );

    // delete old vbo and create a new one with mesh->data
    if( method & ERebuildVbo ) {
        if( mesh->vbo ) {
            glDeleteBuffers( 1, &mesh->vbo );
            mesh->vbo = 0;
        }

        glGenBuffers( 1, &mesh->vbo );
        glBindBuffer( GL_ARRAY_BUFFER, mesh->vbo );
        glBufferData( GL_ARRAY_BUFFER, mesh->vertex_count * 4 * sizeof( f32 ), mesh->data, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );

    // just re-upload the mesh->data in vbo buffer
    } else if( method & EUpdateVbo ) {
        if( !mesh->vbo )  return;

        glBindBuffer( GL_ARRAY_BUFFER, mesh->vbo );
        glBufferSubData( GL_ARRAY_BUFFER, (GLintptr)0, mesh->vertex_count * 4 * sizeof(f32), mesh->data );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
    }


    // delete old ibo and create a new one with mesh->indices
    if( method & ERebuildIbo ) {
        if( mesh->ibo ) {
            glDeleteBuffers( 1, &mesh->ibo );
            mesh->ibo = 0;
        }

        glGenBuffers( 1, &mesh->ibo );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->ibo );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, mesh->index_count * sizeof( u32 ), mesh->indices, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
        mesh->use_indices = true;
    
    // just re-upload the mesh->indices in ibo buffer
    } else if( method & EUpdateIbo ) {
        if( !mesh->ibo )  return;

        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->ibo );
        glBufferSubData( GL_ELEMENT_ARRAY_BUFFER, (GLintptr)0, mesh->index_count * sizeof(u32), mesh->indices );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    
        mesh->use_indices = true;
    }


error:
    return;
}


void Mesh_bind( Mesh *mesh ) {
    glBindBuffer( GL_ARRAY_BUFFER, mesh->vbo );
    glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, 0 );
    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(mesh->texcoord_begin) );
    glEnableVertexAttribArray( 0 );
    glEnableVertexAttribArray( 1 );

    if( mesh->use_indices )
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->ibo );
}

void Mesh_unbind() {
    glDisableVertexAttribArray( 0 );
    glDisableVertexAttribArray( 1 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}
