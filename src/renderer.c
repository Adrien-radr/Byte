#include "renderer.h"
#include "mesh.h"
#include "texture.h"

#include "GL/glew.h"


// Array VaoArray with u32 data type
SimpleArray( u32, Vao )

// Array MeshArray with Mesh* data type
HeapArray( Mesh*, Mesh, Mesh_destroy )

// Array ShaderArray with Shader* data type
HeapArray( Shader*, Shader, Shader_destroy )

// Array TextureArray with Texture* data type
HeapArray( Texture*, Texture, Texture_destroy )

// Array FontArray with Font* data type
HeapArray( Font*, Font, Font_destroy )

/// Application Renderer
struct s_Renderer {
    VaoArray        mVaos;
    MeshArray       mMeshes;
    ShaderArray     mShaders;
    TextureArray    mTextures;
    FontArray       mFonts;

    int             mCurrentMesh,
                    mCurrentShader,
                    mCurrentTexture,
                    mCurrentTextureTarget;
};

/// Renderer only instance definition
Renderer *renderer = NULL;

bool Renderer_init() {
    check( !renderer, "Renderer already created!\n" );

    renderer = byte_alloc( sizeof( Renderer ) );
    check_mem( renderer );
    
    // initial number of 10 vaos and 100 meshes
    VaoArray_init( &renderer->mVaos, 10 );
    MeshArray_init( &renderer->mMeshes, 10 );
    ShaderArray_init( &renderer->mShaders, 10 );
    TextureArray_init( &renderer->mTextures, 10 );
    FontArray_init( &renderer->mFonts, 10 );

    renderer->mCurrentMesh = -1;
    renderer->mCurrentShader = -1;
    renderer->mCurrentTexture = -1;
    renderer->mCurrentTextureTarget = 0;
 
    // GLEW initialisation
    glewExperimental = 1;
    GLenum glerr = glewInit();

    // remove GLEW error cause of Core profile
    glGetError();

    check( GLEW_OK == glerr, "GLEW Error : %s\n", glewGetErrorString( glerr ) );

    log_info( "GLEW v%s successfully initialized!\n", glewGetString( GLEW_VERSION ) );

    // GL Version 
    int majV, minV;
    glGetIntegerv( GL_MAJOR_VERSION, &majV );
    glGetIntegerv( GL_MINOR_VERSION, &minV );
    log_info( "OpenGL %d.%d\n", majV, minV );
    log_info( "GLSL %s\n", glGetString( GL_SHADING_LANGUAGE_VERSION ) );
    log_info( "Hardware : %s - %s\n", glGetString( GL_VENDOR ), glGetString( GL_RENDERER ) );

    // GL state initialisations
    //glHint( GL_GENERATE_MIPMAP_HINT, GL_NICEST );

    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LESS );

    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
    glFrontFace( GL_CCW );

    // enable alpha blend
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor( 0.2f, 0.2f, 0.2f, 1.f );

    // clear init gl errors
    CheckGLError();
 
    log_info( "Renderer successfully initialized!\n" );

    return true;

error:
    Renderer_destroy();

    return false;
}

void Renderer_destroy() {
    if( renderer ) {
        VaoArray_destroy( &renderer->mVaos );
        MeshArray_destroy( &renderer->mMeshes );
        TextureArray_destroy( &renderer->mTextures );
        ShaderArray_destroy( &renderer->mShaders );
        FontArray_destroy( &renderer->mFonts );
        DEL_PTR( renderer );
    }
}

