#include "event.h"


/// Structure containing the state of all inputs at a given time
typedef struct {
    bool keyboard[330];         ///< Keyboard keys
    bool mouse[5];              ///< Mouse buttons
    int  wheel;                 ///< Mouse wheel absolute pos

    bool close_signal;          ///< Window closing signal

    vec2i mouse_pos;            ///< Mouse absolute position
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
    InputState      curr_state,         ///< Inputs of current frame
                    prev_state;         ///< Inputs of previous frame

    ListenerArray   key_listeners,      ///< List of all registered mouse listeners
                    mouse_listeners,    ///< List of all registered key listeners
                    resize_listeners;   ///< List of all registered resize listeners

    EventArray      frame_key_events,    ///< All key events recorded during the frame
                    frame_mouse_events,  ///< All mouse events recorded during the frame
                    frame_resize_events; ///< All resize events recorded during the frame

} EventManager;


// Unique instance of the event manager
static EventManager *em = NULL;


bool EventManager_init() {
    check( !em, "Event Manager already created!\n" );

    em = byte_alloc( sizeof( EventManager ) );
    check_mem( em );

    // Init states
    memset( em->curr_state.keyboard, false, 330 * sizeof( bool ) );
    memset( em->curr_state.mouse, false, 5 * sizeof( bool ) );
    em->curr_state.wheel = 0;
    em->curr_state.close_signal = false;

    memset( em->prev_state.keyboard, false, 330 * sizeof( bool ) );
    memset( em->prev_state.mouse, false, 5 * sizeof( bool ) );
    em->prev_state.wheel = 0;
    em->prev_state.close_signal = false;

    // Init Listener arrays
    ListenerArray_init( &em->key_listeners, 10 );
    ListenerArray_init( &em->mouse_listeners, 10 );
    ListenerArray_init( &em->resize_listeners, 5 );

    EventArray_init( &em->frame_key_events, 50 );
    EventArray_init( &em->frame_mouse_events, 50 );
    EventArray_init( &em->frame_resize_events, 50 );


    log_info( "Event manager successfully initialized!\n" );

    return true;

error:
    EventManager_destroy();
    return false;
}


void EventManager_destroy() {
    if( em ) {
        ListenerArray_destroy( &em->key_listeners );
        ListenerArray_destroy( &em->mouse_listeners );
        ListenerArray_destroy( &em->resize_listeners );
        EventArray_destroy( &em->frame_key_events );
        EventArray_destroy( &em->frame_mouse_events );
        EventArray_destroy( &em->frame_resize_events );
        DEL_PTR( em );
    }
}

void EventManager_update() {
    // if there has been key events during the frame, send them to all keylisteners
    if( em->frame_key_events.cpt ) {
        for( u32 i = 0; i < em->key_listeners.cpt; ++i ) {
            const ListenerStruct *ls = &em->key_listeners.data[i];
            for( u32 j = 0; j < em->frame_key_events.cpt; ++j )
                ls->function( &em->frame_key_events.data[j], ls->data );
        }

        EventArray_clear( &em->frame_key_events );
    }
    // if there has been mouse events during the frame, send them to all mouselisteners
    if( em->frame_mouse_events.cpt ) {
        for( u32 i = 0; i < em->mouse_listeners.cpt; ++i ) {
            const ListenerStruct *ls = &em->mouse_listeners.data[i];
            for( u32 j = 0; j < em->frame_mouse_events.cpt; ++j )
                ls->function( &em->frame_mouse_events.data[j], ls->data );
        }

        EventArray_clear( &em->frame_mouse_events );
    }
    // if there has been resize events during the frame, send them to all resizelisteners
    if( em->frame_resize_events.cpt ) {
        for( u32 i = 0; i < em->resize_listeners.cpt; ++i ) {
            const ListenerStruct *ls = &em->resize_listeners.data[i];
            for( u32 j = 0; j < em->frame_resize_events.cpt; ++j )
                ls->function( &em->frame_resize_events.data[j], ls->data );
        }

        EventArray_clear( &em->frame_resize_events );
    }



    // Set previous state to current state.
    memcpy( em->prev_state.keyboard, em->curr_state.keyboard, 330 * sizeof( bool ) );
    memcpy( em->prev_state.mouse, em->curr_state.mouse, 5 * sizeof( bool ) );

    em->prev_state.wheel = em->curr_state.wheel;
    em->prev_state.close_signal = em->curr_state.close_signal;
    em->prev_state.mouse_pos.x = em->curr_state.mouse_pos.x;
    em->prev_state.mouse_pos.y = em->curr_state.mouse_pos.y;
}

