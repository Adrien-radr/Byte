#ifndef BYTE_GUI_H
#define BYTE_GUI_H

#include "common/common.h"
#include "common/vector.h"
#include "client/event.h"
#include "client/context.h"

typedef struct s_Widget Widget;
typedef struct s_Scene Scene;
typedef struct s_RootWidget RootWidget;

//  Widget tree
RootWidget *root;
Widget            *windowHead,
                          *window,
                      *button,
                      *openWindow;

typedef bool (*WidgetCallback)(Widget*, const Event*);

void rootMouseListener( const Event* event, void* root );
void rootKeyListener( const Event* event, void* root );

//  General function for close buttons on windows.
bool windowCloseButtonCallback( Widget* widget, const Event* e );

bool button1Callback( Widget* widget, const Event* e );

//  General functions for resizing of windows in every direction.
bool moveWindowCallback( Widget* widget, const Event* e );
bool topResizeCallback( Widget* widget, const Event* e );
bool leftResizeCallback( Widget* widget, const Event* e );
bool rightResizeCallback( Widget* widget, const Event* e );
bool bottomResizeCallback( Widget* widget, const Event* e );
bool topLeftResizeCallback( Widget* widget, const Event* e );
bool topRightResizeCallback( Widget* widget, const Event* e );
bool bottomLeftResizeCallback( Widget* widget, const Event* e );
bool bottomRightResizeCallback( Widget* widget, const Event* e );
bool openWindowCallback( Widget* widget, const Event* e );

typedef enum {
    WO_Window = 0,
    WO_Outline = 1,
        WO_Top = 0,
        WO_Left = 1,
        WO_Right = 2,
        WO_Bottom = 3,
        WO_TopLeft = 4,
        WO_TopRight = 5,
        WO_BottomLeft = 6,
        WO_BottomRight = 7,
    WO_Icon = 2,
    WO_Cross = 3
}   WindowOrganization;

typedef enum {
    WT_Root,
    WT_Window,
    WT_WindowHead,
    WT_Sprite,
    WT_Text,
    WT_Button,
}   WidgetType;

typedef enum {
    WA_None,
    WA_TopLeft,
    WA_TopCenter,
    WA_TopRight,
    WA_CenterLeft,
    WA_Center,
    WA_CenterRight,
    WA_BottomLeft,
    WA_BottomCenter,
    WA_BottomRight
}   WidgetAnchor;

typedef struct s_Widget {
    bool                visible;
    vec2i               position;
    vec2i               size;
    vec2i               minSize;    //  A window won't be able to be resized less than this size.
    vec2i               textOffset; //  Position of the text compared to the texture's.
    vec2               scale;      //  Used when the widget is resized.

    bool resized;
    bool moved;
    bool confined;  //  Will the widget always be inside its parent ?
    WidgetAnchor anchor;

    int                 depth;
    int                 sceneIndex; //  Index set when this widget is to be rendered. -1 when it's not.

    WidgetCallback callback;
    vec2i mouseOffset;  //  This value is used when a widget must be resized or moved

    struct s_Widget        *parent;
    struct s_Widget       **children;
    u32                 childrenCount;
    u32                 childrenSize;

    struct {
        u32 mesh;
        u32 texture;
        u32 text;
    }                   assets;
} Widget;

typedef struct s_RootWidget {
    Widget* widget;
    ListenerFunc mouseListener;
    ListenerFunc keyListener;
    bool mouseDown;
}   RootWidget;



/// Initialize the given widget (build the mesh);
RootWidget* RootWidget_init();
Widget* Widget_init( WidgetType type, vec2i* size, const char *mesh, const char *texture, int text );
/// Create a head bar for the window, you must give the window widget and its name as parameters.
Widget* Widget_createWindowHead( Widget* window, u32 name );
void Widget_update( Scene* scene, Widget* widget );
bool Widget_callback( Widget* widget, const Event* e );
void Widget_setPosition( Widget* widget, vec2i* pos );
void Widget_resize( Widget* widget, vec2i* size );
void Widget_addChild( Widget *parent, Widget* child, bool confined );
void Widget_toggleShow( Widget* widget );
bool Widget_mouseOver( const Widget* widget, const vec2i* mouse );

/// Will be called on RootWidget when closing game to remove every widget.
void Widget_remove( Widget* widget );

void Window_resizeRight( Widget* widget, int size );
void Window_resizeLeft( Widget* widget, int size );
void Window_resizeUp( Widget* widget, int size );
void Window_resizeDown( Widget* widget, int size );
vec2i Window_minSize( Widget* widget );

#endif /* BYTE_GUI */
