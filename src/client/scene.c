#include "common/world.h"
#include "scene.h"
#include "renderer.h"
#include "camera.h"
#include "device.h"
#include "context.h"
#include "resource.h"

#include "game.h"

#ifdef USE_GLDL
#include "GL/gldl.h"
#else
#include "GL/glew.h"
#endif

typedef struct s_Scene {
    u32             mSpriteShader;      ///< Shader used to render sprites
    SpriteArray     *mSprites;          ///< Sprites in the scene

    u32             mTextShader;        ///< Shader used to render texts
    TextArray       *mTexts;            ///< Texts in the scene

    Camera          *mCamera;           ///< Camera of the scene

    struct {
        u32 mesh;
        u32 texture;
        u32 shader;
    }               local_map;
} Scene;




// Camera listeners and update function
    void cameraMouseListener( const Event *pEvent, void *pCamera ) {
        Camera *cam = (Camera*)pCamera;
        // manage zoom event 
        if( pEvent->Type == E_MouseWheelMoved ) 
            Camera_zoom( cam, pEvent->i );
    }

    void cameraUpdate( Camera *pCamera ) {
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
            Camera_move( pCamera, &move );
    }

// Event Listener for window resizing
    void sceneWindowResizing( const Event *pEvent, void *pData ) {
        Scene *s = (Scene*)pData;

        // update all texts with new window size
        for( u32 i = 0; i < s->mTexts->mMaxIndex; ++i ) {
            if( HandleManager_isUsed( s->mTexts->mUsed, i ) )
                Text_setString( s->mTexts->mMeshes[i], s->mTexts->mFonts[i], s->mTexts->mStrings[i] );
        }
    }

// MAP visual representation creation/rendering
void initMap( Scene *scene ) {
    vec2 map_pos[4*LOCAL_MAP_WIDTH*LOCAL_MAP_HEIGHT];
    vec2 map_tcs[4*LOCAL_MAP_WIDTH*LOCAL_MAP_HEIGHT];
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
            xi = i * 4;
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
            map_tcs[yj+xi+2].x = 1.f;     map_tcs[yj+xi+2].y = 1.f;
            map_tcs[yj+xi+3].x = 1.f;     map_tcs[yj+xi+3].y = 0.f;

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
    scene->local_map.mesh = Renderer_createStaticMesh( GL_TRIANGLES, map_indices, sizeof(map_indices), map_pos, sizeof(map_pos), map_tcs, sizeof(map_tcs) );
 
    // get shader and texture
    scene->local_map.texture = ResourceManager_get( "map.png" );
    scene->local_map.shader = ResourceManager_get( "map_shader.json" );
}

vec2 Scene_localToGlobal( Scene *scene, const vec2 *local ) {
    vec2 ret = {0,0};
    ret = vec2_add( &ret, local );
    ret = vec2_mul( &ret, scene->mCamera->mZoom );
    ret = vec2_add( &ret, &scene->mCamera->global_position );

    return ret;
}

vec2 Scene_screenToIso( Scene *scene, const vec2 *local ) {
    static vec2 up =    {  50.f,  0.f };
    static vec2 down =  {  50.f, 50.f };
    static vec2 left =  {   0.f, 25.f };
    static vec2 right = { 100.f, 25.f };
    static vec2 tilesize = { 100.f, 50.f };

    // get global mouse position (not depending on camera zoom or pan)
    vec2 global = Scene_localToGlobal( scene, local );

    vec2 ret, offset;
    ret.x = (int)( global.x / tilesize.x );
    ret.y = (int)( global.y / tilesize.y ) * 2;
    offset.x = (int)fmod( global.x, tilesize.x );
    offset.y = (int)fmod( global.y, tilesize.y );


    int upleft = PointOnLine( &offset, &left, &up );
    if( upleft < 0 ) {
        ret.x -= 1;
        ret.y -= 1;
    } else {
        int downleft = PointOnLine( &offset, &left, &down );
        if( downleft > 0 ) {
            ret.x -= 1;
            ret.y += 1;
        } else {
            int upright = PointOnLine( &offset, &up, &right );
            if( upright < 0 ) {
                ret.y -= 1;
            } else {
                int downright = PointOnLine( &offset, &down, &right );
                if( downright > 0 ) 
                    ret.y += 1;
            }
        }
    }

    return ret;
}

