#ifndef BYTE_SCENE_HPP
#define BYTE_SCENE_HPP

#include "common.h"
#include "handlemanager.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"
#include "text.h"
#include "entity.h"
#include "actor.h"
#include "widget.h"

// Forward declaration
typedef struct s_Scene Scene;


/// Create and returns a new scene instance
Scene *Scene_new();

/// Destroy and free the given scene
void Scene_destroy( Scene *pScene );

/// Update the scene (camera, etc)
void Scene_update( Scene *pScene );

/// Update widgets : check if user interacted with one of them.
void Scene_updateWidgets( Scene *pScene, vec2 pMouse );

/// Render all entities in the scene
void Scene_render();

// ##########################################################################3
//      Entities Array
    /// Add an entity to be rendered each frame in the scene
    /// @param pMesh : The mesh handle the entity use
    /// @param pTexture : The texture handle the entity use
    /// @param pMM : The ModelMatrix used to orient the entity
    /// @return : The handle to the given entity
    int  Scene_addEntity( Scene *pScene, u32 pMesh, u32 pTexture, mat3 *pMM );

    /// Add an entity to be rendered each frame in the scene
    /// @param pActor : the Actor that must be drawn as an entity
    /// @return : The handle to the given entity
    int  Scene_addEntityFromActor( Scene *pScene, Actor *pActor );

    /// Modify one attribute of a Entity (given by its handle)
    /// @param pAttrib : the attribute type we want to change (see entity.h)
    /// @param pData   : data set as the attrib
    void Scene_modifyEntity( Scene *pScene, u32 pHandle, EntityAttrib pAttrib, void *pData );

    /// Remove an entity from the scene rendered entities (by its handle)
    void Scene_removeEntity( Scene *pScene, u32 pIndex );

    /// Clears the whole entity array, to set a new scene
    void Scene_clearEntities( Scene *pScene );

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
    /// Add a widget to be rendered
    /// @param pWT : the type of the widget (see widget.h).
    /// @param pData : the corresponding data structure to set the widget (see widget.h).
    int Scene_addWidget( Scene *pScene, WidgetType pWT, void* pDataStruct, int pMother );

    void Scene_modifyWidget( Scene *pScene, u32 pHandle, WidgetAttrib pAttrib, void *pData );

    void Scene_removeWidget( Scene *pScene, u32 pWidget );

    void Scene_clearWidgets( Scene *pScene );


#endif // BYTE_SCENE
