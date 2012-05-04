#include "scene.h"
#include "renderer.h"
#include "camera.h"
#include "context.h"
#include "resource.h"

#include "game.h"

#ifdef USEGLDL
#include "GL/gldl.h"
#else
#include "GL/glew.h"
#endif

// #############################################################################
//          SCENE MAP

/// Local function. Initialize the scene map
void SceneMap_init( Scene *scene ) {
    // buffer positions ( 4 vertices for each map tile )
    vec2 map_pos[4*LOCAL_MAP_WIDTH*LOCAL_MAP_HEIGHT];
    // buffer tex coordinates ( 4 for each tile )
    vec2 map_tcs[4*LOCAL_MAP_WIDTH*LOCAL_MAP_HEIGHT];
    // drawing indices ( 2 triangles(3verts) for each tile )
    u32  map_indices[6*LOCAL_MAP_WIDTH*LOCAL_MAP_HEIGHT];

    // vertex position offsets
    int x_offset, y_offset;  

    // arrays indices offsets ( xi and yj for pos and tcs offsets,
    int xi, yj, ii, ij;      // ii and ij for indices               )
     

    // create map mesh data
    for( int j = 0; j < LOCAL_MAP_HEIGHT/2; ++j )
        for( int i = 0; i < 2*LOCAL_MAP_WIDTH; ++i ) {
            x_offset = i * 50.f;
            y_offset = j * 50.f + (i&1) * 25.f;

            // i * 4 : 4 vertices for each tile
            xi = i * 4;
            // j * 2 : each row index is a col of 2 tiles 
            // LOCAL_MAP_WIDTH * 4 : 4 vertices for a complete row of tiles
            yj = j * 2 * LOCAL_MAP_WIDTH * 4;
            map_pos[yj+xi+0].x = x_offset + 50.f;
            map_pos[yj+xi+0].y = y_offset + 50.f;
            map_pos[yj+xi+1].x = x_offset;
            map_pos[yj+xi+1].y = y_offset + 25.f;
            map_pos[yj+xi+2].x = x_offset + 50.f;
            map_pos[yj+xi+2].y = y_offset;
            map_pos[yj+xi+3].x = x_offset + 100.f;
            map_pos[yj+xi+3].y = y_offset + 25.f; 

            map_tcs[yj+xi+0].x = 0.f;     map_tcs[yj+xi+0].y = 0.f;
            map_tcs[yj+xi+1].x = 0.f;     map_tcs[yj+xi+1].y = 1.f;
            map_tcs[yj+xi+2].x = 0.5f;     map_tcs[yj+xi+2].y = 1.f;
            map_tcs[yj+xi+3].x = 0.5f;     map_tcs[yj+xi+3].y = 0.f;

            ii = i * 6;
            ij = j * 2 * LOCAL_MAP_WIDTH * 6;
            map_indices[ij+ii+0] = yj+xi+0;
            map_indices[ij+ii+1] = yj+xi+2;
            map_indices[ij+ii+2] = yj+xi+1;
            map_indices[ij+ii+3] = yj+xi+0;
            map_indices[ij+ii+4] = yj+xi+3;
            map_indices[ij+ii+5] = yj+xi+2;
        }

    // create mesh
    scene->local_map.mesh = Renderer_createDynamicMesh( GL_TRIANGLES );
    Renderer_setDynamicMeshData( scene->local_map.mesh, (f32*)map_pos, sizeof(map_pos), (f32*)map_tcs, sizeof(map_tcs), map_indices, sizeof(map_indices) );

 
    // get shader and texture
    scene->local_map.texture = ResourceManager_get( "map.png" );
    scene->local_map.shader = ResourceManager_get( "map_shader.json" );
}