Scene *Scene_new() {
    Scene *s = NULL;
    
    s = byte_alloc( sizeof( Scene ) );
    check_mem( s );

    // create array of entities (initial size = 50)
    s->mSprites = SpriteArray_init( 50 );

    // init default sprite shader
    int ss = ResourceManager_get( "sprite_shader.json" );
    check( ss >= 0, "Sprite shader creation error!\n" );

    s->mSpriteShader = ss;

    // create array of texts (intial size = 50)
    s->mTexts = TextArray_init( 50 );

    // init default text shader
    int ts = ResourceManager_get( "text_shader.json" );
    check( ts >= 0, "Text shader creation error!\n" );

    s->mTextShader = ts;

    // map
        initMap( s );

    // camera
        s->mCamera = Camera_new();
        check_mem( s->mCamera );

        Camera_registerListener( s->mCamera, cameraMouseListener, LT_MouseListener );
        Camera_registerUpdateFunction( s->mCamera, cameraUpdate );

        Device_setCamera( s->mCamera );

    // Event listener
    EventManager_addListener( LT_ResizeListener, sceneWindowResizing, s );
        
    return s;
error:
    Scene_destroy( s );
    return NULL;
}

void Scene_destroy( Scene *pScene ) {
    if( pScene ) {
        SpriteArray_destroy( pScene->mSprites );
        TextArray_destroy( pScene->mTexts );
        Camera_destroy( pScene->mCamera );
        DEL_PTR( pScene );
    }   
}

void Scene_update( Scene *pScene ) {
    Camera_update( pScene->mCamera );
}

void Scene_render( Scene *pScene ) {
    if( pScene ) {
        glDisable( GL_CULL_FACE );
        // ##################################################
        //      RENDER MAP
        Renderer_useShader( pScene->local_map.shader );
        Color c = { 0.8f, 0.8f, 0.8f, 1.f };
        Shader_sendColor( "iColor", &c );
        Shader_sendInt( "Depth", 9 );

        Renderer_useTexture( pScene->local_map.texture, 0 );
        mat3 m;
        mat3_identity( &m );
        Shader_sendMat3( "ModelMatrix", &m );
        Renderer_renderMesh( pScene->local_map.mesh );

        // ##################################################
        //      RENDER SPRITES
        Renderer_useShader( pScene->mSpriteShader );

        for( u32 i = 0; i < pScene->mSprites->mMaxIndex; ++i ) {
            if( HandleManager_isUsed( pScene->mSprites->mUsed, i ) ) {
                Renderer_useTexture( pScene->mSprites->mTextures[i], 0 );
                Shader_sendMat3( "ModelMatrix", &pScene->mSprites->mMatrices[i] );
                Shader_sendInt( "Depth", pScene->mSprites->mDepths[i] );
                Renderer_renderMesh( pScene->mSprites->mMeshes[i] );
            }
        }


        // ##################################################
        //      RENDER TEXTS
        Renderer_useShader( pScene->mTextShader );

        for( u32 i = 0; i < pScene->mTexts->mMaxIndex; ++i ) {
            if( HandleManager_isUsed( pScene->mTexts->mUsed, i ) ) {
                Renderer_useTexture( pScene->mTexts->mFonts[i]->mTexture, 0 );
                Shader_sendColor( "Color", &pScene->mTexts->mColors[i] );
                Shader_sendVec2( "Position", &pScene->mTexts->mPositions[i] );
                Renderer_renderMesh( pScene->mTexts->mMeshes[i] );
            }
        }
    }
}

//  =======================
 
int  Scene_addSprite( Scene *pScene, u32 pMesh, u32 pTexture, mat3 *pMM ) {
    int handle = -1;

    if( pScene ) {
        handle = SpriteArray_add( pScene->mSprites );

        if( handle >= 0 ) {
            pScene->mSprites->mMeshes[handle] = pMesh;
            pScene->mSprites->mTextures[handle] = pTexture;
            memcpy( &pScene->mSprites->mMatrices[handle], pMM, 9 * sizeof( f32 ) ); 
        }
    }
    return handle;
}

