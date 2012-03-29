#include "camera.h"
#include "context.h"
#include "renderer.h"

Camera *Camera_new() {
    Camera *c = NULL;
    check( Context_isInitialized(), "Can't create a camera if no context has been created!\n" );

    c = byte_alloc( sizeof( Camera ) );

    c->mSpeed = c->mZoom = c->mZoomX = 1.f;
    c->mZoomSpeed = .05f;
    c->mZoomMax = 1.f;
    c->mZoomMin = .75f;
    Camera_calculateProjectionMatrix( c );

error:
    return c;
}

void Camera_destroy( Camera *pCamera ) {
    DEL_PTR( pCamera );
}

void Camera_registerListener( Camera *pCamera, ListenerFunc pFunc, ListenerType pType ) {
    if( pCamera ) {
        switch( pType ) {
            case LT_KeyListener :
                pCamera->mKeyListener = pFunc;
                EventManager_addListener( pType, pCamera->mKeyListener, pCamera );
                break;
            case LT_MouseListener :
                pCamera->mMouseListener = pFunc;
                EventManager_addListener( pType, pCamera->mMouseListener, pCamera );
                break;
            default :
                break;
        }
    }
}

void Camera_registerUpdateFunction( Camera *pCamera, CameraUpdate pFunc ) {
    if( pCamera ) {
        pCamera->mUpdateFunc = pFunc;
    }
}

void Camera_calculateProjectionMatrix( Camera *pCamera ) {
    if( pCamera && Context_isInitialized() ) {
        vec2 windowSize = Context_getSize();


        f32 xoffset = (windowSize.x - pCamera->mZoom * windowSize.x) / 2.f;
        f32 yoffset = (windowSize.y - pCamera->mZoom * windowSize.y) / 2.f;


        f32 width = windowSize.x - xoffset,
            height = windowSize.y - yoffset;


        mat3_ortho( &pCamera->mProjectionMatrix, xoffset + pCamera->mPosition.x, 
                                                 width + pCamera->mPosition.x, 
                                                 height + pCamera->mPosition.y, 
                                                 yoffset + pCamera->mPosition.y );
    }
}

void Camera_update( Camera *pCamera ) {
    if( pCamera ) 
        pCamera->mUpdateFunc( pCamera );
}

void Camera_move( Camera *pCamera, vec2 *pVector ) {
    if( pCamera && pVector ) {
        pCamera->mPosition.x += pVector->x * pCamera->mSpeed;
        pCamera->mPosition.y += pVector->y * pCamera->mSpeed;

        // recalculate projection matrix and warn every shaders using it
        Camera_calculateProjectionMatrix( pCamera );
        Renderer_updateProjectionMatrix( &pCamera->mProjectionMatrix );
    }
}


void Camera_zoom( Camera *pCamera, int pZoom ) {
    if( pCamera ) {
        static int reached_min = 0, reached_max = 0;

        // clamp pZoom (no rapid zoom
        Clamp( &pZoom, -1.f, 1.f );

        f32 x = pCamera->mZoomX;

        pCamera->mZoomX += pCamera->mZoomSpeed * pZoom;

        // Clamp X (min and max zoom levels)
        if( pCamera->mZoomX < pCamera->mZoomMin ) {
            pCamera->mZoomX = pCamera->mZoomMin;
            if( !reached_min ) reached_min = 1;
        } else if( pCamera->mZoomX > pCamera->mZoomMax ) {
            pCamera->mZoomX = pCamera->mZoomMax;
            if( !reached_max ) reached_max = 1;
        } else {
            reached_min = reached_max = 0;
        }

        if( reached_min < 2 && reached_max < 2 ) {
            // Zoom level = 1 / (x^3) 
            pCamera->mZoom = (1.f / (pCamera->mZoomX*pCamera->mZoomX*pCamera->mZoomX));

            x = (1.f / (x*x*x));

            f32 change = pCamera->mZoom - x;


            // modify cameraposition to zoom on mouse
            vec2 windowSize = Context_getSize();
            f32 mx = GetMouseX(),
                my = GetMouseY();


            // here we zoom in the direction from the window center to the mouse position, with a 
            // magnitude depending on the zoom level (less magnitude if near ground)
            vec2 dir = { .x = (mx - ( windowSize.x / 2.f ) ), .y = (my - ( windowSize.y / 2.f ) ) };
            f32 dir_len = vec2_len( &dir );
            vec2_normalize( &dir );

            f32 pan_magnitude = - dir_len * change;//* .1f * (1.f / pCamera->mZoom); 
            dir = vec2_mul( &dir, pan_magnitude );


            pCamera->mPosition.x += dir.x;
            pCamera->mPosition.y += dir.y;

            if( 1 == reached_min ) reached_min = 2;
            if( 1 == reached_max ) reached_max = 2;
        }

        // recalculate projection matrix and warn every shaders using it
        Camera_calculateProjectionMatrix( pCamera );
        Renderer_updateProjectionMatrix( &pCamera->mProjectionMatrix );
    }
}
