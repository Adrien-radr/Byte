#include "mesh.h"
#include "GL/glew.h"

Mesh* Mesh_new() {
    Mesh* mesh = malloc( sizeof( Mesh ) );

    mesh->mVbo[0] = 0;
    mesh->mIbo = 0;
    mesh->mIndexCount = 0;
    mesh->mVertexCount = 0;
    mesh->mData = NULL;
    mesh->mIndices = NULL;

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
    if( pMesh ) {
        u32 ma = (u32)pIndex;
        
        // If the given array is position data and we already have some, destroy the previous one
        if( 0 == ma ) {
            if( pMesh->mData ) 
                DEL_PTR( pMesh->mData );

            pMesh->mVertexCount = pArraySize / sizeof( vec2 );
            u32 sizeInFloat = pMesh->mVertexCount * 2;

            // transform vec2 data into float data and store it
            pMesh->mData = calloc( sizeInFloat, sizeof( f32 ) ); 

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
    if( pMesh ) {
        if( pMesh->mIndices ) 
            DEL_PTR( pMesh->mIndices );

        pMesh->mIndices = pArray;
        pMesh->mIndexCount = pArraySize / sizeof( u32 );

        if( pMesh->mIbo )
            glDeleteBuffers( 1, &pMesh->mIbo );

        glGenBuffers( 1, &pMesh->mIbo );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, pMesh->mIbo );
            glBufferData( GL_ELEMENT_ARRAY_BUFFER, pArraySize, pArray, GL_STATIC_DRAW );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    }
}

void Mesh_bind( Mesh *pMesh ) {
    glBindBuffer( GL_ARRAY_BUFFER, pMesh->mVbo[0] );
    glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, 0 );
}

void Mesh_unbind() {
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}
