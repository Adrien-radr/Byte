#include "gui.h"
#include "game.h"
#include "resource.h"
#include "render/renderer.h"
#include "render/scene.h"


//  Root widget listener functions
void rootMouseListener( const Event* event, void* root ) {
    RootWidget* r = (RootWidget*)root;
    if( event->type == EMousePressed )
        r->mouseDown = true;
    if( event->type == EMouseReleased )
        r->mouseDown = false;
}

void rootKeyListener( const Event* event, void* root ) {

}


////////////////////////////////////////////////////////
////    Some callback functions
bool windowCloseButtonCallback( Widget* widget, const Event* e ) {
    if( e->type == EMousePressed ){
        if( Widget_mouseOver( widget, &e->v ) ) {
            widget->parent->visible = false;
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
    if( Widget_mouseOver( widget, &e->v ) ) {
        if( e->type == EMousePressed )
            widget->mouseOffset = vec2i_sub( &widget->position, &e->v );
    }
    if( root->mouseDown && widget->mouseOffset.x != -1 ) {
        vec2i pos = vec2i_add( &e->v, &widget->mouseOffset );
        Widget_setPosition( widget, &pos );
        vec2i win = pos;
        win.y += 18;
        Widget_setPosition( widget->children[WO_Window], &win );

        vec2i out = pos;
        out.x -= 5;
        out.y -= 5;
        Widget_setPosition( widget->children[WO_Outline], &out );

        return true;
    }
    else
        widget->mouseOffset = vec2i_c( -1, -1 );

    return false;
}

bool rightResizeCallback( Widget* widget, const Event* e ) {
    if( Widget_mouseOver( widget, &e->v ) ) {
        if( e->type == EMousePressed )
            widget->mouseOffset.x =  e->v.x - widget->position.x;
    }
    if( root->mouseDown && widget->mouseOffset.x != -1 ) {
        int right = e->v.x - widget->mouseOffset.x;

        int size = right - widget->parent->parent->position.x;
        Window_resizeRight( widget->parent->parent->children[WO_Window], size );
        return true;
    }
    else
        widget->mouseOffset.x = -1;

    return false;
}

bool bottomResizeCallback( Widget* widget, const Event* e ) {
    if( Widget_mouseOver( widget, &e->v ) ) {
        if( e->type == EMousePressed )
            widget->mouseOffset.y = e->v.y - widget->position.y;
    }
    if( root->mouseDown && widget->mouseOffset.y != -1 ) {
        int bottom = e->v.y - widget->mouseOffset.y;

        int size = bottom - widget->parent->parent->children[WO_Window]->position.y;
        Window_resizeDown( widget->parent->parent->children[WO_Window], size );
        return true;
    }
    else
        widget->mouseOffset.y = -1;

    return false;
}

bool leftResizeCallback( Widget* widget, const Event* e ) {
    if( Widget_mouseOver( widget, &e->v ) ) {
        if( e->type == EMousePressed )
            widget->mouseOffset.x = widget->position.x - e->v.x;
    }
    if( root->mouseDown && widget->mouseOffset.x != 1 ) {
        int left = e->v.x + 5 + widget->mouseOffset.x;

        int size = widget->parent->parent->children[WO_Window]->position.x + widget->parent->parent->children[WO_Window]->size.x - left;
        Window_resizeLeft( widget->parent->parent->children[WO_Window], size );
        return true;
    }
    else
        widget->mouseOffset.x = 1;

    return false;
}

bool topResizeCallback( Widget* widget, const Event* e ) {
    if( Widget_mouseOver( widget, &e->v ) ) {
        if( e->type == EMousePressed )
            widget->mouseOffset.y = widget->position.y - e->v.y;
    }
    if( root->mouseDown && widget->mouseOffset.y != 1 ) {
        int top = e->v.y + 5 + widget->mouseOffset.y + 18;

        int size = widget->parent->parent->children[WO_Window]->position.y + widget->parent->parent->children[WO_Window]->size.y - top;
        Window_resizeUp( widget->parent->parent->children[WO_Window], size );
        return true;
    }
    else
        widget->mouseOffset.y = 1;

    return false;
}

bool bottomRightResizeCallback( Widget* widget, const Event* e ) {
    if( Widget_mouseOver( widget, &e->v ) ) {
        if( e->type == EMousePressed )
            widget->mouseOffset = vec2i_sub( &e->v, &widget->position );
    }
    if( root->mouseDown && widget->mouseOffset.x != -1 ) {
        vec2i bottomright = vec2i_sub( &e->v, &widget->mouseOffset );

        vec2i size = vec2i_sub( &bottomright, &widget->parent->parent->children[WO_Window]->position );
        Window_resizeRight( widget->parent->parent->children[WO_Window], size.x );
        Window_resizeDown( widget->parent->parent->children[WO_Window], size.y );
        return true;
    }
    else
        widget->mouseOffset = vec2i_c( -1, -1 );

    return false;
}

bool topRightResizeCallback( Widget* widget, const Event* e ) {
    if( Widget_mouseOver( widget, &e->v ) ) {
        if( e->type == EMousePressed ) {
            widget->mouseOffset.x = e->v.x - widget->position.x;
            widget->mouseOffset.y = widget->position.y - e->v.y;
        }
    }
    if( root->mouseDown && widget->mouseOffset.x != -1 ) {
        vec2i topright;
        topright.x = e->v.x - widget->mouseOffset.x;
        topright.y = e->v.y + 5 + widget->mouseOffset.y + 18;

        vec2i size;
        size.x = topright.x - widget->parent->parent->position.x;
        size.y = widget->parent->parent->children[WO_Window]->position.y + widget->parent->parent->children[WO_Window]->size.y - topright.y;
        Window_resizeRight( widget->parent->parent->children[WO_Window], size.x );
        Window_resizeUp( widget->parent->parent->children[WO_Window], size.y );
        return true;
    }
    else
        widget->mouseOffset = vec2i_c( -1, 1 );

    return false;
}

bool topLeftResizeCallback( Widget* widget, const Event* e ) {
    if( Widget_mouseOver( widget, &e->v ) ) {
        if( e->type == EMousePressed ) {
            widget->mouseOffset = vec2i_sub( &widget->position, &e->v );
        }
    }
    if( root->mouseDown && widget->mouseOffset.x != 1 ) {
        vec2i topleft = vec2i_add( &e->v, &widget->mouseOffset );
        topleft.x += 5;
        topleft.y += 23;

        vec2i bottomright = vec2i_add( &widget->parent->parent->children[WO_Window]->position, &widget->parent->parent->children[WO_Window]->size );
        vec2i size = vec2i_sub( &bottomright, &topleft );
        Window_resizeLeft( widget->parent->parent->children[WO_Window], size.x );
        Window_resizeUp( widget->parent->parent->children[WO_Window], size.y );
        return true;
    }
    else
        widget->mouseOffset = vec2i_c( 1, 1 );

    return false;
}

bool bottomLeftResizeCallback( Widget* widget, const Event* e ) {
    if( Widget_mouseOver( widget, &e->v ) ) {
        if( e->type == EMousePressed ) {
            widget->mouseOffset.x = widget->position.x - e->v.x;
            widget->mouseOffset.y = e->v.y - widget->position.y;
        }
    }
    if( root->mouseDown && widget->mouseOffset.x != 1 ) {
        vec2i bottomleft;
        bottomleft.x = e->v.x + widget->mouseOffset.x + 5;
        bottomleft.y = e->v.y - widget->mouseOffset.y;

        vec2i size;
        size.x = widget->parent->parent->children[WO_Window]->position.x + widget->parent->parent->children[WO_Window]->size.x - bottomleft.x;
        size.y = bottomleft.y - widget->parent->parent->children[WO_Window]->position.y;
        Window_resizeLeft( widget->parent->parent->children[WO_Window], size.x );
        Window_resizeDown( widget->parent->parent->children[WO_Window], size.y );
        return true;
    }
    else
        widget->mouseOffset = vec2i_c( 1, -1 );

    return false;
}

bool openWindowCallback( Widget* widget, const Event* e ) {
    if( Widget_mouseOver( widget, &e->v ) ) {
        if( e->type == EMousePressed ){
            Widget_toggleShow( windowHead );
            return true;
        }
    }
    return false;
}


//////////////////////////////////////////////////////
////    Widget functions
RootWidget* RootWidget_init() {
    check( game, "Can't init widget without running game.\n" );
    RootWidget* rootWidget = byte_alloc( sizeof( RootWidget ) );
    if( rootWidget ) {
        rootWidget->keyListener = rootKeyListener;
        rootWidget->mouseListener = rootMouseListener;
        rootWidget->mouseDown = false;
        EventManager_addListener( LT_MouseListener, rootWidget->mouseListener, rootWidget );
        EventManager_addListener( LT_KeyListener, rootWidget->keyListener, rootWidget );
        vec2i size = Context_getSize();
        rootWidget->widget = Widget_init( WT_Root, &size, NULL, NULL, -1 );
    }
    return rootWidget;
error :
    return NULL;
}


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
        widget->moved = false;
        widget->confined = false;
        widget->anchor = WA_None;
        widget->scale = vec2_c( 1.f, 1.f );
        widget->mouseOffset = vec2i_c( 0, 0 );
        switch( type ) {
            case WT_Root :
                widget->children = byte_alloc( 50 * sizeof( Widget* ) );    //  Default size of 50 widgets.
                widget->childrenSize = 50;
                widget->position = vec2i_c( 0, 0 );

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
                        int scaled_mesh = Renderer_createRescaledMesh( widget->assets.mesh, &size, &(vec2){ 1, 1 } );
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
                        int scaled_mesh = Renderer_createRescaledMesh( widget->assets.mesh, &size, &(vec2){ 1, 1 } );
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
                        int scaled_mesh = Renderer_createRescaledMesh( widget->assets.mesh, &size, &(vec2){ 1, 1 } );
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
                        int scaled_mesh = Renderer_createRescaledMesh( widget->assets.mesh, &size, &(vec2){ 1, 1 } );
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


void Widget_addChild( Widget *parent, Widget* child, bool confine ) {
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

        if( confine )
            child->confined = true;

        parent->minSize = Window_minSize( parent );
        if( parent->size.x < parent->minSize.x )
            Window_resizeRight( parent, parent->minSize.x );

        if( parent->size.y < parent->minSize.y )
            Window_resizeDown( parent, parent->minSize.y );


        if( confine ) {
            child->depth = parent->depth - 1;

            if( child->position.x < parent->position.x )
                Widget_setPosition( child, &(vec2i){ parent->position.x, child->position.y } );
            if( child->position.y < parent->position.y )
                Widget_setPosition( child, &(vec2i){ child->position.x, parent->position.y } );
            if( child->position.x + child->size.x > parent->position.x + parent->size.x )
                Widget_setPosition( child, &(vec2i){ parent->position.x + parent->size.x - child->size.x, child->position.y } );
            if( child->position.y + child->size.y > parent->position.y + parent->size.y )
                Widget_setPosition( child, &(vec2i){ child->position.x, parent->position.y + parent->size.y - child->size.y } );
        }



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
        Widget* icon = Widget_init( WT_Sprite, &(vec2i){ 11, 11 }, "quadmesh.json", "winicon.png", -1 );
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
    windowCornerTopLeft->depth = windowOutline->depth - 1;
    windowCornerTopLeft->callback = &topLeftResizeCallback;

    windowCornerTopRight->position.x = windowExtents.x;
    windowCornerTopRight->position.y = head->position.y - 5;
    windowCornerTopRight->depth = windowOutline->depth - 1;
    windowCornerTopRight->callback = &topRightResizeCallback;

    windowCornerBottomLeft->position.x = window->position.x - 5;
    windowCornerBottomLeft->position.y = windowExtents.y;
    windowCornerBottomLeft->depth = windowOutline->depth - 1;
    windowCornerBottomLeft->callback = &bottomLeftResizeCallback;

    windowCornerBottomRight->position.x = windowExtents.x;
    windowCornerBottomRight->position.y = windowExtents.y;
    windowCornerBottomRight->depth = windowOutline->depth - 1;
    windowCornerBottomRight->callback = &bottomRightResizeCallback;

    windowSideTop->position.x = window->position.x;
    windowSideTop->position.y = head->position.y - 5;
    windowSideTop->depth = windowOutline->depth - 1;
    windowSideTop->callback = &topResizeCallback;

    windowSideLeft->position.x = window->position.x - 5;
    windowSideLeft->position.y = head->position.y;
    windowSideLeft->depth = windowOutline->depth - 1;
    windowSideLeft->callback = &leftResizeCallback;

    windowSideRight->position.x = windowExtents.x;
    windowSideRight->position.y = head->position.y;
    windowSideRight->depth = windowOutline->depth - 1;
    windowSideRight->callback = &rightResizeCallback;

    windowSideBottom->position.x = window->position.x;
    windowSideBottom->position.y = windowExtents.y;
    windowSideBottom->depth = windowOutline->depth - 1;
    windowSideBottom->callback = &bottomResizeCallback;

    Widget_addChild( head, window, false );
    Widget_addChild( head, windowOutline, false );
    Widget_addChild( windowOutline, windowSideTop, true );
    Widget_addChild( windowOutline, windowSideLeft, true );
    Widget_addChild( windowOutline, windowSideRight, true );
    Widget_addChild( windowOutline, windowSideBottom, true );
    Widget_addChild( windowOutline, windowCornerTopLeft, true );
    Widget_addChild( windowOutline, windowCornerTopRight, true );
    Widget_addChild( windowOutline, windowCornerBottomLeft, true );
    Widget_addChild( windowOutline, windowCornerBottomRight, true );



    vec2i iconOffset = vec2i_c( 3, 3 );
    icon->position = vec2i_add( &iconOffset, &head->position );
    icon->depth = window->depth - 1;

    vec2i crossOffset = vec2i_c( window->size.x - 13, 3 );
    cross->position = vec2i_add( &crossOffset, &head->position );
    cross->depth = window->depth - 1;
    cross->callback = &windowCloseButtonCallback;

    Widget_addChild( head, icon, true );
    Widget_addChild( head, cross, true );


    return head;
}

void Widget_update( Scene* scene, Widget* widget ) {
    if( widget->sceneIndex >= 0 ) {
        if( widget->visible == false )
            Scene_removeWidget( scene, widget );
        //  For windows : if it has been resized, update all his children. //TODO BEULARD
        if( widget->resized ) {
            Scene_modifyWidget( scene, widget->sceneIndex, WA_Scale, &widget->scale );
            widget->resized = false;
        }
        if( widget->moved ){
            for( u32 i = 0; i< widget->childrenCount; ++i )
                widget->children[i]->moved = true;

            Scene_modifyWidget( scene, widget->sceneIndex, WA_Position, &widget->position );
            widget->moved = false;
        }
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
    //  TODO FIX
    widget->position = *pos;
    widget->moved = true;
    if( widget->confined ) {
        if( pos->x < widget->parent->position.x )
            widget->position.x = widget->parent->position.x;
        if( pos->y < widget->parent->position.y )
            widget->position.y = widget->parent->position.y;
        if( pos->x + widget->size.x > widget->parent->position.x + widget->parent->size.x )
            widget->position.x = widget->parent->position.x + widget->parent->size.x - widget->size.x;
        if( pos->y + widget->size.y > widget->parent->position.y + widget->parent->size.y )
            widget->position.y = widget->parent->position.y + widget->parent->size.y - widget->size.y;
    }

    vec2i offSet = vec2i_sub( pos, &originalPos );

    for( u32 i = 0; i < widget->childrenCount; ++i ) {
        if( widget->children[i]->confined ){
            vec2i newPos = vec2i_add( &widget->children[i]->position, &offSet );
            Widget_setPosition( widget->children[i], &newPos );
        }
    }
}

void Widget_resize( Widget* widget, vec2i* size ) {
    size->x == 0 ? size->x = 1 : 1;
    size->y == 0 ? size->y = 1 : 1;
    float scaleX = (float)size->x / (float)widget->size.x;
    float scaleY = (float)size->y / (float)widget->size.y;
    widget->size = *size;

    widget->scale.x *= scaleX;
    widget->scale.y *= scaleY;
    widget->resized = true;
}

void Window_resizeRight( Widget* widget, int size ) {
    if( size < widget->minSize.x )
        size = widget->minSize.x;

    Widget* head = widget->parent;
    Widget* outline = widget->parent->children[WO_Outline];
    Widget_resize( head, &(vec2i){ size, 17 } );
    Widget_resize( outline, &(vec2i){ size + 10, widget->size.y + head->size.y + 11 } );
    Widget_resize( outline->children[WO_Bottom], &(vec2i){ size, 5 } );
    Widget_resize( outline->children[WO_Top], &(vec2i){ size, 5 } );


    Widget_setPosition( outline->children[WO_Right], &(vec2i){ widget->position.x + size, outline->position.y + 5 } );
    Widget_setPosition( outline->children[WO_TopRight], &(vec2i){ widget->position.x + size, outline->position.y } );
    Widget_setPosition( outline->children[WO_BottomRight], &(vec2i){ widget->position.x + size, outline->position.y + outline->size.y - 5 } );
    Widget_setPosition( head->children[WO_Cross], &(vec2i){ head->position.x + head->size.x - 13, head->position.y + 3 } );

    int offSet = size - widget->size.x;
    Widget_resize( widget, &(vec2i){ size, widget->size.y } );

    for( u32 i = 0; i < widget->childrenCount; ++i )
        Widget_setPosition( widget->children[i], &(vec2i){ widget->children[i]->position.x + offSet, widget->children[i]->position.y } );


}

void Window_resizeDown( Widget* widget, int size ) {
    if( size < widget->minSize.y )
        size = widget->minSize.y;

    Widget* outline = widget->parent->children[WO_Outline];
    Widget_resize( outline, &(vec2i){ widget->size.x + 10, size + 28 } );
    Widget_resize( outline->children[WO_Right], &(vec2i){ 5, size + 18 } );
    Widget_resize( outline->children[WO_Left], &(vec2i){ 5, size + 18 } );


    Widget_setPosition( outline->children[WO_Bottom], &(vec2i){ outline->position.x + 5, widget->position.y + size } );
    Widget_setPosition( outline->children[WO_BottomLeft], &(vec2i){ outline->position.x, widget->position.y + size } );
    Widget_setPosition( outline->children[WO_BottomRight], &(vec2i){ outline->position.x + outline->size.x - 5, widget->position.y + size } );

    int offSet = size - widget->size.y;
    Widget_resize( widget, &(vec2i){ widget->size.x, size } );

    for( u32 i = 0; i < widget->childrenCount; ++i )
        Widget_setPosition( widget->children[i], &(vec2i){ widget->children[i]->position.x, widget->children[i]->position.y + offSet } );
}

void Window_resizeLeft( Widget* widget, int size ) {
    if( size < widget->minSize.x )
        size = widget->minSize.x;

    Widget* head = widget->parent;
    Widget* outline = widget->parent->children[WO_Outline];
    Widget_resize( head, &(vec2i){ size, 17 } );
    Widget_setPosition( head, &(vec2i){ widget->position.x + widget->size.x - size, widget->position.y - 18 } );
    Widget_resize( outline, &(vec2i){ size + 10, widget->size.y + head->size.y + 11 } );
    Widget_setPosition( outline, &(vec2i){ head->position.x - 5, head->position.y - 5 } );
    Widget_resize( outline->children[WO_Bottom], &(vec2i){ size, 5 } );
    Widget_setPosition( outline->children[WO_Bottom], &(vec2i){ head->position.x, widget->position.y + widget->size.y } );
    Widget_resize( outline->children[WO_Top], &(vec2i){ size, 5 } );
    Widget_setPosition( outline->children[WO_Top], &(vec2i){ head->position.x, head->position.y - 5 } );


    Widget_setPosition( outline->children[WO_TopRight], &(vec2i){ outline->position.x + outline->size.x - 5, outline->position.y } );
    Widget_setPosition( outline->children[WO_Right], &(vec2i){ outline->position.x + outline->size.x - 5, outline->position.y + 5 } );
    Widget_setPosition( outline->children[WO_BottomRight], &(vec2i){ outline->position.x + outline->size.x - 5, outline->position.y + outline->size.y - 5 } );
    Widget_setPosition( head->children[WO_Cross], &(vec2i){ head->position.x + head->size.x - 13, head->position.y + 3 } );
    Widget_setPosition( head->children[WO_Icon], &(vec2i){ head->position.x + 3, head->position.y + 3 } );

    Widget_resize( widget, &(vec2i){ size, widget->size.y } );
    Widget_setPosition( widget, &(vec2i){ head->position.x, head->position.y + 18 } );
}

void Window_resizeUp( Widget* widget, int size ) {
    if( size < widget->minSize.y )
        size = widget->minSize.y;

    Widget* head = widget->parent;
    Widget* outline = widget->parent->children[WO_Outline];
    Widget_setPosition( head, &(vec2i){ widget->position.x, widget->position.y + widget->size.y - size - 18  } );

    Widget_resize( outline, &(vec2i){ widget->size.x + 10, head->size.y + 11 + size } );
    Widget_setPosition( outline, &(vec2i){ head->position.x - 5, head->position.y - 5 } );
    Widget_resize( outline->children[WO_Left], &(vec2i){ 5, size + 18 } );
    Widget_setPosition( outline->children[WO_Left], &(vec2i){ outline->position.x, outline->position.y + 5 } );
    Widget_resize( outline->children[WO_Right], &(vec2i){ 5, size + 18 } );
    Widget_setPosition( outline->children[WO_Right], &(vec2i){ outline->position.x + outline->size.x - 5, outline->position.y + 5 } );


    Widget_setPosition( head->children[WO_Cross], &(vec2i){ head->position.x + head->size.x - 13, head->position.y + 3 } );
    Widget_setPosition( head->children[WO_Icon], &(vec2i){ head->position.x + 3, head->position.y + 3 } );
    Widget_setPosition( outline->children[WO_Bottom], &(vec2i){ outline->position.x + 5, outline->position.y + outline->size.y - 5 } );
    Widget_setPosition( outline->children[WO_BottomLeft], &(vec2i){ outline->position.x, outline->position.y + outline->size.y - 5 } );
    Widget_setPosition( outline->children[WO_BottomRight], &(vec2i){ outline->position.x + outline->size.x - 5, outline->position.y + outline->size.y - 5 } );

    Widget_resize( widget, &(vec2i){ widget->size.x, size } );
    Widget_setPosition( widget, &(vec2i){ head->position.x, head->position.y + 18 } );
}

vec2i Window_minSize( Widget* widget ) {
    vec2i minSize = vec2i_c( 100, 1 );
    for( u32 i = 0; i < widget->childrenCount; ++i ) {
        if( widget->children[i]->confined ) {
            if( widget->children[i]->size.x > minSize.x )
                minSize.x = widget->children[i]->size.x;
            if( widget->children[i]->size.y > minSize.y )
                minSize.y = widget->children[i]->size.y;
        }
    }
    return minSize;
}


