#ifndef BYTE_GUI_H
#define BYTE_GUI_H

#include "common/common.h"
#include "common/vector.h"
#include "client/event.h"

typedef struct s_Widget Widget;
typedef struct s_Scene Scene;

typedef void (*WidgetCallback)(Widget*, const Event*);


typedef enum {
    WT_Root,
    WT_Window,
    WT_WindowHead,
    WT_Sprite,
    WT_Text,
    WT_Button
}   WidgetType;

typedef struct s_Widget {
    bool                visible;
    vec2i               position;
    vec2i               size;
    vec2i               textOffset; //  Position of the text compared to the texture's.
    int                 depth;
    int                 sceneIndex; //  Index set when this widget is to be rendered. -1 when it's not.

    WidgetCallback callback;

    struct s_Widget        *master;
    struct s_Widget       **children;
    u32                 childrenCount;
    u32                 childrenSize;

    struct {
        u32 mesh;
        u32 texture;
        u32 text;
    }                   assets;
} Widget;


Widget* RootWidget;
//  General function for close buttons on windows.
void windowCloseButton( Widget* widget, const Event* e );


/// Initialize the given widget (build the mesh);
Widget* Widget_init( WidgetType type, vec2i* size, const char *mesh, const char *texture, int text );
/// Create a head bar for the window, you must give the window widget and its name as parameters.
Widget* Widget_createWindowHead( Widget* window, u32 name );
void Widget_update( Scene* scene, Widget* widget );
void Widget_callback( Widget* widget, const Event* e );
void Widget_addChild( Widget *master, Widget* child );
/// Will be called on RootWidget to remove every widget.
void Widget_remove( Widget* widget );

#endif /* BYTE_GUI */
