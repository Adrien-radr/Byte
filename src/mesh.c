#include "mesh.h"

#ifdef USE_GLDL
#include "GL/gldl.h"
#else
#include "GL/glew.h"
#endif

Mesh* Mesh_new() {
    Mesh *mesh = byte_alloc( sizeof( Mesh ) );

    return mesh;
}

void Mesh_destroy( Mesh *pMesh ) {
    if( pMesh ) {
        glDeleteBuffers( 1, &pMesh->mVbo );
        glDeleteBuffers( 1, &pMesh->mIbo );
        DEL_PTR( pMesh->mData );
        DEL_PTR( pMesh->mIndices );
        DEL_PTR( pMesh );
    }
}

void Mesh_cpy( Mesh *dest, const Mesh *src ) {
    if( dest && src ) {
        dest->mVbo = dest->mIbo = 0;
        dest->mIndexCount = src->mIndexCount;
        dest->mVertexCount = src->mVertexCount;

        DEL_PTR( dest->mData );

        dest->mData = byte_alloc( src->mVertexCount * 4 * sizeof( f32 ) );
        memcpy( dest->mData, src->mData, src->mVertexCount * 4 * sizeof( f32 ) );

        if( src->mUseIndices ) {
            DEL_PTR( dest->mIndices );

            dest->mIndices = byte_alloc( src->mIndexCount * sizeof( u32 ) );
            memcpy( dest->mIndices, src->mIndices, src->mIndexCount * sizeof( u32 ) );
        }

        dest->mTexcoordBegin = src->mTexcoordBegin;
        dest->mUseIndices = src->mUseIndices;
    }
}

void Mesh_resize( Mesh *pMesh, const vec2 *pSize ) {
    if( pMesh && pSize ) {
        for( u32 i = 0; i < pMesh->mVertexCount; ++i ) {
            pMesh->mData[2*i] *= pSize->x;
            pMesh->mData[2*i+1] *= pSize->y;
        }
    }
}

bool Mesh_addVertexData( Mesh *pMesh, vec2 *pPositions, u32 pPositionSize, vec2 *pTexcoords, u32 pTexcoordSize ) {
    check( pPositions, "In Mesh_addMeshData : given Position array is NULL!\n" );
    check( pTexcoords, "In Mesh_addMeshData : given Texcoords array is NULL!\n" );

    if( pMesh ) {
        pMesh->mVertexCount = pPositionSize / sizeof( vec2 );
        u32 sizeInFloat = pMesh->mVertexCount * 2;

        if( pMesh->mData ) {
            log_info( "In Mesh_addMeshData : Vertex data was already existing!\n" );
            DEL_PTR( pMesh->mData );
        }

        pMesh->mData = byte_alloc( sizeInFloat * 2 * sizeof( f32 ) );
        
        // Copy data from two arrays of vec2 into mesh data array of f32
        memcpy( pMesh->mData, pPositions, sizeInFloat * sizeof( f32 ) );
        memcpy( pMesh->mData + (size_t)sizeInFloat, pTexcoords, sizeInFloat * sizeof( f32 ) );

        pMesh->mTexcoordBegin = sizeInFloat * sizeof( f32 );

        return true;
    }

error:
    return false;
}

bool Mesh_addIndexData( Mesh *pMesh, u32 *pIndices, u32 pIndexSize ) {
    check( pIndices, "In Mesh_addMeshIndices : given Indices array is NULL!\n" );
    
    if( pMesh ) {
        pMesh->mIndexCount = pIndexSize / sizeof( u32 );

        if( pMesh->mIndices ) {
            log_info( "In Mesh_addMeshIndices : Index data was already existing!\n" );
            DEL_PTR( pMesh->mIndices );
        }

        pMesh->mIndices = byte_alloc( pIndexSize );
        memcpy( pMesh->mIndices, pIndices, pIndexSize );

        return true;
    }
error :
    return false;
}

void Mesh_build( Mesh *pMesh, bool pDynamic ) {
    check( pMesh->mData, "In Mesh_build, the given mesh is not complete. Please supply at least Vertex and Index Data before building the mesh!\n" );

    if( pMesh->mVbo ) {
        glDeleteBuffers( 1, &pMesh->mVbo );
        pMesh->mVbo = 0;
    }
    if( pMesh->mIbo ) {
        glDeleteBuffers( 1, &pMesh->mIbo );
        pMesh->mIbo = 0;
    }

    glGenBuffers( 1, &pMesh->mVbo );
    glBindBuffer( GL_ARRAY_BUFFER, pMesh->mVbo );
    glBufferData( GL_ARRAY_BUFFER, pMesh->mVertexCount * 4 * sizeof( f32 ), pMesh->mData, pDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    if( pMesh->mIndices ) {
        glGenBuffers( 1, &pMesh->mIbo );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, pMesh->mIbo );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, pMesh->mIndexCount * sizeof( u32 ), pMesh->mIndices, pDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
        pMesh->mUseIndices = true;
    } else {
        pMesh->mUseIndices = false;
    }

error:
    return;
}


void Mesh_bind( Mesh *pMesh ) {
    glBindBuffer( GL_ARRAY_BUFFER, pMesh->mVbo );
    glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, 0 );
    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(pMesh->mTexcoordBegin) );

    if( pMesh->mUseIndices )
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, pMesh->mIbo );
}

void Mesh_unbind() {
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}