void SceneMap_redTile( Scene *scene, u32 i, u32 j ) {
    if( i >= LOCAL_MAP_WIDTH*2 || j >= LOCAL_MAP_HEIGHT/2 ) 
        return;

    Mesh *m = Renderer_getMesh( scene->local_map.mesh );
    u32 tcs_offset = m->vertex_count * 2;
    int i_offset = i * 8,
        j_offset = j * 2 * LOCAL_MAP_WIDTH * 8;
 

    // we change only on even indices, only the X coord, not Y
    m->data[tcs_offset+i_offset+j_offset+0] = 0.5f;
    m->data[tcs_offset+i_offset+j_offset+2] = 0.5f;
    m->data[tcs_offset+i_offset+j_offset+4] = 1.f;
    m->data[tcs_offset+i_offset+j_offset+6] = 1.f;

    // rebuild map mesh
    Mesh_build( m, EUpdateVbo, false );
}

vec2 Scene_localToGlobal( Scene *scene, const vec2i *local ) {
    vec2 ret = { local->x, local->y};
    ret = vec2_mul( &ret, scene->camera->mZoom );
    ret = vec2_add( &ret, &scene->camera->global_position );

    return ret;
}

vec2i Scene_screenToIso( Scene *scene, const vec2i *local ) {
    static vec2i up =    {  50,  0 };
    static vec2i down =  {  50, 50 };
    static vec2i left =  {   0, 25 };
    static vec2i right = { 100, 25 };
    static vec2i tilesize = { 100, 50 };

    // get global mouse position (not depending on camera zoom or pan)
    vec2 global = Scene_localToGlobal( scene, local );

    vec2i ret, offset;
    ret.x = 2 * (int)( global.x / tilesize.x);
    ret.y = global.y / tilesize.y;
    offset.x = (int)global.x % tilesize.x;
    offset.y = (int)global.y % tilesize.y;


    if( PointOnLinei( &offset, &left, &up ) < 0 ) {
        ret.y -= 1;      
        ret.x -= 1;
    } 
    else if( PointOnLinei( &offset, &left, &down ) > 0 ) 
        ret.x -= 1;
    else if( PointOnLinei( &offset, &up, &right ) < 0 ) {
        ret.y -= 1;     
        ret.x += 1;
    } else if( PointOnLinei( &offset, &down, &right ) > 0 )  
        ret.x += 1;

    return ret;
}


// #############################################################################
//          SCENE

// Camera listeners and update function
    void cameraMouseListener( const Event *event, void *camera ) {
        Camera *cam = (Camera*)camera;
        // manage zoom event 
        if( event->type == EMouseWheelMoved ) 
            Camera_zoom( cam, event->i );
    }

    void cameraUpdate( Camera *camera ) {
        // manage position pan 
        vec2 move = { .x = 0.f, .y = 0.f };
        if( IsKeyDown( K_W ) )
            move.y -= 1.f;
        if( IsKeyDown( K_A ) )
            move.x -= 1.f;
        if( IsKeyDown( K_S ) )
            move.y += 1.f;
        if( IsKeyDown( K_D ) )
            move.x += 1.f;

        if( move.x || move.y )
            Camera_move( camera, &move );
    }

// Event Listener for window resizing
    void sceneWindowResizing( const Event *event, void *data ) {
        Scene *s = (Scene*)data;

        // update all texts with new window size
        for( u32 i = 0; i < s->texts->mMaxIndex; ++i ) {
            if( HandleManager_isUsed( s->texts->mUsed, i ) )
                Text_setString( s->texts->mMeshes[i], s->texts->mFonts[i], s->texts->mStrings[i] );
        }
        for( u32 i = 0; i < s->widgets->mMaxIndex; ++i ) {
            if( HandleManager_isUsed( s->widgets->mTextUsed, i ) )
                Text_setString( s->widgets->mTextMeshes[i], s->widgets->mFonts[i], s->widgets->mStrings[i] );
        }
    }