int  Scene_addSpriteFromActor( Scene *pScene, Actor *pActor ) { 
    int handle = -1;

    if( pScene && pActor ) {
        handle = SpriteArray_add( pScene->mSprites );

        if( handle >= 0 ) {
            pActor->mUsedSprite = handle;
            pScene->mSprites->mMeshes[handle] = pActor->mMesh_id;
            pScene->mSprites->mTextures[handle] = pActor->mTexture_id;
            mat3 m;
            mat3_translationMatrixfv( &m, &pActor->mPosition );
            memcpy( pScene->mSprites->mMatrices[handle].x, m.x, 9 * sizeof( f32 ) ); 
        }
    }
    return handle;
}

void Scene_modifySprite( Scene *pScene, u32 pHandle, SpriteAttrib pAttrib, void *pData ) {
    if( pScene ) {
        if( HandleManager_isUsed( pScene->mSprites->mUsed, pHandle ) ) {
            switch( pAttrib ) {
                case SA_Matrix :
                    memcpy( &pScene->mSprites->mMatrices[pHandle], (mat3*)pData, 9 * sizeof( f32 ) ); 
                    break;
                case SA_Texture :
                    pScene->mSprites->mTextures[pHandle] = *((u32*)pData);
                    break;
                case SA_Depth :
                    pScene->mSprites->mDepths[pHandle] = *((u32*)pData);
                    break;
            }
        }
    }   
}

void Scene_transformSprite( Scene *pScene, u32 pHandle, mat3 *pTransform ) {
    if( pScene && pTransform ) {
        if( HandleManager_isUsed( pScene->mSprites->mUsed, pHandle ) ) {
            mat3_mul( &pScene->mSprites->mMatrices[pHandle], pTransform );
        }
    }
}

void Scene_removeSprite( Scene *pScene, u32 pIndex ) {
    if( pScene ) 
        SpriteArray_remove( pScene->mSprites, pIndex );
}

void Scene_clearSprites( Scene *pScene ) {
    if( pScene ) 
        SpriteArray_clear( pScene->mSprites );
}

//  =======================

int Scene_addText( Scene *pScene, const Font *pFont, Color pColor ) {
    int handle = -1;

    if( pScene ) {
        handle = TextArray_add( pScene->mTexts );

        if( handle >= 0 ) {
            pScene->mTexts->mFonts[handle] = pFont;
            pScene->mTexts->mColors[handle] = pColor;
        }
    }

    return handle;
}

void Scene_modifyText( Scene *pScene, u32 pHandle, TextAttrib pAttrib, void *pData ) {
    if( pScene ) {
        // check if the given handle is a used text
        if( HandleManager_isUsed( pScene->mTexts->mUsed, pHandle ) ) {
            switch( pAttrib ) {
                case TA_Position :
                    {
                        vec2 new_pos = *((vec2*)pData);

                        vec2 ws = Context_getSize();
                        f32 sx = 2.f / ws.x, sy = 2.f / ws.y;

                        new_pos.x *= sx;
                        new_pos.y *= -sy;

                        pScene->mTexts->mPositions[pHandle] = new_pos;
                    }
                    break;
                case TA_String :
                    {
                        const char *s = (const char*)pData;

                        // recreate VBO
                        Text_setString( pScene->mTexts->mMeshes[pHandle], pScene->mTexts->mFonts[pHandle], s );

                        // copy string inside textarray to keep track of current string
                        DEL_PTR(pScene->mTexts->mStrings[pHandle] );

                        pScene->mTexts->mStrings[pHandle] = byte_alloc( strlen( s ) + 1 );
                        strcpy( pScene->mTexts->mStrings[pHandle], s );
                    }
                    break;
                case TA_Font :
                    pScene->mTexts->mFonts[pHandle] = (const Font*)pData;
                    break;
                case TA_Color:
                    pScene->mTexts->mColors[pHandle] = *((Color*)pData);
                    break;
            }
        }
    }
}

void Scene_removeText( Scene *pScene, u32 pIndex ) {
    if( pScene ) 
        TextArray_remove( pScene->mTexts, pIndex );
}

void Scene_clearTexts( Scene *pScene ) {
    if( pScene ) 
        TextArray_clear( pScene->mTexts );
}
