#include "renderer.h"
#include "texture.h"

#include "GL/glew.h"

// Array MeshArray with Mesh* data type
HeapArray( Mesh*, Mesh, Mesh_destroy )

// Array ShaderArray with Shader* data type
HeapArray( Shader*, Shader, Shader_destroy )

// Array TextureArray with Texture* data type
HeapArray( Texture*, Texture, Texture_destroy )

// Array FontArray with Font* data type
HeapArray( Font*, Font, Font_destroy )

// Array SpriteDataArray with Sprite data type
SimpleArray( Sprite, SpriteData )

/// Application Renderer
struct s_Renderer {
    int             mVao;                   ///< Global VAO used to render everything

    // These are array of the actual data used throughout the game
    // All the data in these arrays is deleted/free'd at the end (Renderer_destroy()).
    MeshArray       mMeshes;                ///< Array of created meshes
    ShaderArray     mShaders;               ///< Array of loaded GL shader programs
    TextureArray    mTextures;              ///< Array of GL textures
    FontArray       mFonts;                 ///< Array of Font (loaded with freetype2 to GL Texs)

    SpriteDataArray     sprites;

    // these are state variables. -1 mean nothing is currently used
    int             mCurrentMesh,           ///< The currently bound OpenGL VBO
                    mCurrentShader,         ///< The currently bound OpenGL Shader Program
                    mCurrentTexture[2],     ///< The currently bound OpenGL Textures on respective target
                    mCurrentTextureTarget;  ///< The current OpenGL Texture targer

    //  Projection matrices for the game and for the widgets.
    mat3 projection_matrix_game;
    mat3 projection_matrix_ui;
};

/// Renderer only instance definition
Renderer *renderer = NULL;

bool Renderer_init() {
    check( !renderer, "Renderer already created!\n" );

    renderer = byte_alloc( sizeof( Renderer ) );
    check_mem( renderer );

    // initial number of 10 vaos and 100 meshes
    MeshArray_init( &renderer->mMeshes, 10 );
    ShaderArray_init( &renderer->mShaders, 10 );
    TextureArray_init( &renderer->mTextures, 10 );
    FontArray_init( &renderer->mFonts, 10 );
    SpriteDataArray_init( &renderer->sprites, 50 );

    renderer->mCurrentMesh = -1;
    renderer->mCurrentShader = -1;
    renderer->mCurrentTexture[0] = -1;
    renderer->mCurrentTexture[1] = -1;
    renderer->mCurrentTextureTarget = 0;

    renderer->mVao = -1;
 
    // GLEW/GLDL initialisation
#ifdef USE_GLDL
    int noerr = gldlInit();

    check( noerr, "Failed to initialize GLDL!\n" );
    log_info( "GLDL successfully initialized!\n" );
#else
    glewExperimental = 1;
    GLenum glerr = glewInit();

    // remove GLEW error cause of Core profile
    glGetError();

    check( GLEW_OK == glerr, "GLEW Error : %s\n", glewGetErrorString( glerr ) );
    log_info( "GLEW v%s successfully initialized!\n", glewGetString( GLEW_VERSION ) );
#endif

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
    glDepthFunc( GL_LEQUAL );

    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
    glFrontFace( GL_CCW );

    // enable alpha blend
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glClearColor( 0.f, 0.f, 0.f, 0.f );

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
        MeshArray_destroy( &renderer->mMeshes );
        TextureArray_destroy( &renderer->mTextures );
        ShaderArray_destroy( &renderer->mShaders );
        FontArray_destroy( &renderer->mFonts );
        SpriteDataArray_destroy( &renderer->sprites );
        DEL_PTR( renderer );
    }
}