Scene *Scene_init() {
    Scene *s = NULL;
    
    s = byte_alloc( sizeof( Scene ) );
    check_mem( s );

    // sprites
    // create array of entities (initial size = 50)
    s->sprites = SpriteArray_init( 50 );

    // init default sprite shader
    int ss = ResourceManager_get( "sprite_shader.json" );
    check( ss >= 0, "Sprite shader creation error!\n" );

    s->sprite_shader = ss;

    // texts
    // create array of texts (intial size = 50)
    s->texts = TextArray_init( 50 );

    // init default text shader
    int ts = ResourceManager_get( "text_shader.json" );
    check( ts >= 0, "Text shader creation error!\n" );

    s->text_shader = ts;


    // widgets
    s->widgets = WidgetArray_init(50);

    // default ui shader
    int ws = ResourceManager_get( "ui_shader.json" );
    check( ws >= 0, "UI Shader creation error!\n" );

    s->ui_shader = ws;

    // map
        SceneMap_init( s );

    // camera
        s->camera = Camera_new();
        check_mem( s->camera );

        Camera_registerListener( s->camera, cameraMouseListener, LT_MouseListener );
        Camera_registerUpdateFunction( s->camera, cameraUpdate );


    // 2D proj matrix (just ortho2D camera with no zoom or pan)
    vec2i winsize = Context_getSize();
    mat3_ortho( &s->proj_matrix_2d, 0.f, winsize.x, winsize.y, 0.f );

    // Event listener
    EventManager_addListener( LT_ResizeListener, sceneWindowResizing, s );

    // Lights
    Color_set( &s->ambient_color, 0.0f, 0.0f, 0.0f, 1.f );

    Renderer_useShader( s->local_map.shader );
    Shader_sendColor( "amb_color", &s->ambient_color );

    vec2 lightpos = { 300.f, 200.f };
    Color diffuse = { 1.f, 1.f, 1.f, 1.f };
    Light_set( &s->light1, &lightpos, 150.f, &diffuse, 0.382f, 0.01f, 0.f );
        
    return s;
error:
    Scene_destroy( s );
    return NULL;
}

void Scene_destroy( Scene *pScene ) {
    if( pScene ) {
        SpriteArray_destroy( pScene->sprites );
        TextArray_destroy( pScene->texts );
        WidgetArray_destroy( pScene->widgets );
        Camera_destroy( pScene->camera );
        DEL_PTR( pScene );
    }   
}

void Scene_update( Scene *pScene ) {
    Camera_update( pScene->camera );
}

void Scene_updateShadersProjMatrix( Scene *pScene ) {
    Renderer_updateProjectionMatrix( ECamera, &pScene->camera->mProjectionMatrix );
    Renderer_updateProjectionMatrix( EGui, &pScene->proj_matrix_2d );
}