u32  GetMouseX() {
    return em->curr_state.mouse_pos.x;
}

u32  GetMouseY() {
    return em->curr_state.mouse_pos.y;
}


bool IsKeyDown( Key pK ) {
    return em->curr_state.keyboard[pK];
}

bool IsKeyUp( Key pK ) {
    return !em->curr_state.keyboard[pK] && em->prev_state.keyboard[pK];
}

bool IsKeyHit( Key pK ) {
    return em->curr_state.keyboard[pK] && !em->prev_state.keyboard[pK];
}


bool IsMouseDown( MouseButton pK ) {
    return em->curr_state.mouse[pK];
}

bool IsMouseUp( MouseButton pK ) {
    return em->curr_state.mouse[pK] && !em->prev_state.mouse[pK];
}

bool IsMouseHit( MouseButton pK ) {
    return em->curr_state.mouse[pK] && !em->prev_state.mouse[pK];
}


bool IsWheelUp() {
    return em->curr_state.wheel > em->prev_state.wheel;
}

bool IsWheelDown() {
    return em->curr_state.wheel < em->prev_state.wheel;
}

// Listener funcs
bool EventManager_addListener( ListenerType type, ListenerFunc func, void *data ) {
    if( em ) {
        ListenerStruct s = { .function = func, .data = data };
        // switch on Listener type
        if( LT_KeyListener == type ) {
            if( ListenerArray_checkSize( &em->key_listeners ) ) {
                em->key_listeners.data[em->key_listeners.cpt++] = s;
            } else
                return false;
        } else if( LT_MouseListener == type ) {
            if( ListenerArray_checkSize( &em->mouse_listeners ) )
                em->mouse_listeners.data[em->mouse_listeners.cpt++] = s;
            else
                return false;
        } else if( LT_ResizeListener == type ) {
            if( ListenerArray_checkSize( &em->resize_listeners ) )
                em->resize_listeners.data[em->resize_listeners.cpt++] = s;
            else
                return false;
        } else
            return false;

        return true;
    }
    return false;
}

void EventManager_propagateEvent( const Event pEvent ) {
    switch( pEvent.type ) {
        case EKeyPressed:
        case EKeyReleased:
        case ECharPressed:
            if( EventArray_checkSize( &em->frame_key_events ) )
                em->frame_key_events.data[em->frame_key_events.cpt++] = pEvent;
            break;
        case EMouseMoved:
        case EMousePressed:
        case EMouseReleased:
        case EMouseWheelMoved:
            if( EventArray_checkSize( &em->frame_mouse_events ) )
                em->frame_mouse_events.data[em->frame_mouse_events.cpt++] = pEvent;
            break;
        case EWindowResized:
            if( EventArray_checkSize( &em->frame_resize_events ) )
                em->frame_resize_events.data[em->frame_resize_events.cpt++] = pEvent;
            break;
    }
}


// GLFW Event Callback functions
    void KeyPressedCallback( int key, int value ) {
        if( em )
            em->curr_state.keyboard[key] = value ? true : false;

        Event e = { .type = ( value ? EKeyPressed : EKeyReleased ), .i = key };
        e.key = (Key)key;

        EventManager_propagateEvent( e );
    }

    void CharPressedCallback( int c, int value ) {
        if( em ) {
            Event e = { .type = ECharPressed, .i = c };

            EventManager_propagateEvent( e );
        }
    }

    void MouseButtonCallback( int button, int value ) {
        if( em ) {
            em->curr_state.mouse[button] = value ? true : false;

            Event e = { .type = (value ? EMousePressed : EMouseReleased), .v = { em->curr_state.mouse_pos.x, em->curr_state.mouse_pos.y } };
            e.button = button;

            EventManager_propagateEvent( e );
        }
    }

    void MouseWheelCallback( int pWheel ) {
        if( em ) {
            em->curr_state.wheel = pWheel;

            Event e = { .type = EMouseWheelMoved, .i = (pWheel - em->prev_state.wheel) };

            EventManager_propagateEvent( e );
        }
    }

    void MouseMovedCallback( int x, int y ) {
        if( em ) {
            em->curr_state.mouse_pos.x = (f32)x;
            em->curr_state.mouse_pos.y = (f32)y;

            Event e = { .type = EMouseMoved, .v = { .x = x, .y = y } };

            EventManager_propagateEvent( e );
        }
    }

    void WindowResizeCallback( int width, int height ) {
        if( em ) {
            Event e = { .type = EWindowResized, .v = { .x = width, .y = height } };

            EventManager_propagateEvent( e );
        }
    }


