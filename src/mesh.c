#include "mesh.h"
#include "GL/glew.h"

Mesh* Mesh_new() {
    Mesh *mesh = byte_alloc( sizeof( Mesh ) );

    return mesh;
}

void Mesh_destroy( Mesh *pMesh ) {
    if( pMesh ) {
        glDeleteBuffers( 1, &pMesh->mVbo[0] );
        glDeleteBuffers( 1, &pMesh->mIbo );
        DEL_PTR( pMesh->mData );
        DEL_PTR( pMesh->mIndices );
        DEL_PTR( pMesh );
    }
}

void Mesh_addVbo( Mesh *pMesh, MeshAttrib pIndex, vec2 *pArray, u32 pArraySize ) {
    if( !pArray ) {
        log_err( "In Mesh_addVbo : given Data array is NULL!\n" );
        return;
    }

    if( pMesh ) {
        u32 ma = (u32)pIndex;
        
        // If the given array is position data and we already have some, destroy the previous one
        if( 0 == ma ) {
            if( pMesh->mData ) 
                DEL_PTR( pMesh->mData );

            // transform vec2 data into float data and store it
            pMesh->mVertexCount = pArraySize / sizeof( vec2 );
            u32 sizeInFloat = pMesh->mVertexCount * 2;

            pMesh->mData = byte_alloc( sizeInFloat * sizeof( f32 ) ); 

            for( int i = 0; i < pMesh->mVertexCount; ++i ) {
                pMesh->mData[i * 2] = pArray[i].x;
                pMesh->mData[i * 2 + 1] = pArray[i].y;
            }


            if( pMesh->mVbo[0] )
                glDeleteBuffers( 1, &pMesh->mVbo[0] );

            glGenBuffers( 1, &pMesh->mVbo[0] );
            glBindBuffer( GL_ARRAY_BUFFER, pMesh->mVbo[0] );
                glBufferData( GL_ARRAY_BUFFER, sizeInFloat * sizeof( f32 ), pMesh->mData, GL_STATIC_DRAW );
            glBindBuffer( GL_ARRAY_BUFFER, 0 );
        } 
    }
}

void Mesh_addIbo( Mesh *pMesh, u32 *pArray, u32 pArraySize ) {
    if( !pArray ) {
        log_err( "In Mesh_addIbo : given Index array is NULL!\n" );
        return;
    }

    if( pMesh ) {
        if( pMesh->mIndices ) 
            DEL_PTR( pMesh->mIndices );

        pMesh->mIndexCount = pArraySize / sizeof( u32 );

        // allocate the mesh indice array
        pMesh->mIndices = byte_alloc( pArraySize );
        memcpy( pMesh->mIndices, pArray, pArraySize );

        if( pMesh->mIbo )
            glDeleteBuffers( 1, &pMesh->mIbo );

        glGenBuffers( 1, &pMesh->mIbo );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, pMesh->mIbo );
            glBufferData( GL_ELEMENT_ARRAY_BUFFER, pArraySize, pMesh->mIndices, GL_STATIC_DRAW );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    }

}

void Mesh_bind( Mesh *pMesh ) {
    glBindBuffer( GL_ARRAY_BUFFER, pMesh->mVbo[0] );
    glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, pMesh->mIbo );
}

void Mesh_unbind() {
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}