void Scene_render( Scene *pScene ) {
    if( pScene ) {
        glDisable( GL_CULL_FACE );
        // ##################################################
        //      RENDER MAP
        Renderer_useShader( pScene->local_map.shader );
        Shader_sendInt( "Depth", 9 );
        Shader_sendColor( "light_color", &pScene->light1.diffuse );
        Shader_sendVec2( "light_pos", &pScene->light1.position );
        Shader_sendFloat( "light_height", pScene->light1.height );
        Shader_sendFloat( "light_cstatt", pScene->light1.cst_att );
        Shader_sendFloat( "light_linatt", pScene->light1.lin_att );
        Shader_sendFloat( "light_quadatt", pScene->light1.quad_att );

        Renderer_useTexture( pScene->local_map.texture, 0 );
        mat3 m;
        mat3_identity( &m );
        Shader_sendMat3( "ModelMatrix", &m );
        Renderer_renderMesh( pScene->local_map.mesh );

        // ##################################################
        //      RENDER SPRITES
        Renderer_useShader( pScene->sprite_shader );

        for( u32 i = 0; i < pScene->sprites->mMaxIndex; ++i ) {
            if( HandleManager_isUsed( pScene->sprites->mUsed, i ) ) {
                // use sprites different textures (multi texturing)
                Renderer_useTexture( pScene->sprites->mTextures0[i], 0 );
                Renderer_useTexture( pScene->sprites->mTextures1[i], 1 );
                Shader_sendMat3( "ModelMatrix", &pScene->sprites->mMatrices[i] );
                Shader_sendInt( "Depth", pScene->sprites->mDepths[i] );
                Shader_sendColor( "light_color", &pScene->light1.diffuse );
                Shader_sendVec2( "light_pos", &pScene->light1.position );
                Shader_sendFloat( "light_height", pScene->light1.height );
                Shader_sendFloat( "light_cstatt", pScene->light1.cst_att );
                Shader_sendFloat( "light_linatt", pScene->light1.lin_att );
                Shader_sendFloat( "light_quadatt", pScene->light1.quad_att );
                Renderer_renderMesh( pScene->sprites->mMeshes[i] );
            }
        }


        // ##################################################
        //      RENDER TEXTS
        Renderer_useShader( pScene->text_shader );

        for( u32 i = 0; i < pScene->texts->mMaxIndex; ++i ) {
            if( HandleManager_isUsed( pScene->texts->mUsed, i ) ) {
                Renderer_useTexture( pScene->texts->mFonts[i]->mTexture, 0 );
                Shader_sendInt( "Depth", pScene->texts->mDepths[i] );
                Shader_sendColor( "Color", &pScene->texts->mColors[i] );
                Shader_sendVec2( "Position", &pScene->texts->mPositions[i] );
                Renderer_renderMesh( pScene->texts->mMeshes[i] );
            }
        }

        // ##################################################
        //      RENDER WIDGETS
        Renderer_useShader( pScene->ui_shader );

        for( u32 i = 0; i < pScene->widgets->mMaxIndex; ++i ){
            if( HandleManager_isUsed( pScene->widgets->mUsed, i ) ) {
                if( HandleManager_getHandle( pScene->widgets->mEntityUsed, i ) != 0) {
                    Renderer_useTexture( pScene->widgets->mTextures[i], 0 );
                    vec2 v = vec2_vec2i( &pScene->widgets->mPositions[i] );
                    Shader_sendVec2( "Position", &v );
                    Shader_sendInt( "Depth", pScene->widgets->mDepths[i] );
                    Renderer_renderMesh( pScene->widgets->mTextureMeshes[i] );
                }
            }
        }
    }
}

//  =======================
 
int  Scene_addSprite( Scene *pScene, u32 pMesh, int pTexture[2], mat3 *pMM ) {
    int handle = -1;

    if( pScene ) {
        handle = SpriteArray_add( pScene->sprites );

        if( handle >= 0 ) {
            pScene->sprites->mMeshes[handle] = pMesh;
            pScene->sprites->mTextures0[handle] = pTexture[0];
            pScene->sprites->mTextures1[handle] = pTexture[1];
            memcpy( &pScene->sprites->mMatrices[handle], pMM, 9 * sizeof( f32 ) ); 
        }
    }
    return handle;
}

int  Scene_addSpriteFromActor( Scene *pScene, Actor *pActor ) { 
    int handle = -1;

    if( pScene && pActor ) {
        handle = SpriteArray_add( pScene->sprites );

        if( handle >= 0 ) {
            pActor->used_sprite = handle;
            pScene->sprites->mMeshes[handle] = pActor->mesh_id;
            pScene->sprites->mTextures0[handle] = pActor->texture_ids[0];
            pScene->sprites->mTextures1[handle] = pActor->texture_ids[1];
            mat3 m;
            mat3_translationMatrixfv( &m, &pActor->mPosition );
            memcpy( pScene->sprites->mMatrices[handle].x, m.x, 9 * sizeof( f32 ) ); 
        }
    }
    return handle;
}

void Scene_modifySprite( Scene *pScene, u32 pHandle, SpriteAttrib pAttrib, void *pData ) {
    if( pScene ) {
        if( HandleManager_isUsed( pScene->sprites->mUsed, pHandle ) ) {
            switch( pAttrib ) {
                case SA_Matrix :
                    memcpy( &pScene->sprites->mMatrices[pHandle], (mat3*)pData, 9 * sizeof( f32 ) ); 
                    break;
                case SA_Texture0 :
                    pScene->sprites->mTextures0[pHandle] = *((u32*)pData);
                    break;
                case SA_Texture1 :
                    pScene->sprites->mTextures1[pHandle] = *((int*)pData);
                    break;
                case SA_Depth :
                    pScene->sprites->mDepths[pHandle] = *((u32*)pData);
                    break;
            }
        }
    }   
}

