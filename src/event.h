#ifndef BYTE_EVENT_HPP
#define BYTE_EVENT_HPP

#include "common/common.h"
#include "common/vector.h"
#include "keys.h"

/// Types of event that can occur
typedef enum {
    EMouseMoved,
    EMousePressed,
    EMouseReleased,
    EMouseWheelMoved,
    EKeyPressed,
    EKeyReleased,
    ECharPressed,

    EWindowResized
} EventType;

/// Event object binding a Type of Event to the value
/// changed by the event.
/// This is used primalarly by the GLFW Callback funcs
/// to distribute the event recorded to all Listeners
typedef struct {
    EventType   type;           ///< Type of the event

    vec2i       v;              ///< Mouse pos
    int         i;              ///< Mouse wheel relative

    union {
        MouseButton button;     ///< Mouse button used
        Key         key;        ///< Keyboard key used
    };
} Event;



/// Initialize the Eventmanager and its states
bool EventManager_init();

/// Free the EventManager
void EventManager_destroy();

/// Update the input states of the EventManager and propagate all recorded events
/// to all registered listeners
/// Call this every frame
void EventManager_update();

// Input querying functions
    u32  GetMouseX();
    u32  GetMouseY();
    bool IsKeyDown( Key k );
    bool IsKeyUp( Key k );
    bool IsKeyHit( Key k );

    bool IsMouseDown( MouseButton m );
    bool IsMouseUp( MouseButton m );
    bool IsMouseHit( MouseButton m );

    bool IsWheelUp();
    bool IsWheelDown();



// Listeners
// To create a event listener, one must register it to the Eventmanager
// You can send a void* when registering. It will be available everytime the callback is used
// Exemple : if pData is a pointer on Camera, one could do this in his listener func :
//         ((Camera*)pData).mPosition.x = pEvent.MousePos.x;
//         ((Camera*)pData).mPosition.y = pEvent.MousePos.y;
    /// Listener function type
    /// @param event : Event recorded that can be processed
    /// @param data : Void pointer on anything that could be usefull in the callback
    typedef void (*ListenerFunc)( const Event *event, void *data );

    /// Different types of listener
    typedef enum {
        LT_KeyListener,
        LT_MouseListener,
        LT_ResizeListener
    } ListenerType;

    /// Listener registering function
    /// @param type : Key or Mouse listener
    /// @param func : Callback function of the listener (of type ListenerFunc)
    /// @param data : Void pointer on anything that could be usefull in the callback
    bool EventManager_addListener( ListenerType type, ListenerFunc func, void *data );




// GLFW Event Callback functions
    void WindowResizeCallback( int width, int height );
    void KeyPressedCallback( int key, int value );
    void CharPressedCallback( int key, int value );
    void MouseButtonCallback( int button, int value );
    void MouseWheelCallback( int wheel );
    void MouseMovedCallback( int x, int y );

#endif