void Renderer_beginFrame() {
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void Renderer_updateProjectionMatrix( ProjMatrixType t, const mat3 *pm ) {
    if( renderer ) {
        u32 currShader = renderer->mCurrentShader;

        // update every shader using projection matrix
        for( u32 i = 0; renderer && (i < renderer->mShaders.cpt); ++i ) 
            if( renderer->mShaders.data[i]->proj_matrix_type == t ) {
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

void Renderer_initVao() {
    if( renderer && renderer->mVao < 0 ) {
        // gen a VAO ID
        GLuint vao;
        glGenVertexArrays( 1, &vao );

        // place it in renderer and bind
        renderer->mVao = vao;

        // enable position & texcoords
        glBindVertexArray( renderer->mVao );
    }
}

int  Renderer_createStaticMesh( PrimitiveType p, u32 *pIndices, u32 pIndiceSize, vec2 *pPositions, u32 pPositionSize, vec2 *pTexcoords, u32 pTexcoordSize ) {
    Mesh *m = NULL;
    if( renderer ) {
        if( MeshArray_checkSize( &renderer->mMeshes ) ) {
            m = Mesh_new( GL_TRIANGLES );
            check_mem( m );

            MeshConstruction method = ERebuildVbo;

            // add Vertex Data
            check( Mesh_addVertexData( m, (f32*)pPositions, pPositionSize, (f32*)pTexcoords, pTexcoordSize ), "Error in mesh creation when setting Vertex Data !\n" );

            // add Index data if given
            if( pIndices ) {
                check( Mesh_addIndexData( m, pIndices, pIndiceSize ), "Error in mesh creation when setting Index Data !\n" );
                method |= ERebuildIbo;
            }

            // build Mesh VBO
            Mesh_build( m, method, false );


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

int  Renderer_createRescaledMesh( u32 mesh, const vec2 *pos_scale, const vec2 *texcoords_scale ) {
    Mesh *m = NULL;
    if( renderer && mesh < renderer->mMeshes.cpt ) {
        if( MeshArray_checkSize( &renderer->mMeshes ) ) {
            m = Mesh_new( GL_TRIANGLES );
            check_mem( m );
    
            // copy given mesh and rescale it
            Mesh_cpy( m, renderer->mMeshes.data[mesh] );
            Mesh_resize( m, pos_scale, texcoords_scale );

            // build mesh VBO
            Mesh_build( m, ERebuildVbo | (m->use_indices ? ERebuildIbo : 0), false );

            // storage
            int index = renderer->mMeshes.cpt++;
            renderer->mMeshes.data[index] = m;

            return index;

        } else
            log_err( "In Renderer_createRescaledMesh : Error with MeshArray expansion!\n" );
    }
error:
    Mesh_destroy( m );
    return -1;
}

int  Renderer_createDynamicMesh( u32 mode ) {
    Mesh *m = NULL;
    if( renderer ) {
        if( MeshArray_checkSize( &renderer->mMeshes ) ) {
            m = Mesh_new( mode );
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

bool Renderer_setDynamicMeshData( u32 pMesh, f32 *positions, u32 positions_size, f32 *texcoords, u32 texcoords_size, u32 *indices, u32 indices_size ) {
    if( renderer ) {
        Mesh *m = renderer->mMeshes.data[pMesh];
        check( m, "Wanted to change Dynamic Mesh Data of an unexisting mesh (handle = %d)\n", pMesh );

        MeshConstruction method = 0;

        // Change vertex data if given
        if( positions && positions_size > 0 && texcoords && texcoords_size > 0 ) {
            bool scaled = Mesh_addVertexData( m, positions, positions_size, texcoords, texcoords_size );
            method |= (scaled ? ERebuildVbo : EUpdateVbo);
        }

        // Change index data if given
        if( indices && indices_size > 0 ) {
            bool scaled = Mesh_addIndexData( m, indices, indices_size );
            method |= (scaled ? ERebuildIbo : EUpdateIbo);
        } 
 
        // if any change, rebuild mesh
        if( method ) 
            Mesh_build( m, method, true );

        return true;
    }
error:
    return false;
}

bool Renderer_setDynamicMeshDataBlock( u32 pMesh, f32 *data, u32 data_size, u32 *indices, u32 indices_size ) {
    if( renderer ) {
        Mesh *m = renderer->mMeshes.data[pMesh];
        check( m, "Wanted to change Dynamic Mesh Data of an unexisting mesh (handle = %d)\n", pMesh );

        MeshConstruction method = 0;

        // Change vertex data if given
        if( data && data_size > 0 ) {
            bool scaled = Mesh_addVertexDataBlock( m, data, data_size );
            method |= (scaled ? ERebuildVbo : EUpdateVbo);
        }

        // Change index data if given
        if( indices && indices_size > 0 ) {
            bool scaled = Mesh_addIndexData( m, indices, indices_size );
            method |= (scaled ? ERebuildIbo : EUpdateIbo);
        } 
 
        // if any change, rebuild mesh
        if( method ) 
            Mesh_build( m, method, true );

        return true;
    }
error:
    return false;
}

Mesh *Renderer_getMesh( u32 pMesh ) {
    if( renderer && pMesh < renderer->mMeshes.cpt )
        return renderer->mMeshes.data[pMesh];
    return NULL;
}

void Renderer_renderMesh( u32 pIndex ) {
    if( renderer && pIndex < renderer->mMeshes.cpt ) {
        Mesh *m = renderer->mMeshes.data[pIndex];

        // bind mesh only if it isnt already
        if( renderer->mCurrentMesh != pIndex ) {
            Mesh_bind( m );
            renderer->mCurrentMesh = pIndex;
        }

        if( m->use_indices )
            glDrawElements( m->mode, m->index_count, GL_UNSIGNED_INT, 0 );
        else
            glDrawArrays( m->mode, 0, m->vertex_count );
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

inline u32  Renderer_currentGLProgram() {
    if( renderer )
        return renderer->mShaders.data[renderer->mCurrentShader]->mID;
    return 0;
}


int  Renderer_createTexture( const char *pTFile, bool pMipmaps ) {
    Texture *t = NULL;

    if( renderer && TextureArray_checkSize( &renderer->mTextures ) ) {
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

    if( renderer && TextureArray_checkSize( &renderer->mTextures ) ) {
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

inline void Renderer_useTexture( int pTexture, u32 pTarget ) {
    if( renderer && pTexture < renderer->mTextures.cpt && pTexture != renderer->mCurrentTexture[pTarget] ) {
        renderer->mCurrentTexture[pTarget] = pTexture;

        // will send -1 to Texture_bind if no need to change target
        int target = -1;
        if( pTarget != renderer->mCurrentTextureTarget )
            renderer->mCurrentTextureTarget = target = pTarget;

        Texture_bind( pTexture < 0 ? NULL : renderer->mTextures.data[pTexture], target );
    }
}

inline Texture *Renderer_getTexture( u32 tex ) {
    if( renderer && tex < renderer->mTextures.cpt )
        return renderer->mTextures.data[tex];
    return NULL;
}

int  Renderer_createFont( const char *pFile, u32 pSize ) {
    Font *f = NULL;

    if( renderer && pSize > 0 && FontArray_checkSize( &renderer->mFonts ) ) {
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
    if( renderer && pHandle < renderer->mFonts.cpt )
        return renderer->mFonts.data[pHandle];
    return NULL;
}

int  Renderer_createSprite( const char *file ) {
    if( renderer && SpriteDataArray_checkSize( &renderer->sprites ) ) {
        Sprite s;
        bool loaded = Sprite_load( &s, file );

        if( loaded ) {
            int index = renderer->sprites.cpt++;
            Sprite_cpy( &renderer->sprites.data[index], &s );

            return index;
        }
    }

    return -1;
}

Sprite *Renderer_getSprite( u32 handle ) {
    if( renderer && handle < renderer->sprites.cpt )
        return &renderer->sprites.data[handle];
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
