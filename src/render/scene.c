#include "scene.h"
#include "renderer.h"
#include "camera.h"
#include "context.h"
#include "resource.h"
#include "game.h"

#include "GL/glew.h"

// #############################################################################
//          SCENE MAP

/// Local function. Initialize the scene map
void SceneMap_init( Scene *scene ) {
    // buffer positions ( 4 vertices for each map tile )
    vec2 map_pos[4*lmap_size];
    // buffer tex coordinates ( 4 for each tile )
    vec2 map_tcs[4*lmap_size];
    // drawing indices ( 2 triangles(3verts) for each tile )
    u32  map_indices[6*lmap_size];

    // vertex position offsets
    int x_offset, y_offset;  

    // arrays indices offsets ( xi and yj for pos and tcs offsets,
    int xi, yj, ii, ij;      // ii and ij for indices               )
     

    // create map mesh data
    for( int j = 0; j < lmap_height/2; ++j )
        for( int i = 0; i < 2*lmap_width; ++i ) {
            x_offset = i * 50.f;
            y_offset = j * 50.f + (i&1) * 25.f;

            // i * 4 : 4 vertices for each tile
            xi = i * 4;
            // j * 2 : each row index is a col of 2 tiles 
            // LOCAL_MAP_WIDTH * 4 : 4 vertices for a complete row of tiles
            yj = j * 2 * lmap_width * 4;
            map_pos[yj+xi+0].x = x_offset + 50.f;
            map_pos[yj+xi+0].y = y_offset + 50.f;
            map_pos[yj+xi+1].x = x_offset;
            map_pos[yj+xi+1].y = y_offset + 25.f;
            map_pos[yj+xi+2].x = x_offset + 50.f;
            map_pos[yj+xi+2].y = y_offset;
            map_pos[yj+xi+3].x = x_offset + 100.f;
            map_pos[yj+xi+3].y = y_offset + 25.f; 

            map_tcs[yj+xi+0].x = 0.5f;     map_tcs[yj+xi+0].y = 1.f;
            map_tcs[yj+xi+1].x = 0.f;     map_tcs[yj+xi+1].y = 0.5f;
            map_tcs[yj+xi+2].x = 0.5f;     map_tcs[yj+xi+2].y = 0.f;
            map_tcs[yj+xi+3].x = 1.f;     map_tcs[yj+xi+3].y = 0.5f;

            ii = i * 6;
            ij = j * 2 * lmap_width * 6;
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
    scene->local_map.texture = ResourceManager_get( "pierre.png" );
    scene->local_map.shader = ResourceManager_get( "map_shader.json" );
}

void SceneMap_redTile( Scene *scene, const vec2i *tile, bool red ) {
    if( tile->x >= lmap_width*2 || tile->y >= lmap_height/2 ) 
        return;

    Mesh *m = Renderer_getMesh( scene->local_map.mesh );
    u32 tcs_offset = m->vertex_count * 2;
    int i_offset = tile->x * 8,
        j_offset = tile->y * 2 * lmap_width * 8;
 

    // we change only on even indices, only the X coord, not Y
    m->data[tcs_offset+i_offset+j_offset+0] = red ? 0.75f : 0.25f;
    m->data[tcs_offset+i_offset+j_offset+2] = red ? 0.5f : 0.0f;
    m->data[tcs_offset+i_offset+j_offset+4] = red ? 0.75f : 0.25f;
    m->data[tcs_offset+i_offset+j_offset+6] = red ? 1.f : 0.5f;

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

    // get global mouse position (not depending on camera zoom or pan)
    vec2 global = Scene_localToGlobal( scene, local );

    vec2i ret, offset;
    ret.x = 2 * (int)( global.x / tile_w);
    ret.y = global.y / tile_h;
    offset.x = (int)global.x % tile_w;
    offset.y = (int)global.y % tile_h;


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
    s->ambient_color = (Color){ 0.05f, 0.05f, 0.05f, 1.f };

    Renderer_useShader( s->local_map.shader );
    Shader_sendColor( "amb_color", &s->ambient_color );
    Shader_sendFloat( "light_power", 1.f );
    Renderer_useShader( s->sprite_shader );
    Shader_sendColor( "amb_color", &s->ambient_color );
    Shader_sendFloat( "light_power", 1.f );

    vec2i lightpos = { 12, 5 };
    Color diffuse = { 1.f, 1.f, 1.f, 1.f };
    Light_set( &s->light1, &lightpos, 2100.f, 200.f, &diffuse );
        


    return s;

error:
    Scene_destroy( s );
    return NULL;
}

void Scene_destroy( Scene *scene ) {
    if( scene ) {
        SpriteArray_destroy( scene->sprites );
        TextArray_destroy( scene->texts );
        WidgetArray_destroy( scene->widgets );
        Camera_destroy( scene->camera );

        DEL_PTR( scene );
    }   
}

void Scene_update( Scene *scene, f32 frame_time ) {
    Camera_update( scene->camera );

    // update all sprite animations
    for( u32 i = 0; i < scene->sprites->mMaxIndex; ++i ) 
        if( HandleManager_isUsed( scene->sprites->mUsed, i ) && scene->sprites->anims[i].frame_n >= 0 ) {
            if( Anim_update( &scene->sprites->anims[i], frame_time ) ) {
                scene->sprites->mAttributes[i].x[6] = scene->sprites->anims[i].frames[scene->sprites->anims[i].curr_n].x;
                scene->sprites->mAttributes[i].x[7] = scene->sprites->anims[i].frames[scene->sprites->anims[i].curr_n].y;
            }
        }
}

void Scene_updateShadersProjMatrix( Scene *pScene ) {
    Renderer_updateProjectionMatrix( ECamera, &pScene->camera->mProjectionMatrix );
    Renderer_updateProjectionMatrix( EGui, &pScene->proj_matrix_2d );
}

static f32 light_powers[5] = { 0.9f, 0.95f, 1.f, 0.88f, 1.f };

void Scene_render( Scene *pScene ) {
    static f32 change_power = 0.f;
    static int power_index = 0;
    if( pScene ) {
        const f32 tmp = Game_getElapsedTime();
        glDisable( GL_CULL_FACE );

        // ##################################################
        //      RENDER MAP
        Renderer_useShader( pScene->local_map.shader );
        Shader_sendInt( "Depth", 9 );
        Shader_sendColor( "light_color", &pScene->light1.diffuse );
        Shader_sendVec2( "light_pos", &pScene->light1.position );
        Shader_sendFloat( "light_radius", pScene->light1.radius );
        Shader_sendFloat( "light_height", pScene->light1.height );

        if( change_power > 0.09f ) 
            Shader_sendFloat( "light_power", light_powers[power_index] );

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
                Shader_sendMat3( "AttrMatrix", &pScene->sprites->mAttributes[i] );
                Shader_sendColor( "light_color", &pScene->light1.diffuse );
                Shader_sendVec2( "light_pos", &pScene->light1.position );
                Shader_sendFloat( "light_radius", pScene->light1.radius );
                Shader_sendFloat( "light_height", pScene->light1.height );
                if( change_power > 0.09f ) {
                    Shader_sendFloat( "light_power", light_powers[power_index] );
                    change_power = 0.f; 
                    power_index = (power_index + 1) % 5;
                }
                Renderer_renderMesh( pScene->sprites->mMeshes[i] );
            }
        }


        // ##################################################
        //      RENDER WIDGETS
        Renderer_useShader( pScene->ui_shader );

        for( u32 i = 0; i < pScene->widgets->max_index; ++i ){
            if( HandleManager_isUsed( pScene->widgets->used, i ) ) {
                Renderer_useTexture( pScene->widgets->textures[i], 0 );
                Shader_sendVec2( "Position", &pScene->widgets->positions[i] );
                Shader_sendInt( "Depth", pScene->widgets->depths[i] );
                Renderer_renderMesh( pScene->widgets->meshes[i] );
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

        change_power += Game_getElapsedTime() - tmp;
    }
}

//  =======================
/*
int  Scene_addSpriteFromActor( Scene *pScene, Actor *pActor ) { 
    int handle = -1;

    if( pScene && pActor ) {
        handle = SpriteArray_add( pScene->sprites );

        if( handle >= 0 ) {
            pActor->used_sprite = handle;
            pScene->sprites->mMeshes[handle] = pActor->assets.mesh_id;
            pScene->sprites->mTextures0[handle] = pActor->assets.texture_id[0];
            pScene->sprites->mTextures1[handle] = pActor->assets.texture_id[1];
            // set sprite global position from actor tile position
            //Game_setActorPosition( pActor, &pActor->position );

            memset( &pScene->sprites->mAttributes[handle], 0, 9 * sizeof(f32) );

            // position components
            pScene->sprites->mAttributes[handle].x[0] = pActor->position.x;
            pScene->sprites->mAttributes[handle].x[1] = pActor->position.y;

            // depth component
            pScene->sprites->mAttributes[handle].x[2] = 0;

            // mesh size components
            pScene->sprites->mAttributes[handle].x[3] = pActor->assets.mesh_size.x;
            pScene->sprites->mAttributes[handle].x[4] = pActor->assets.mesh_size.y;

            // texcoords of size of mesh on texture atlas
            if( pActor->assets.animation ) {
                pScene->sprites->mAttributes[handle].x[6] = pActor->assets.animation->frames[pActor->assets.animation->curr_n].x;
                pScene->sprites->mAttributes[handle].x[7] = pActor->assets.animation->frames[pActor->assets.animation->curr_n].y;

                // copy current actor animation
                memcpy( &pScene->sprites->anims[handle], pActor->assets.animation, sizeof(Anim) );
                // reset it
                Anim_restart( &pScene->sprites->anims[handle] );
            } else {
                pScene->sprites->mAttributes[handle].x[6] = 0;
                pScene->sprites->mAttributes[handle].x[7] = 0;
                // frame_n = -1 to signal there aint no anim
                pScene->sprites->anims[handle].frame_n = -1;
            }

        }
    }
    return handle;
}
*/

void Scene_modifySprite( Scene *pScene, u32 pHandle, SpriteAttrib pAttrib, void *pData ) {
    if( pScene ) {
        if( HandleManager_isUsed( pScene->sprites->mUsed, pHandle ) ) {
            switch( pAttrib ) {
                case SA_Position :
                    {
                        const vec2 *pos = (vec2*)pData;
                        pScene->sprites->mAttributes[pHandle].x[0] = pos->x;
                        pScene->sprites->mAttributes[pHandle].x[1] = pos->y;
                    }
                    break;
                case SA_Animation :
                    // copy of anim 
                    memcpy( &pScene->sprites->anims[pHandle], (Anim*)pData, sizeof(Anim) );

                    // reset it
                    Anim_restart( &pScene->sprites->anims[pHandle] );
                    break;
                case SA_Texture0 :
                    pScene->sprites->mTextures0[pHandle] = *((u32*)pData);
                    break;
                case SA_Texture1 :
                    pScene->sprites->mTextures1[pHandle] = *((int*)pData);
                    break;
                case SA_Depth :
                    pScene->sprites->mAttributes[pHandle].x[2] = *((u32*)pData);
                    break;
            }
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
                        vec2 new_pos = vec2_vec2i( (vec2i*)pData );
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

int Scene_addWidget( Scene *scene, const Widget *widget ) {
    int handle = -1;

    if( scene ){
        handle = WidgetArray_add( scene->widgets );
        if( handle >= 0 ) {
            scene->widgets->meshes[handle] = widget->assets.mesh;
            scene->widgets->textures[handle] = widget->assets.texture;
            scene->widgets->depths[handle] = widget->depth;
            scene->widgets->positions[handle] = vec2_vec2i( &widget->position );
        }
    }

    return handle;
}

void Scene_modifyWidget( Scene *scene, u32 handle, WidgetAttrib attrib, void *data ) {
    if( scene ) {
        if( HandleManager_isUsed( scene->widgets->used, handle ) ) {
            switch( attrib ) {
                case WA_Texture :
                    scene->widgets->textures[handle] = *((u32*)data);
                    break;
                case WA_Position :
                    scene->widgets->positions[handle] = vec2_vec2i( (vec2i*)data );
                    break;
                case WA_Depth :
                    scene->widgets->depths[handle] = *((u32*)data);
                    break;
                default :
                    break;
            }
        }
    }
}



void Scene_removeWidget( Scene *scene, u32 widget ) {
    if( scene )
        WidgetArray_remove( scene->widgets, widget );
}

void Scene_clearWidgets( Scene *scene) {
    if( scene )
        WidgetArray_clear( scene->widgets );
}
