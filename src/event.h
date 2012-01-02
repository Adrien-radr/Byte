#ifndef BYTE_EVENT_HPP
#define BYTE_EVENT_HPP

#include "common.h"
#include "keys.h"
#include "vector.h"

/// Event object binding a Type of Event to the value
/// changed by the event.
/// This is used primalarly by the GLFW Callback funcs
/// to distribute the event recorded to all Listeners
typedef struct s_Event {
    /// Type of the event
    enum {
        E_MouseMoved,
        E_MousePressed,
        E_MouseReleased,
        E_MouseWheelMoved,
        E_KeyPressed,
        E_KeyReleased,
        E_CharPressed
    } mType;
                        
    vec2 mMousePos;     		    ///< In case of MouseMoved
    enum MouseButton mMouseButton;	///< In case of MousePressed/Released
    int mWheel;					    ///< In case of MouseWheelMoved
    enum Key mKey;				    ///< In case of KeyPressed/Released
    char mChar;					    ///< In case of CharPressed/KeyPressed/Released
} Event;



/// Initialize the Eventmanager and its states
bool EventManager_init();

/// Free the EventManager
void EventManager_destroy();

/// Update the input states of the EventManager
/// Call this every frame
void EventManager_update();

// Input querying functions
    u32  GetMouseX();
    u32  GetMouseY();
    bool IsKeyDown( enum Key pK );
    bool IsKeyUp( enum Key pK );
    bool IsKeyHit( enum Key pK );

    bool IsMouseDown( enum MouseButton pK );
    bool IsMouseUp( enum MouseButton pK );
    bool IsMouseHit( enum MouseButton pK );

    bool IsWheelUp();
    bool IsWheelDown();



// Listeners
    typedef void (*ListenerFunc)( const Event* );
    
bool EventManager_addKeyListener( ListenerFunc pFunc );
bool EventManager_addMouseListener( ListenerFunc pFunc );




// GLFW Event Callback functions
    void KeyPressedCallback( int pKey, int pValue );
    void CharPressedCallback( int pKey, int pValue );
    void MousePressedCallback( int pButton, int pValue );
    void MouseWheelCallback( int pWheel );
    void MouseMovedCallback( int pX, int pY );

#endif
