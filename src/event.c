#include "event.h"


/// Structure containing the state of all inputs at a given time
typedef struct {
    bool mKeyboard[330];        ///< Keyboard keys
    bool mMouse[5];             ///< Mouse buttons
    int  mWheel;                ///< Mouse wheel absolute pos

    bool mCloseSignal;          ///< Window closing signal

    vec2 mMousePos;             ///< Mouse absolute position
} InputState;


typedef struct {
    ListenerFunc    function;   ///< actual listener function
    void            *data;      ///< data to be set when registering listener
} ListenerStruct;

// Array ListenerArray with ListenerStruct data type
SimpleArray( ListenerStruct, Listener )

// Array EventArray with Event data type
SimpleArray( Event, Event );

/// Manage real time events from GLFW callbacks
/// Distribute these events to registered listeners
typedef struct {
    InputState      mCurrState,         ///< Inputs of current frame
                    mPrevState;         ///< Inputs of previous frame
            
    ListenerArray   mKeyListeners,      ///< List of all registered mouse listeners
                    mMouseListeners,    ///< List of all registered key listeners
                    mResizeListeners;   ///< List of all registered resize listeners

    EventArray      mFrameKeyEvents,    ///< All key events recorded during the frame
                    mFrameMouseEvents,  ///< All mouse events recorded during the frame
                    mFrameResizeEvents; ///< All resize events recorded during the frame
   
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
    ListenerArray_init( &eventManager->mResizeListeners, 5 );

    EventArray_init( &eventManager->mFrameKeyEvents, 50 );
    EventArray_init( &eventManager->mFrameMouseEvents, 50 );
    EventArray_init( &eventManager->mFrameResizeEvents, 50 );

    
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
        ListenerArray_destroy( &eventManager->mResizeListeners );
        EventArray_destroy( &eventManager->mFrameKeyEvents );
        EventArray_destroy( &eventManager->mFrameMouseEvents );
        EventArray_destroy( &eventManager->mFrameResizeEvents );
        DEL_PTR( eventManager );
    }
}

