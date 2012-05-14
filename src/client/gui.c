#include "gui.h"
#include "game.h"
#include "resource.h"
#include "renderer.h"
#include "scene.h"

////////////////////////////////////////////////////////
////    Some callback functions
void windowCloseButtonCallback( Widget* widget, const Event* e ) {
    if( e->type == EMouseReleased ){
        vec2i extents = vec2i_add( &widget->position, &widget->size );
        if( e->v.x < widget->position.x || e->v.x > extents.x || e->v.y < widget->position.y || e->v.y > extents.y )
            return;
        else
            widget->master->visible = false;
    }
}

void button1Callback( Widget* widget, const Event* e ) {
    Widget_toggleShow( windowHead );
}



//////////////////////////////////////////////////////
////    Widget functions
Widget* Widget_init( WidgetType type, vec2i* size, const char *mesh, const char *texture, int text ) {
    check( game, "Can't init widget without running game.\n" );
    Widget* widget = byte_alloc( sizeof( Widget ) );
    if( widget ){
        widget->childrenCount = 0;
        widget->childrenSize = 0;
        widget->sceneIndex = -1;
        widget->assets.text = -1;
        widget->assets.texture = -1;
        widget->assets.mesh = -1;
        widget->children = NULL;
        widget->master = NULL;
        widget->callback = NULL;
        widget->visible = true;
        widget->size = *size;
        switch( type ) {
            case WT_Root :
                widget->children = byte_alloc( 50 * sizeof( Widget* ) );    //  Default size of 50 widgets.
                widget->childrenSize = 50;
                //  This doesn't mean everything is hidden but it causes problems if it is set to true because the root widget has nothing to render.
                widget->visible = false;
            break;
            case WT_Window :
                widget->children = byte_alloc( 10 * sizeof( Widget* ) );    //  Default size of 10 widgets per window.
                widget->childrenSize = 10;
                if( mesh && texture ) {
                    // load scaled mesh
                    str256 scaled_mesh_str;
                    str16 mesh_size;
                    snprintf( mesh_size, 16, "%d.%d", widget->size.x, widget->size.y );

                    strcpy( scaled_mesh_str, mesh );
                    strcat( scaled_mesh_str, mesh_size );

                    widget->assets.mesh = ResourceManager_get( scaled_mesh_str );
                    // if correctly sized mesh is not yet loaded, create it
                    if( -1 == widget->assets.mesh ) {
                        widget->assets.mesh = ResourceManager_get( mesh );
                        check( widget->assets.mesh >= 0, "Error while loading widget mesh. Mesh '%s' is not a loaded resource.\n", mesh );

                        // resize
                        vec2 size = vec2_vec2i( &widget->size );
                        int scaled_mesh = Renderer_createRescaledMesh( widget->assets.mesh, &size );
                        check( scaled_mesh >= 0, "Error while creating scaled mesh for widget.\n" );

                        widget->assets.mesh = scaled_mesh;

                        // add newly rescaled mesh to resource manager
                        ResourceManager_add( scaled_mesh_str, scaled_mesh );
                    }

                    // texture
                    widget->assets.texture = ResourceManager_get( texture );
                }
            break;
            case WT_WindowHead :
                widget->children = byte_alloc( 3 * sizeof( Widget* ) );     //  Default size of 3 widgets : the window, the icon and the close button.
                widget->childrenSize = 3;
                if( mesh && texture ) {
                    // load scaled mesh
                    str256 scaled_mesh_str;
                    str16 mesh_size;
                    snprintf( mesh_size, 16, "%d.%d", widget->size.x, widget->size.y );

                    strcpy( scaled_mesh_str, mesh );
                    strcat( scaled_mesh_str, mesh_size );

                    widget->assets.mesh = ResourceManager_get( scaled_mesh_str );
                    // if correctly sized mesh is not yet loaded, create it
                    if( -1 == widget->assets.mesh ) {
                        widget->assets.mesh = ResourceManager_get( mesh );
                        check( widget->assets.mesh >= 0, "Error while loading widget mesh. Mesh '%s' is not a loaded resource.\n", mesh );

                        // resize
                        vec2 size = vec2_vec2i( &widget->size );
                        int scaled_mesh = Renderer_createRescaledMesh( widget->assets.mesh, &size );
                        check( scaled_mesh >= 0, "Error while creating scaled mesh for widget.\n" );

                        widget->assets.mesh = scaled_mesh;

                        // add newly rescaled mesh to resource manager
                        ResourceManager_add( scaled_mesh_str, scaled_mesh );
                    }

                    // texture
                    widget->assets.texture = ResourceManager_get( texture );

                    if( text >= 0 )
                        widget->assets.text = text;
                }
            break;
            case WT_Sprite :
                if( mesh && texture ) {
                    // load scaled mesh
                    str256 scaled_mesh_str;
                    str16 mesh_size;
                    snprintf( mesh_size, 16, "%d.%d", widget->size.x, widget->size.y );

                    strcpy( scaled_mesh_str, mesh );
                    strcat( scaled_mesh_str, mesh_size );

                    widget->assets.mesh = ResourceManager_get( scaled_mesh_str );
                    // if correctly sized mesh is not yet loaded, create it
                    if( -1 == widget->assets.mesh ) {
                        widget->assets.mesh = ResourceManager_get( mesh );
                        check( widget->assets.mesh >= 0, "Error while loading widget mesh. Mesh '%s' is not a loaded resource.\n", mesh );

                        // resize
                        vec2 size = vec2_vec2i( &widget->size );
                        int scaled_mesh = Renderer_createRescaledMesh( widget->assets.mesh, &size );
                        check( scaled_mesh >= 0, "Error while creating scaled mesh for widget.\n" );

                        widget->assets.mesh = scaled_mesh;

                        // add newly rescaled mesh to resource manager
                        ResourceManager_add( scaled_mesh_str, scaled_mesh );
                    }

                    // texture
                    widget->assets.texture = ResourceManager_get( texture );
                }
            break;

            case WT_Text :
                if( text >= 0 ) {
                    widget->assets.text = text;
                }
            break;

            case WT_Button :
                if( mesh && texture ) {
                    // load scaled mesh
                    str256 scaled_mesh_str;
                    str16 mesh_size;
                    snprintf( mesh_size, 16, "%d.%d", widget->size.x, widget->size.y );

                    strcpy( scaled_mesh_str, mesh );
                    strcat( scaled_mesh_str, mesh_size );

                    widget->assets.mesh = ResourceManager_get( scaled_mesh_str );
                    // if correctly sized mesh is not yet loaded, create it
                    if( -1 == widget->assets.mesh ) {
                        widget->assets.mesh = ResourceManager_get( mesh );
                        check( widget->assets.mesh >= 0, "Error while loading widget mesh. Mesh '%s' is not a loaded resource.\n", mesh );

                        // resize
                        vec2 size = vec2_vec2i( &widget->size );
                        int scaled_mesh = Renderer_createRescaledMesh( widget->assets.mesh, &size );
                        check( scaled_mesh >= 0, "Error while creating scaled mesh for widget.\n" );

                        widget->assets.mesh = scaled_mesh;

                        // add newly rescaled mesh to resource manager
                        ResourceManager_add( scaled_mesh_str, scaled_mesh );
                    }

                    // texture
                    widget->assets.texture = ResourceManager_get( texture );
                }
                // text
                if( text >= 0 )
                    widget->assets.text = text;

            break;

        }
        return widget;
    }
error:
    return NULL;
}

