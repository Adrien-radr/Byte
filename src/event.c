#include "event.h"


/// Structure containing the state of all inputs at a given time
typedef struct s_InputState {
    bool mKeyboard[330];        ///< Keyboard keys
    bool mMouse[5];             ///< Mouse buttons
    int  mWheel;                ///< Mouse wheel absolute pos

    bool mCloseSignal;          ///< Window closing signal

    vec2 mMousePos;             ///< Mouse absolute position
} InputState;


// Array ListenerArray with ListenerFunc* data type
SimpleArray( ListenerFunc, Listener )

/// Manage real time events from GLFW callbacks
/// Distribute these events to registered listeners
typedef struct s_EventManager {
    InputState   mCurrState,        ///< Inputs of current frame
                 mPrevState;        ///< Inputs of previous frame
            
    ListenerArray mKeyListeners;
    ListenerArray mMouseListeners;
   
} EventManager;


// Unique instance of the event manager
static EventManager *eventManager = NULL;


bool EventManager_init() {
    check( !eventManager, "Event Manager already created!\n" );

    eventManager = byte_alloc( sizeof( EventManager ) );
    check_mem( eventManager );

    // Init states
    memset( eventManager->mCurrState.mKeyboard, false, 330 * sizeof( bool ) );
    memset( eventManager->mCurrState.mMouse, false, 5 * sizeof( bool ) );
    eventManager->mCurrState.mWheel = 0;
    eventManager->mCurrState.mCloseSignal = false;

    memset( eventManager->mPrevState.mKeyboard, false, 330 * sizeof( bool ) );
    memset( eventManager->mPrevState.mMouse, false, 5 * sizeof( bool ) );
    eventManager->mPrevState.mWheel = 0;
    eventManager->mPrevState.mCloseSignal = false;

    // Init Listener arrays
    ListenerArray_init( &eventManager->mKeyListeners, 10 );
    ListenerArray_init( &eventManager->mMouseListeners, 10 );


    
    log_info( "Event manager successfully initialized!\n" );

    return true;

error:
    EventManager_destroy();
    return false;
}


void EventManager_destroy() {
    if( eventManager ) {
        ListenerArray_destroy( &eventManager->mKeyListeners );
        ListenerArray_destroy( &eventManager->mMouseListeners );
        DEL_PTR( eventManager, sizeof( EventManager ) );
    }
}

void EventManager_update() {
    // Set previous state to current state.
    memcpy( eventManager->mPrevState.mKeyboard, eventManager->mCurrState.mKeyboard, 330 * sizeof( bool ) );
    memcpy( eventManager->mPrevState.mMouse, eventManager->mCurrState.mMouse, 5 * sizeof( bool ) );

    eventManager->mPrevState.mWheel = eventManager->mCurrState.mWheel;
    eventManager->mPrevState.mCloseSignal = eventManager->mCurrState.mCloseSignal;
    eventManager->mPrevState.mMousePos.x = eventManager->mCurrState.mMousePos.x;
    eventManager->mPrevState.mMousePos.y = eventManager->mCurrState.mMousePos.y;
}

u32  GetMouseX() {
    return eventManager->mCurrState.mMousePos.x;
}

u32  GetMouseY() {
    return eventManager->mCurrState.mMousePos.y;
}


bool IsKeyDown( enum Key pK ) {
    return eventManager->mCurrState.mKeyboard[pK];
}

bool IsKeyUp( enum Key pK ) {
    return !eventManager->mCurrState.mKeyboard[pK] && eventManager->mPrevState.mKeyboard[pK];
}

bool IsKeyHit( enum Key pK ) {
    return eventManager->mCurrState.mKeyboard[pK] && !eventManager->mPrevState.mKeyboard[pK];
}


bool IsMouseDown( enum MouseButton pK ) {
    return eventManager->mCurrState.mMouse[pK];
}

bool IsMouseUp( enum MouseButton pK ) {
    return eventManager->mCurrState.mMouse[pK] && !eventManager->mPrevState.mMouse[pK];
}

bool IsMouseHit( enum MouseButton pK ) {
    return eventManager->mCurrState.mMouse[pK] && !eventManager->mPrevState.mMouse[pK];
}


bool IsWheelUp() {
    return eventManager->mCurrState.mWheel > eventManager->mPrevState.mWheel;
}

bool IsWheelDown() {
    return eventManager->mCurrState.mWheel < eventManager->mPrevState.mWheel;
}

// Listener funcs
bool EventManager_addListener( enum ListenerType pType, ListenerFunc pFunc ) {
    if( eventManager ) {
        // switch on Listener type
        if( LT_KeyListener == pType ) {
            if( ListenerArray_checkSize( &eventManager->mKeyListeners ) ) 
                eventManager->mKeyListeners.data[eventManager->mKeyListeners.cpt++] = pFunc;
            else
                return false;
        } else if( LT_MouseListener == pType ) {
            if( ListenerArray_checkSize( &eventManager->mMouseListeners ) ) 
                eventManager->mMouseListeners.data[eventManager->mMouseListeners.cpt++] = pFunc;
            else
                return false;
        } else
            return false;

        return true;
    }
    return false;
}

void EventManager_propagateEvent( const Event* pEvent ) {
    switch( pEvent->mType ) {
        case E_KeyPressed:
        case E_KeyReleased:
        case E_CharPressed:
            for( u32 i = 0; i < eventManager->mKeyListeners.cpt; ++i )
                eventManager->mKeyListeners.data[i]( pEvent );
            break;
        default:
            for( u32 i = 0; i < eventManager->mMouseListeners.cpt; ++i )
                eventManager->mMouseListeners.data[i]( pEvent );
            break;
    }
}


// GLFW Event Callback functions
    void KeyPressedCallback( int pKey, int pValue ) {
        if( eventManager )
            eventManager->mCurrState.mKeyboard[pKey] = pValue ? true : false;

        Event e = { .mType = ( pValue ? E_KeyPressed : E_KeyReleased ), .mKey = (enum Key)pKey, .mChar = pKey };

        EventManager_propagateEvent( &e );
    }

    void CharPressedCallback( int pChar, int pValue ) {
        if( eventManager ) {
            Event e = { .mType = E_CharPressed, .mChar = pChar };

            EventManager_propagateEvent( &e );
        }
    }

    void MousePressedCallback( int pButton, int pValue ) {
        if( eventManager ) {
            eventManager->mCurrState.mMouse[pButton] = pValue ? true : false;

            Event e = { .mType = (pValue ? E_MousePressed : E_MouseReleased), .mMouseButton = (enum MouseButton)pButton };

            EventManager_propagateEvent( &e );
        }
    }

    void MouseWheelCallback( int pWheel ) {
        if( eventManager ) {
            eventManager->mCurrState.mWheel = pWheel;

            Event e = { .mType = E_MouseWheelMoved, .mWheel = (pWheel - eventManager->mPrevState.mWheel) };

            EventManager_propagateEvent( &e );
        }
    }

    void MouseMovedCallback( int pX, int pY ) {
        if( eventManager ) {
            eventManager->mCurrState.mMousePos.x = (f32)pX;
            eventManager->mCurrState.mMousePos.y = (f32)pY;

            Event e = { .mType = E_MouseMoved, .mMousePos = { .x = pX, .y = pY } };

            EventManager_propagateEvent( &e );
        }
    }


