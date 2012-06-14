#ifndef BYTE_SCENE_H
#define BYTE_SCENE_H

#include "common/common.h"
#include "common/handlemanager.h"
#include "anim.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"
#include "text.h"
#include "sprite.h"
#include "static.h"
#include "widget.h"
#include "gui.h"
#include "light.h"
#include "camera.h"
#include "agent.h"

/// Visual representation of the map in game
typedef struct {
    u32     mesh;
    u32     texture;

    vec2i   location;                   ///< Location of the upper-left tile
                                        ///< as a world-tile
    vec2i   global_loc;                 ///< Same but as a global regular tile.
} SceneMap;

typedef struct {
    SceneMap            map;
    //Mesh                *walls;
    //StaticObjectArray   *wall_objs;
} SceneTile;

/// Scene structure. Keep all info about rendering in the current view
typedef struct {
    u32             map_shader;         ///< Shader used to render map

    u32             sprite_shader;      ///< Shader used to render sprites
    SpriteArray     *sprites;           ///< Sprites in the scene

    u32             text_shader;        ///< Shader used to render texts
    TextArray       *texts;             ///< Texts in the scene

    u32             ui_shader;          ///< Shader used to render ui elements
    WidgetArray     *widgets;           ///< Widgets in the scene. 

    Camera          *camera;            ///< Camera of the scene

    mat3            proj_matrix_2d;     ///< 2D projection matrix (GUI & text)

    //SceneMap        local_map;

    Light           *lights[8];
    u32             used_lights;
    Color           ambient_color;

    //Mesh            *walls;
    //StaticObjectArray *wall_objs;

    SceneMap        map;

    //SceneTile       tiles[9];           ///< 9 Scene Tiles
                                        ///< one for current player pos.
                                        ///< 8 for surrounding ones
} Scene;


/// Initialize the given scene
bool Scene_init( Scene **s );

/// Destroy and free the given scene
void Scene_destroy( Scene *pScene );

/// Scene receive an event to be processed
void Scene_receiveEvent( Scene *scene, const Event *evt );

/// Update the scene (camera, etc)
void Scene_update( Scene *scene, f32 frame_time, GameMode mode );

/// Render all sprites & texts in the scene
void Scene_render( Scene *pScene );

/// Update all shaders with the scene projection matrices
void Scene_updateShadersProjMatrix( Scene *pScene );

/// Returns the SceneTile at (i,j) coordinates, if present
//SceneTile *Scene_getTile( Scene *scene, u32 x, u32 y );

/// Load a 3x3 world-tiles centered on (x+1,y+1) to be considered as the current scene
/// It will load the map geometry and all agents and statics as sprites.
void Scene_setLocation( Scene *scene, int x, int y );

// ##########################################################################3
//      Scene Map
    /// Returns the global coordinates of a screen pos, not depending of camera
    /// zoom or pan. 
    vec2  Scene_screenToGlobal( Scene *scene, const vec2i *local );

    /// Returns the map tile at a given local screen position
    vec2i Scene_screenToIso( Scene *scene, const vec2i *local );

    //void SceneMap_redTile( Scene *scene, const vec2i *tile, bool red );
    

// ##########################################################################3
//      Sprite Array
    /// Add a sprite to be rendered each frame in the scene
    /// @param pActor : the Actor that must be drawn as a sprite
    /// @return : The handle to the given sprite 
    int  Scene_addAgentSprite( Scene *scene, Agent *agent );

    /// Modify one attribute of a Sprite (given by its handle)
    /// @param pAttrib : the attribute type we want to change (see sprite.h)
    /// @param pData   : data set as the attrib
    void Scene_modifySprite( Scene *pScene, u32 pHandle, SpriteAttrib pAttrib, void *pData );

    /// Remove a sprite from the scene rendered sprites (by its handle)
    void Scene_removeSprite( Scene *pScene, u32 pIndex );

    /// Clears the whole sprite array, to set a new scene
    void Scene_clearSprites( Scene *pScene );

// ##########################################################################3
//      Texts Array
    /// Add a text to be rendered each frame
    /// @return : a handle to the given registered text;
    int Scene_addText( Scene *pScene, const Font *pFont, Color pColor );

    /// Modify one attribute of a Text (given by its handle)
    /// @param pAttrib : the attribute type we want to change (see text.h)
    /// @param pData   : data set as the attrib
    void Scene_modifyText( Scene *pScene, u32 pHandle, TextAttrib pAttrib, void *pData );

    /// Remove a text from the scene (by its handle).
    void Scene_removeText( Scene *pScene, u32 pText );

    /// Clears the whole text array, to set a new scene
    void Scene_clearTexts( Scene *pScene );

// ##########################################################################3
//      Widgets Array
    /// Add a widget (and recursively all its children) to be rendered
    int Scene_addWidget( Scene *scene, const Widget *widget );

    /// Modify one attribute of a Widget (given by its handle)
    /// @param attrib : the attribute type we want to change (see widget.h)
    /// @param data   : data set as the attrib
    void Scene_modifyWidget( Scene *scene, u32 handle, WidgetAttrib attrib, void *data );

    /// Remove a widget from the scene (by its handle).
    void Scene_removeWidget( Scene *scene, u32 widget );

    /// Clears the whole widget array, to set a new scene
    void Scene_clearWidgets( Scene *scene );


// ##########################################################################3
//      Lights
    /// Add a light to be used for rendering (and update shaders using them)
    void Scene_addLight( Scene *scene, Light *l );

    /// Remove a light (and update shaders using them)
    void Scene_removeLight( Scene *scene, Light *l );

    /// Remove all lights from scene (and update shaders using them)
    void Scene_clearLights( Scene *scene );

    /// Notify a modification to a light present in scene attribute
    void Scene_modifyLight( Scene *scene, Light *l, LightAttribute la );

// ##########################################################################3
    /*
//      Static objects
    /// Add a static object to the current scene
    void Scene_addStaticObject( Scene *scene, StaticObject *so );

    /// Remove a static object
    void Scene_removeStaticObject( Scene *scene, int index );

    /// Build the scene static objects' VBO (call when all Statics are added)
    void Scene_buildStaticObjects( Scene *scene );
    */


#endif // BYTE_SCENE