void Scene_transformSprite( Scene *pScene, u32 pHandle, mat3 *pTransform ) {
    if( pScene && pTransform ) {
        if( HandleManager_isUsed( pScene->sprites->mUsed, pHandle ) ) {
            mat3_mul( &pScene->sprites->mMatrices[pHandle], pTransform );
        }
    }
}

void Scene_removeSprite( Scene *pScene, u32 pIndex ) {
    if( pScene ) 
        SpriteArray_remove( pScene->sprites, pIndex );
}

void Scene_clearSprites( Scene *pScene ) {
    if( pScene ) 
        SpriteArray_clear( pScene->sprites );
}

//  =======================

int Scene_addText( Scene *pScene, const Font *pFont, Color pColor ) {
    int handle = -1;

    if( pScene ) {
        handle = TextArray_add( pScene->texts );

        if( handle >= 0 ) {
            pScene->texts->mFonts[handle] = pFont;
            pScene->texts->mColors[handle] = pColor;
        }
    }

    return handle;
}

void Scene_modifyText( Scene *pScene, u32 pHandle, TextAttrib pAttrib, void *pData ) {
    if( pScene ) {
        // check if the given handle is a used text
        if( HandleManager_isUsed( pScene->texts->mUsed, pHandle ) ) {
            switch( pAttrib ) {
                case TA_Position :
                    {
                        vec2 new_pos = *((vec2*)pData);
                        pScene->texts->mPositions[pHandle] = new_pos;
                    }
                    break;
                case TA_Depth :
                    pScene->texts->mDepths[pHandle] = *((int*)pData);
                    break;
                case TA_String :
                    {
                        const char *s = (const char*)pData;

                        // recreate VBO
                        Text_setString( pScene->texts->mMeshes[pHandle], pScene->texts->mFonts[pHandle], s );

                        // copy string inside textarray to keep track of current string 
                        pScene->texts->mStrings[pHandle] = byte_realloc( pScene->texts->mStrings[pHandle], strlen( s ) + 1 );
                        strcpy( pScene->texts->mStrings[pHandle], s );
                    }
                    break;
                case TA_Font :
                    pScene->texts->mFonts[pHandle] = (const Font*)pData;
                    break;
                case TA_Color:
                    pScene->texts->mColors[pHandle] = *((Color*)pData);
                    break;
            }
        }
    }
}

void Scene_removeText( Scene *pScene, u32 pIndex ) {
    if( pScene ) 
        TextArray_remove( pScene->texts, pIndex );
}

void Scene_clearTexts( Scene *pScene ) {
    if( pScene ) 
        TextArray_clear( pScene->texts );
}


//  =======================

int Scene_addWidget( Scene *pScene, WidgetType pWT, void* pDataStruct, int pMother ) {
    int handle = -1;

    if( pScene ){
        handle = WidgetArray_add( pScene->widgets, pWT, pMother );
        switch( pWT ) {
            case WT_Master :
                if( handle >= 0 ) {
                    pScene->widgets->mBounds[handle] = Context_getSize();
                }
                break;
            case WT_Text :
                {
                    if( handle >= 0 ) {
                        WidgetTextAttributes wta = *(WidgetTextAttributes*)pDataStruct;
                        pScene->widgets->mBounds[handle] = wta.mBounds;
                        pScene->widgets->mTextPositions[handle] = wta.mPosition;
                        pScene->widgets->mFonts[handle] = wta.mFont;
                        pScene->widgets->mColors[handle] = wta.mColor;
                    }
                }
                break;
            case WT_Sprite :
                {
                    if( handle >= 0 ) {
                        WidgetSpriteAttributes wsa = *(WidgetSpriteAttributes*)pDataStruct;
                        pScene->widgets->mTextureMeshes[handle] = wsa.mMesh;
                        pScene->widgets->mTextures[handle] = wsa.mTexture;
                        pScene->widgets->mPositions[handle] = wsa.mPosition;
                        pScene->widgets->mBounds[handle] = wsa.mBounds;

                    }
                }
                break;
            case WT_Button :
                {
                    if( handle >= 0 ) {
                        WidgetButtonAttributes wba = *(WidgetButtonAttributes*)pDataStruct;
                        pScene->widgets->mFonts[handle] = wba.mFont;
                        pScene->widgets->mColors[handle] = wba.mColor;
                        pScene->widgets->mTextureMeshes[handle] = wba.mMesh;
                        pScene->widgets->mTextures[handle] = wba.mTexture;
                        pScene->widgets->mPositions[handle] = wba.mPosition;
                        pScene->widgets->mBounds[handle] = wba.mBounds;

                    }
                }
                break;
        }
    }
    pScene->widgets->mWidgetTypes[handle] = pWT;
    return handle;
}