void Renderer_beginFrame() {
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void Renderer_updateProjectionMatrix( const mat3 *pm ) {
    if( renderer ) {
        u32 currShader = renderer->mCurrentShader;

        // update every shader using projection matrix
        for( u32 i = 0; renderer && (i < renderer->mShaders.cpt); ++i ) 
            if( renderer->mShaders.data[i]->mUseProjectionMatrix ) {
                Renderer_useShader( i );
                Shader_sendMat3( "ProjectionMatrix", pm );
            }

        // returns to shader used before update
        Renderer_useShader( currShader );
    }
}

bool Renderer_isInitialized() {
    return ( NULL != renderer );
}

int Renderer_beginVao() {
    if( renderer ) {
        // check if Vao array is not already full
        if( VaoArray_checkSize( &renderer->mVaos ) ) {
            // create and bind a new VAO
            glGenVertexArrays( 1, &renderer->mVaos.data[renderer->mVaos.cpt] );
            glBindVertexArray( renderer->mVaos.data[renderer->mVaos.cpt] );


            return renderer->mVaos.cpt++;
        }
    }   
    return -1;
}

void Renderer_endVao() {
    if( renderer ) 
        glBindVertexArray( 0 );
}

void Renderer_bindVao( u32 pIndex ) {
    if( renderer )
        glBindVertexArray( renderer->mVaos.data[pIndex] );
}



int  Renderer_createStaticMesh( u32 *pIndices, u32 pIndiceSize, vec2 *pPositions, u32 pPositionSize, vec2 *pTexcoords, u32 pTexcoordSize ) {
    Mesh *m = NULL;
    if( renderer ) {
        if( MeshArray_checkSize( &renderer->mMeshes ) ) {
            m = Mesh_new();
            check_mem( m );

            // add Vertex Data
            check( Mesh_addVertexData( m, pPositions, pPositionSize, pTexcoords, pTexcoordSize ), "Error in mesh creation when setting Vertex Data !\n" );

            // add Index data
            check( Mesh_addIndexData( m, pIndices, pIndiceSize ), "Error in mesh creation when setting Index Data !\n" );

            // build Mesh VBO
            Mesh_build( m, false );


            // add the Mesh to the renderer array
            int index = renderer->mMeshes.cpt++;
            renderer->mMeshes.data[index] = m;

            return index;

        } else
            log_err( "In Renderer_createStaticMesh : Error with MeshArray expansion!\n" );
    }
error:
    Mesh_destroy( m );
    return -1;
}

int  Renderer_createDynamicMesh() {
    Mesh *m = NULL;
    if( renderer ) {
        if( MeshArray_checkSize( &renderer->mMeshes ) ) {
            m = Mesh_new();
            check_mem( m );

            // storage
            int index = renderer->mMeshes.cpt++;
            renderer->mMeshes.data[index] = m;

            return index;
        } else
            log_err( "In Renderer_createDynamicMesh : Error with MeshArray expansion!\n" );
    }
error:
    Mesh_destroy( m );
    return -1;
}

bool Renderer_setDynamicMeshData( u32 pMesh, f32 *pVData, u32 pVSize, u32 *pIData, u32 pISize ) {
    if( renderer ) {
        Mesh *m = renderer->mMeshes.data[pMesh];
        check( m, "Wanted to change Dynamic Mesh Data of an unexisting mesh (handle = %d)\n", pMesh );

        // Change vertex data if given
        if( pVData && pVSize > 0 ) {
            // delete previous data if it exists
            if( m->mData )
                DEL_PTR( m->mData );

            // allocate new space for data, and copy array content
            m->mData = byte_alloc( pVSize );
            memcpy( m->mData, pVData, pVSize );

            m->mTexcoordBegin = pVSize / 2;
            // vertex count = 4 floats(px, py, tx, ty) by vertex
            m->mVertexCount = pVSize / (sizeof( u32 ) * 4);
        }

        // Change index data if given
        if( pIData && pISize > 0 ) {
            // delete previous data if it exists
            if( m->mIndices )
                DEL_PTR( m->mIndices );

            // allocate new space for data, and copy array content
            m->mIndices = byte_alloc( pISize );
            memcpy( m->mIndices, pIData, pISize );

            m->mIndexCount = pISize / sizeof( u32 );
        } 

        // if any change, rebuild mesh
        if( pVSize > 0 || pISize > 0 )
            Mesh_build( m, true );

        return true;
    }
error:
    return false;
}

void Renderer_renderMesh( u32 pIndex ) {
    if( renderer && pIndex < renderer->mMeshes.cpt ) {
        Mesh *m = renderer->mMeshes.data[pIndex];

        // bind mesh only if it isnt already
        if( renderer->mCurrentMesh != pIndex ) {
            Mesh_bind( m );
            renderer->mCurrentMesh = pIndex;
        }

        if( m->mUseIndices )
            glDrawElements( GL_TRIANGLES, m->mIndexCount, GL_UNSIGNED_INT, 0 );
        else
            glDrawArrays( GL_TRIANGLES, 0, m->mVertexCount );
    }
}

int  Renderer_createShader( const char *pVFile, const char *pFFile ) {
    Shader *s = NULL;

    if( renderer && ShaderArray_checkSize( &renderer->mShaders ) ) {
        s = Shader_new();
        check_mem( s );

        // shader creation and linking
        check( Shader_buildFromFile( s, pVFile, pFFile ), "Error in shader creation.\n" );

        // storage
        int index = renderer->mShaders.cpt++;
        renderer->mShaders.data[index] = s;

        return index;
    }

error:
    Shader_destroy( s );
    return -1;
}

void Renderer_useShader( int pShader ) { 
    if( renderer && pShader < renderer->mShaders.cpt && pShader != renderer->mCurrentShader ) {
        renderer->mCurrentShader = pShader;
        Shader_bind( pShader < 0 ? 0 : renderer->mShaders.data[pShader] );
    }
}

Shader *Renderer_getShader( u32 pShader ) {
    if( renderer && pShader < renderer->mShaders.cpt )
        return renderer->mShaders.data[pShader];
    return NULL;
}

int  Renderer_currentShader() {
    if( renderer ) 
        return renderer->mCurrentShader;
    return -1;
}

u32  Renderer_currentGLProgram() {
    if( renderer )
        return renderer->mShaders.data[renderer->mCurrentShader]->mID;
    return 0;
}


int  Renderer_createTexture( const char *pTFile, bool pMipmaps ) {
    Texture *t = NULL;

    if( renderer && pTFile ) {
        t = Texture_new();
        check_mem( t );

        // texture creation from file
        check( Texture_loadFromFile( t, pTFile, pMipmaps ), "Error in texture creation.\n" );

        // storage
        int index = renderer->mTextures.cpt++;
        renderer->mTextures.data[index] = t;

        return index;
    }

error:
    Texture_destroy( t );
    return -1;
}

int  Renderer_allocateEmptyTexture( Texture **t ) {
    check( NULL == *t, "Renderer_allocateEmptyTexture needs a NULL texture as a parameter!\n" );
    if( renderer ) {
        *t = Texture_new();
        check_mem( *t );

        //storage
        int index = renderer->mTextures.cpt++;
        renderer->mTextures.data[index] = *t;

        return index;
    }
error:
    return -1;
}

void Renderer_useTexture( int pTexture, u32 pTarget ) {
    if( renderer && pTexture < renderer->mTextures.cpt && pTexture != renderer->mCurrentTexture ) {
        renderer->mCurrentTexture = pTexture;

        int target = -1;
        if( pTarget != renderer->mCurrentTextureTarget ) 
            renderer->mCurrentTextureTarget = target = pTarget;

        Texture_bind( pTexture < 0 ? 0 : renderer->mTextures.data[pTexture], target );
    }
}

int  Renderer_createFont( const char *pFile, u32 pSize ) {
    Font *f = NULL;

    if( pFile && renderer && pSize > 0 ) {
        f = Font_new();
        check_mem( f );

        check( Font_createAtlas( f, pFile, pSize ), "Error in font creation!\n" );

        // storage
        int index = renderer->mFonts.cpt++;
        renderer->mFonts.data[index] = f;

        return index;
    }
error:
    Font_destroy( f );
    return -1;
}

Font *Renderer_getFont( u32 pHandle ) {
    if( renderer )
        return renderer->mFonts.data[pHandle];
    return NULL;
}

void CheckGLError_func( const char *pFile, u32 pLine ) {
#ifdef _DEBUG
    GLenum error = glGetError();

    if (error != GL_NO_ERROR) {
        str64 errorStr;
        str256 description;

        switch (error)
        {
            case GL_INVALID_ENUM :
            {
                strncpy( errorStr, "GL_INVALID_ENUM", 64 );
                strncpy( description, "An unacceptable value has been specified for an enumerated argument.", 256 );
                break;
            }

            case GL_INVALID_VALUE :
            {
                strncpy( errorStr, "GL_INVALID_VALUE", 64 );
                strncpy( description, "A numeric argument is out of range.", 256 );
                break;
            }

            case GL_INVALID_OPERATION :
            {
                strncpy( errorStr, "GL_INVALID_OPERATION", 64 );
                strncpy( description, "The specified operation is not allowed in the current state.", 256 );
                break;
            }

            case GL_STACK_OVERFLOW :
            {
                strncpy( errorStr, "GL_STACK_OVERFLOW", 64 );
                strncpy( description, "This command would cause a stack overflow.", 256 );
                break;
            }

            case GL_STACK_UNDERFLOW :
            {
                strncpy( errorStr, "GL_STACK_UNDERFLOW", 64 );
                strncpy( description, "This command would cause a stack underflow.", 256 );
                break;
            }

            case GL_OUT_OF_MEMORY :
            {
                strncpy( errorStr, "GL_OUT_OF_MEMORY", 64 );
                strncpy( description, "There is not enough memory left to execute the command.", 256 );
                break;
            }

            case GL_INVALID_FRAMEBUFFER_OPERATION :
            {
                strncpy( errorStr, "GL_INVALID_FRAMEBUFFER_OPERATION_EXT", 64 );
                strncpy( description, "The object bound to FRAMEBUFFER_BINDING is not \"framebuffer complete\".", 256 );
                break;
            }

            default : 
            {
                strncpy( errorStr, "UNKNOWN", 64 );
                strncpy( description, "Unknown GL Error", 256 );
                break;
            }
        }

        log_err( "OpenGL Error (%s [%d])\n"
                 "-- Error          : %s\n"
                 "-- Description    : %s\n", pFile, pLine, errorStr, description );
    }
#endif
}
