#ifndef BYTE_EVENT_HPP
#define BYTE_EVENT_HPP

#include "common.h"
#include "keys.h"
#include "vector.h"

/// Types of event that can occur
typedef enum {
    E_MouseMoved,
    E_MousePressed,
    E_MouseReleased,
    E_MouseWheelMoved,
    E_KeyPressed,
    E_KeyReleased,
    E_CharPressed,

    E_WindowResized
} EventType;

/// Event object binding a Type of Event to the value
/// changed by the event.
/// This is used primalarly by the GLFW Callback funcs
/// to distribute the event recorded to all Listeners
typedef struct {
    EventType   Type;               ///< Type of the event

    vec2        v;
    int         i;
    union {
        MouseButton button;
        Key         key;
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
    bool IsKeyDown( Key pK );
    bool IsKeyUp( Key pK );
    bool IsKeyHit( Key pK );

    bool IsMouseDown( MouseButton pK );
    bool IsMouseUp( MouseButton pK );
    bool IsMouseHit( MouseButton pK );

    bool IsWheelUp();
    bool IsWheelDown();



// Listeners
// To create a event listener, one must register it to the Eventmanager
// You can send a void* when registering. It will be available everytime the callback is used
// Exemple : if pData is a pointer on Camera, one could do this in his listener func :
//         ((Camera*)pData).mPosition.x = pEvent.MousePos.x;
//         ((Camera*)pData).mPosition.y = pEvent.MousePos.y;
    /// Listener function type
    /// @param pEvent : Event recorded that can be processed
    /// @param pData : Void pointer on anything that could be usefull in the callback
    typedef void (*ListenerFunc)( const Event *pEvent, void *pData );

    /// Different types of listener
    typedef enum {
        LT_KeyListener,
        LT_MouseListener,
        LT_ResizeListener
    } ListenerType;

    /// Listener registering function
    /// @param pType : Key or Mouse listener
    /// @param pFunc : Callback function of the listener (of type ListenerFunc)
    /// @param pData : Void pointer on anything that could be usefull in the callback
    bool EventManager_addListener( ListenerType pType, ListenerFunc pFunc, void *pData );




// GLFW Event Callback functions
    void WindowResizeCallback( int pWidth, int pHeight );
    void KeyPressedCallback( int pKey, int pValue );
    void CharPressedCallback( int pKey, int pValue );
    void MousePressedCallback( int pButton, int pValue );
    void MouseWheelCallback( int pWheel );
    void MouseMovedCallback( int pX, int pY );

#endif