void Scene_modifyWidget( Scene *pScene, u32 pHandle, WidgetAttrib pAttrib, void *pData ) {
    if( pScene ) {
        if( HandleManager_isUsed( pScene->widgets->mUsed, pHandle ) ) {
            if( HandleManager_getHandle( pScene->widgets->mEntityUsed, pHandle ) != 0) {
                switch( pAttrib ) {
                    case WA_Texture :
                        {
                            u32 new_texture = *((u32*)pData);
                            pScene->widgets->mTextures[pHandle] = new_texture;
                        }
                        break;
                    case WA_Position :
                        pScene->widgets->mPositions[pHandle] = *((vec2i*)pData);
                        break;
                    case WA_Bounds :
                        {
                            vec2i new_bounds = *((vec2i*)pData);
                            pScene->widgets->mBounds[pHandle] = new_bounds;
                        }
                        break;
                    case WA_Depth :
                        pScene->widgets->mDepths[pHandle] = *((u32*)pData);
                        break;
                    default :
                        break;
                }
            }
            if( HandleManager_getHandle( pScene->widgets->mTextUsed, pHandle ) != 0) {
                switch( pAttrib ) {
                    case WA_Bounds :
                        {
                            vec2i new_bounds = *((vec2i*)pData);
                            pScene->widgets->mBounds[pHandle] = new_bounds;
                        }
                        break;
                    case WA_Font :
                        {
                            const Font* new_font = ((const Font*)pData);
                            pScene->widgets->mFonts[pHandle] = new_font;
                        }
                        break;
                    case WA_Color :
                        pScene->widgets->mColors[pHandle] = *((Color*)pData);
                        break;
                    case WA_TextPosition :
                        {
                            vec2i new_pos = *((vec2i*)pData);

                            //  Re-place the text on the entity so that a button will look like a button. ( //TODO: center the text )
                            if( pScene->widgets->mWidgetTypes[pHandle] == WT_Button ) {
                                new_pos.x = pScene->widgets->mPositions[pHandle].x;
                                new_pos.y = pScene->widgets->mPositions[pHandle].y;
                            }

                            pScene->widgets->mTextPositions[pHandle] = new_pos;
                        }
                        break;
                    case WA_String :
                        {
                            const char *s = (const char*)pData;

                            // recreate VBO
                            Text_setString( pScene->widgets->mTextMeshes[pHandle], pScene->widgets->mFonts[pHandle], s );

                            // copy string inside textarray to keep track of current string
                            DEL_PTR(pScene->widgets->mStrings[pHandle] );

                            pScene->widgets->mStrings[pHandle] = byte_alloc( strlen( s ) + 1 );
                            strcpy( pScene->widgets->mStrings[pHandle], s );
                        }
                        break;
                    default :
                        break;
                }
            }
        }
    }
}



void Scene_removeWidget( Scene *pScene, u32 pWidget ) {
    if( pScene ){
        WidgetArray_remove( pScene->widgets, pWidget );
    }
}

void Scene_clearWidgets( Scene *pScene) {
    if( pScene ){
        WidgetArray_clear( pScene->widgets );
    }
}
