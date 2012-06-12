#include "camera.h"
#include "context.h"
#include "renderer.h"

static const float zoom_levels[10] = { 1.f, 1.25f, 1.5f, 1.75f, 2.f, 2.5f, 3.f, 4.f, 5.f, 6.f };

Camera *Camera_new() {
    Camera *c = NULL;
    check( Context_isInitialized(), "Can't create a camera if no context has been created!\n" );

    c = byte_alloc( sizeof( Camera ) );

    c->mZoomX = 2;
    c->mZoom = zoom_levels[c->mZoomX];
    c->mSpeed = 4.f * zoom_levels[c->mZoomX];
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

void Camera_calculateProjectionMatrix( Camera *pCamera ) {
    if( pCamera && Context_isInitialized() ) {
        vec2i windowSize = Context_getSize();


        f32 xoffset = ((f32)windowSize.x - pCamera->mZoom * (f32)windowSize.x) / 2.f;
        f32 yoffset = ((f32)windowSize.y - pCamera->mZoom * (f32)windowSize.y) / 2.f;

        pCamera->global_position.x = xoffset + pCamera->mPosition.x;
        pCamera->global_position.y = yoffset + pCamera->mPosition.y;

        f32 width = (f32)windowSize.x - xoffset,
            height = (f32)windowSize.y - yoffset;


        mat3_ortho( &pCamera->mProjectionMatrix, pCamera->global_position.x, 
                                                 width + pCamera->mPosition.x, 
                                                 height + pCamera->mPosition.y, 
                                                 pCamera->global_position.y );
    }
}

inline void Camera_update( Camera *camera ) {
    if( camera ) {
        // handle key movement for camera pan
        vec2 move = { .x = 0.f, .y = 0.f };
        if( IsKeyDown( K_W ) )
            move.y -= 1.f;
        if( IsKeyDown( K_A ) )
            move.x -= 1.f;
        if( IsKeyDown( K_S ) )
            move.y += 1.f;
        if( IsKeyDown( K_D ) )
            move.x += 1.f;

        if( move.x || move.y )
            Camera_move( camera, &move );
    }
}

inline void Camera_move( Camera *pCamera, vec2 *pVector ) {
    if( pCamera && pVector ) {
        // normalize vector 
        vec2_normalize( pVector );

        pCamera->mPosition.x += pVector->x * pCamera->mSpeed;
        pCamera->mPosition.y += pVector->y * pCamera->mSpeed;

        // recalculate projection matrix and warn every shaders using it
        Camera_calculateProjectionMatrix( pCamera );
        Renderer_updateProjectionMatrix( ECamera, &pCamera->mProjectionMatrix );
    }
}


void Camera_zoom( Camera *pCamera, int pZoom ) {
    if( pCamera ) {
        // clamp pZoom (no rapid zoom)
        Clamp( &pZoom, -1, 1 );


        // get new index for zoom_levels array
        int old_x = pCamera->mZoomX;
        pCamera->mZoomX -= pZoom;
        Clamp( &pCamera->mZoomX, 0, 9 );

        // if change occured, change zoom level and update cam
        if( old_x != pCamera->mZoomX ) {
            f32 old_zoom = pCamera->mZoom;
            pCamera->mZoom = zoom_levels[pCamera->mZoomX];

            // update pan speed to be proportionnal to cam height
            pCamera->mSpeed = 4.f * zoom_levels[pCamera->mZoomX];

            f32 change = pCamera->mZoom - old_zoom;


            vec2i windowSize = Context_getSize();
            f32 mx = GetMouseX(),
                my = GetMouseY();


            // here we zoom in the direction from the window center to the mouse position, with a 
            // magnitude depending on the zoom level (less magnitude if near ground)
            vec2 dir = { .x = (mx - ( windowSize.x / 2.f ) ), .y = (my - ( windowSize.y / 2.f ) ) };
            f32 dir_len = vec2_len( &dir );
            vec2_normalize( &dir );

            f32 pan_magnitude = - dir_len * change;
            dir = vec2_mul( &dir, pan_magnitude );


            pCamera->mPosition.x += dir.x;
            pCamera->mPosition.y += dir.y;

            // recalculate projection matrix and warn every shaders using it
            Camera_calculateProjectionMatrix( pCamera );
            Renderer_updateProjectionMatrix( ECamera, &pCamera->mProjectionMatrix );
        }
    }

/*              ZOOM LEVEL FROM 1/x*x*x
        static int reached_min = 0, reached_max = 0;

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

            f32 pan_magnitude = - dir_len * change;
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
    */
}