void Widget_remove( Widget* widget ) {
    for( u32 i = 0; i < widget->childrenCount; ++i )
        Widget_remove( widget->children[i] );
    if( widget->children )
        DEL_PTR( widget->children );
    DEL_PTR( widget );
}


void Widget_addChild( Widget *master, Widget* child ) {
    if( master && child ) {
        if( child->master ) {
            log_err( "Widget already has a master, can't change it !" );
            return;
        }
        if( master->childrenCount == master->childrenSize ) {
            //  If the widget has too much children, we resize its array and add 10 more entries.
            master->children = byte_realloc( master->children, (master->childrenSize + 10) * sizeof( Widget* ) );
            master->childrenSize += 10;
        }
        master->children[master->childrenCount] = child;
        child->master = master;
        master->childrenCount++;
    }
}

void Widget_toggleShow( Widget* widget ) {
    for( u32 i = 0; i < widget->childrenCount; ++i )
        Widget_toggleShow( widget->children[i] );
    widget->visible = !(widget->visible);
}


Widget* Widget_createWindowHead( Widget* window, u32 name ) {
    Widget* icon = Widget_init( WT_Sprite, &(vec2i){ 11, 11 }, "quadmesh.json", "square.png", -1 );
    Widget* cross =  Widget_init( WT_Button, &(vec2i){ 11, 11 }, "quadmesh.json", "cross.png", -1 );
    Widget* head = Widget_init( WT_WindowHead, &(vec2i){ window->size.x, 17 }, "quadmesh.json", "widgettexture.png", name );

    head->position.x = window->position.x;
    head->position.y = window->position.y - 18;
    head->depth = window->depth;
    head->textOffset = vec2i_c( 18, 1 );

    vec2i iconOffset = vec2i_c( 3, 3 );
    icon->position = vec2i_add( &iconOffset, &head->position );
    icon->depth = window->depth - 1;

    vec2i crossOffset = vec2i_c( window->size.x - 13, 3 );
    cross->position = vec2i_add( &crossOffset, &head->position );
    cross->depth = window->depth - 1;
    cross->callback = &windowCloseButtonCallback;

    Widget_addChild( head, icon );
    Widget_addChild( head, cross );
    Widget_addChild( head, window );

    return head;
}

void Widget_update( Scene* scene, Widget* widget ) {
    if( widget->sceneIndex >= 0 ) {
        if( widget->visible == false )
            Scene_removeWidget( scene, widget );
    }
    else {
        if( widget->visible == true )
            Scene_addWidget( scene, widget );
    }
    for( u32 i = 0; i < widget->childrenCount; ++i )
        Widget_update( scene, widget->children[i] );
}

void Widget_callback( Widget* widget, const Event* e ) {
    vec2i extents = vec2i_add( &widget->position, &widget->size );
    if( e->v.x < widget->position.x || e->v.x > extents.x || e->v.y < widget->position.y || e->v.y > extents.y )
        return;
    else {
        for( u32 i = 0; i < widget->childrenCount; ++i ) {
            Widget_callback( widget->children[i], e );
        }
        if( widget->callback )
            widget->callback( widget, e );
    }
}









