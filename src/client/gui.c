#include "gui.h"
#include "game.h"
#include "resource.h"
#include "renderer.h"
#include "scene.h"

////////////////////////////////////////////////////////
////    Some callback functions
bool windowCloseButtonCallback( Widget* widget, const Event* e ) {
    if( e->type == EMouseReleased ){
        if( Widget_mouseOver( widget, &e->v ) ) {
            widget->parent->visible = false;
            printf( "LE CHEVAL EST DANS L'ECURIE\n" );
            return true;
        }
    }
    return false;
}

bool button1Callback( Widget* widget, const Event* e ) {
    if( e->type == EMouseReleased )
        if( Widget_mouseOver( widget, &e->v ) ){
            Widget_toggleShow( windowHead );
            return true;
        }
    return false;
}

bool moveWindowCallback( Widget* widget, const Event* e ) {
    if( e->type == EMousePressed ) {
        if( Widget_mouseOver( widget, &e->v ) ) {
            printf( "cheval" );
            vec2i offset = vec2i_sub( &widget->position, &e->v );
            vec2i pos = vec2i_add( &e->v, &offset );
            pos.x += 5;
            Widget_setPosition( widget, &pos );
            return true;
        }
    }
    return false;
}

bool rightResizeCallback( Widget* widget, const Event* e ) {
    if( e->type == EMousePressed ) {
        if( Widget_mouseOver( widget, &e->v ) ) {
            int offset = e->v.x - widget->position.x;
            int size = e->v.x - offset - widget->position.x;
            vec2i newSize = vec2i_c( size, 1 );
            Widget_resize( widget, &newSize );
            return true;
        }
    }
    return false;
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
        widget->parent = NULL;
        widget->callback = NULL;
        widget->visible = true;
        widget->size.x = size->x;
        widget->size.y = size->y;
        widget->resized = false;
        widget->scale = vec2_c( 1.f, 1.f );
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
                widget->minSize = vec2i_c( 50, 50 );
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


void Widget_addChild( Widget *parent, Widget* child ) {
    if( parent && child ) {
        if( child->parent ) {
            log_err( "Widget already has a parent, can't change it !" );
            return;
        }
        if( parent->childrenCount == parent->childrenSize ) {
            //  If the widget has too much children, we resize its array and add 10 more entries.
            parent->children = byte_realloc( parent->children, (parent->childrenSize + 10) * sizeof( Widget* ) );
            parent->childrenSize += 10;
        }
        parent->children[parent->childrenCount] = child;
        child->parent = parent;
        parent->childrenCount++;
    }
}

void Widget_toggleShow( Widget* widget ) {
    for( u32 i = 0; i < widget->childrenCount; ++i )
        Widget_toggleShow( widget->children[i] );
    widget->visible = !(widget->visible);
}

bool Widget_mouseOver( const Widget* widget, const vec2i* mouse ) {
    vec2i extents = vec2i_add( &widget->position, &widget->size );
    if( mouse->x < widget->position.x || mouse->x > extents.x || mouse->y < widget->position.y || mouse->y > extents.y )
        return false;
    return true;
}

Widget* Widget_createWindowHead( Widget* window, u32 name ) {
    Widget* head = Widget_init( WT_WindowHead, &(vec2i){ window->size.x, 17 }, "quadmesh.json", "widgettexture.png", name );
        Widget* icon = Widget_init( WT_Sprite, &(vec2i){ 11, 11 }, "quadmesh.json", "square.png", -1 );
        Widget* cross =  Widget_init( WT_Button, &(vec2i){ 11, 11 }, "quadmesh.json", "cross.png", -1 );
        //  The window contour is composed of 9 rectangles the main one, the ones on the side and the ones in the corners.
        Widget* windowOutline = Widget_init( WT_Button, &(vec2i){ window->size.x + 10, window->size.y + head->size.y + 11 }, "quadmesh.json", "widgetcontour.png", -1 );
            //  Corners of the contour
            Widget* windowCornerTopLeft = Widget_init( WT_Button, &(vec2i){ 5, 5 }, "quadmesh.json", "right.png", -1 );
            Widget* windowCornerTopRight = Widget_init( WT_Button, &(vec2i){ 5, 5 }, "quadmesh.json", "bottom.png", -1 );
            Widget* windowCornerBottomLeft = Widget_init( WT_Button, &(vec2i){ 5, 5 }, "quadmesh.json", "top.png", -1 );
            Widget* windowCornerBottomRight = Widget_init( WT_Button, &(vec2i){ 5, 5 }, "quadmesh.json", "left.png", -1 );
            //  Sides of the contour
            Widget* windowSideTop = Widget_init( WT_Button, &(vec2i){ window->size.x, 5 }, "quadmesh.json", "top.png", -1 );
            Widget* windowSideLeft = Widget_init( WT_Button, &(vec2i){ 5, window->size.y + head->size.y + 1}, "quadmesh.json", "left.png", -1 );
            Widget* windowSideRight = Widget_init( WT_Button, &(vec2i){ 5, window->size.y + head->size.y + 1 }, "quadmesh.json", "right.png", -1 );
            Widget* windowSideBottom = Widget_init( WT_Button, &(vec2i){ window->size.x, 5 }, "quadmesh.json", "bottom.png", -1 );


    head->position.x = window->position.x;
    head->position.y = window->position.y - 18;
    head->depth = window->depth;
    head->textOffset = vec2i_c( 18, 1 );
    head->callback = &moveWindowCallback;


    vec2i windowExtents = vec2i_add( &window->position, &window->size );

    windowOutline->position.x = head->position.x - 5;
    windowOutline->position.y = head->position.y - 5;
    windowOutline->depth = window->depth + 1;

    windowCornerTopLeft->position.x = head->position.x - 5;
    windowCornerTopLeft->position.y = head->position.y - 5;
    windowCornerTopLeft->depth = windowOutline->depth + 1;

    windowCornerTopRight->position.x = windowExtents.x;
    windowCornerTopRight->position.y = head->position.y - 5;
    windowCornerTopRight->depth = windowOutline->depth + 1;

    windowCornerBottomLeft->position.x = window->position.x - 5;
    windowCornerBottomLeft->position.y = windowExtents.y;
    windowCornerBottomLeft->depth = windowOutline->depth + 1;

    windowCornerBottomRight->position.x = windowExtents.x;
    windowCornerBottomRight->position.y = windowExtents.y;
    windowCornerBottomRight->depth = windowOutline->depth + 1;

    windowSideTop->position.x = window->position.x;
    windowSideTop->position.y = head->position.y - 5;
    windowSideTop->depth = windowOutline->depth + 1;

    windowSideLeft->position.x = window->position.x - 5;
    windowSideLeft->position.y = head->position.y;
    windowSideLeft->depth = windowOutline->depth + 1;

    windowSideRight->position.x = windowExtents.x;
    windowSideRight->position.y = head->position.y;
    windowSideRight->depth = windowOutline->depth + 1;
    windowSideRight->callback = &rightResizeCallback;

    windowSideBottom->position.x = window->position.x;
    windowSideBottom->position.y = windowExtents.y;
    windowSideBottom->depth = windowOutline->depth;

    Widget_addChild( head, window );
    Widget_addChild( head, windowOutline );
    Widget_addChild( windowOutline, windowSideTop );
    Widget_addChild( windowOutline, windowSideLeft );
    Widget_addChild( windowOutline, windowSideRight );
    Widget_addChild( windowOutline, windowSideBottom );
    Widget_addChild( windowOutline, windowCornerTopLeft );
    Widget_addChild( windowOutline, windowCornerTopRight );
    Widget_addChild( windowOutline, windowCornerBottomLeft );
    Widget_addChild( windowOutline, windowCornerBottomRight );



    vec2i iconOffset = vec2i_c( 3, 3 );
    icon->position = vec2i_add( &iconOffset, &head->position );
    icon->depth = window->depth - 1;

    vec2i crossOffset = vec2i_c( window->size.x - 13, 3 );
    cross->position = vec2i_add( &crossOffset, &head->position );
    cross->depth = window->depth - 1;
    cross->callback = &windowCloseButtonCallback;

    Widget_addChild( head, icon );
    Widget_addChild( head, cross );


    return head;
}

void Widget_update( Scene* scene, Widget* widget ) {
    if( widget->sceneIndex >= 0 ) {
        if( widget->visible == false )
            Scene_removeWidget( scene, widget );
        //  For windows : if it has been resized, update all his children. //TODO BEULARD
        if( widget->resized )
            Scene_modifyWidget( scene, widget->sceneIndex, WA_Scale, &widget->scale );
        if( widget->moved )
            Scene_modifyWidget( scene, widget->sceneIndex, WA_Position, &widget->position );
    }
    else {
        if( widget->visible == true )
            Scene_addWidget( scene, widget );
    }
    for( u32 i = 0; i < widget->childrenCount; ++i )
        Widget_update( scene, widget->children[i] );
}

bool Widget_callback( Widget* widget, const Event* e ) {
    bool ret = false;
    for( u32 i = 0; i < widget->childrenCount; ++i ) {
        if( Widget_callback( widget->children[i], e ) )
            ret = true;
    }
    if( widget->sceneIndex >= 0 && widget->callback && !ret ) {
        if( widget->callback( widget, e ) )
            ret = true;
    }
    return ret;
}

void Widget_setPosition( Widget* widget, vec2i* pos ) {
    vec2i originalPos =  widget->position;
    vec2i offSet = vec2i_sub( pos, &originalPos );
    widget->position = *pos;
    widget->moved = true;
    for( u32 i = 0; i < widget->childrenCount; ++i ) {
        vec2i newPos = vec2i_add( &widget->children[i]->position, &offSet );
        Widget_setPosition( widget->children[i], &newPos );
    }
}

void Widget_resize( Widget* widget, vec2i* size ) {
    widget->scale.x = size->x / widget->size.x;
    widget->scale.y = size->y / widget->size.y;
    widget->resized = true;
}


void WidgetHead_reorganizeHorizontal( Widget* widget, Scene* scene ) {

}

void WidgetHead_reorganizeVertical( Widget* widget, Scene* scene ) {

}