void EventManager_update() {
    // if there has been key events during the frame, send them to all keylisteners
    if( eventManager->mFrameKeyEvents.cpt ) {
        for( u32 i = 0; i < eventManager->mKeyListeners.cpt; ++i ) {
            const ListenerStruct *ls = &eventManager->mKeyListeners.data[i];
            for( u32 j = 0; j < eventManager->mFrameKeyEvents.cpt; ++j )
                ls->function( &eventManager->mFrameKeyEvents.data[j], ls->data );
        }

        EventArray_clear( &eventManager->mFrameKeyEvents );
    }
    // if there has been mouse events during the frame, send them to all mouselisteners
    if( eventManager->mFrameMouseEvents.cpt ) {
        for( u32 i = 0; i < eventManager->mMouseListeners.cpt; ++i ) {
            const ListenerStruct *ls = &eventManager->mMouseListeners.data[i];
            for( u32 j = 0; j < eventManager->mFrameMouseEvents.cpt; ++j )
                ls->function( &eventManager->mFrameMouseEvents.data[j], ls->data );
        }

        EventArray_clear( &eventManager->mFrameMouseEvents );
    }
    // if there has been resize events during the frame, send them to all resizelisteners
    if( eventManager->mFrameResizeEvents.cpt ) {
        for( u32 i = 0; i < eventManager->mResizeListeners.cpt; ++i ) {
            const ListenerStruct *ls = &eventManager->mResizeListeners.data[i];
            for( u32 j = 0; j < eventManager->mFrameResizeEvents.cpt; ++j )
                ls->function( &eventManager->mFrameResizeEvents.data[j], ls->data );
        }

        EventArray_clear( &eventManager->mFrameResizeEvents );
    }



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


bool IsKeyDown( Key pK ) {
    return eventManager->mCurrState.mKeyboard[pK];
}

bool IsKeyUp( Key pK ) {
    return !eventManager->mCurrState.mKeyboard[pK] && eventManager->mPrevState.mKeyboard[pK];
}

bool IsKeyHit( Key pK ) {
    return eventManager->mCurrState.mKeyboard[pK] && !eventManager->mPrevState.mKeyboard[pK];
}


bool IsMouseDown( MouseButton pK ) {
    return eventManager->mCurrState.mMouse[pK];
}

bool IsMouseUp( MouseButton pK ) {
    return eventManager->mCurrState.mMouse[pK] && !eventManager->mPrevState.mMouse[pK];
}

bool IsMouseHit( MouseButton pK ) {
    return eventManager->mCurrState.mMouse[pK] && !eventManager->mPrevState.mMouse[pK];
}


bool IsWheelUp() {
    return eventManager->mCurrState.mWheel > eventManager->mPrevState.mWheel;
}

bool IsWheelDown() {
    return eventManager->mCurrState.mWheel < eventManager->mPrevState.mWheel;
}

// Listener funcs
bool EventManager_addListener( ListenerType pType, ListenerFunc pFunc, void *pData ) {
    if( eventManager ) {
        ListenerStruct s = { .function = pFunc, .data = pData };
        // switch on Listener type
        if( LT_KeyListener == pType ) {
            if( ListenerArray_checkSize( &eventManager->mKeyListeners ) ) {
                eventManager->mKeyListeners.data[eventManager->mKeyListeners.cpt++] = s;
            } else
                return false;
        } else if( LT_MouseListener == pType ) {
            if( ListenerArray_checkSize( &eventManager->mMouseListeners ) ) 
                eventManager->mMouseListeners.data[eventManager->mMouseListeners.cpt++] = s;
            else
                return false;
        } else if( LT_ResizeListener == pType ) {
            if( ListenerArray_checkSize( &eventManager->mResizeListeners ) ) 
                eventManager->mResizeListeners.data[eventManager->mResizeListeners.cpt++] = s;
            else
                return false;
        } else
            return false;

        return true;
    }
    return false;
}

void EventManager_propagateEvent( const Event pEvent ) {
    switch( pEvent.Type ) {
        case E_KeyPressed:
        case E_KeyReleased:
        case E_CharPressed:
            if( EventArray_checkSize( &eventManager->mFrameKeyEvents ) ) 
                eventManager->mFrameKeyEvents.data[eventManager->mFrameKeyEvents.cpt++] = pEvent;
            break;
        case E_MouseMoved:
        case E_MousePressed:
        case E_MouseReleased:
        case E_MouseWheelMoved:
            if( EventArray_checkSize( &eventManager->mFrameMouseEvents ) ) 
                eventManager->mFrameMouseEvents.data[eventManager->mFrameMouseEvents.cpt++] = pEvent;
            break;
        case E_WindowResized:
            if( EventArray_checkSize( &eventManager->mFrameResizeEvents ) ) 
                eventManager->mFrameResizeEvents.data[eventManager->mFrameResizeEvents.cpt++] = pEvent;
            break;
    }
}


// GLFW Event Callback functions
    void KeyPressedCallback( int pKey, int pValue ) {
        if( eventManager )
            eventManager->mCurrState.mKeyboard[pKey] = pValue ? true : false;

        Event e = { .Type = ( pValue ? E_KeyPressed : E_KeyReleased ), .key = (Key)pKey, .i = pKey };

        EventManager_propagateEvent( e );
    }

    void CharPressedCallback( int pChar, int pValue ) {
        if( eventManager ) {
            Event e = { .Type = E_CharPressed, .i = pChar };

            EventManager_propagateEvent( e );
        }
    }

    void MousePressedCallback( int pButton, int pValue ) {
        if( eventManager ) {
            eventManager->mCurrState.mMouse[pButton] = pValue ? true : false;

            Event e = { .Type = (pValue ? E_MousePressed : E_MouseReleased), .button = (MouseButton)pButton };

            EventManager_propagateEvent( e );
        }
    }

    void MouseWheelCallback( int pWheel ) {
        if( eventManager ) {
            eventManager->mCurrState.mWheel = pWheel;

            Event e = { .Type = E_MouseWheelMoved, .i = (pWheel - eventManager->mPrevState.mWheel) };

            EventManager_propagateEvent( e );
        }
    }

    void MouseMovedCallback( int pX, int pY ) {
        if( eventManager ) {
            eventManager->mCurrState.mMousePos.x = (f32)pX;
            eventManager->mCurrState.mMousePos.y = (f32)pY;

            Event e = { .Type = E_MouseMoved, .v = { .x = pX, .y = pY } };

            EventManager_propagateEvent( e );
        }
    }

    void WindowResizeCallback( int pWidth, int pHeight ) {
        if( eventManager ) {
            Event e = { .Type = E_WindowResized, .v = { .x = pWidth, .y = pHeight } };

            EventManager_propagateEvent( e );
        }
    }


