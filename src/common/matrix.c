#include "matrix.h"
#include "common.h"


extern inline void mat3_print( const mat3 *mat );

inline mat3 mat3_new(   float x00, float x01, float x02,
                        float x10, float x11, float x12,
                        float x20, float x21, float x22 ) {
    mat3 m = { .x = { x00, x01, x02, x10, x11, x12, x20, x21, x22 } };
    return m;
}

inline void mat3_identity( mat3 *A ) {
    if( A ) {
        memset( A->x, 0, 9 * sizeof( float ) );
        A->x[0] = A->x[4] = A->x[8] = 1.f;
    }
}

inline void mat3_cpy( mat3 *A, const mat3 *B ) {
    if( A && B ) 
        memcpy( A->x, B->x, 9 * sizeof( float ) );
}

inline void mat3_mul( mat3 *A, const mat3 *B ) {
    if( A && B ) {
        mat3 m;
        m.x[0] = A->x[0] * B->x[0] + A->x[1] * B->x[3] + A->x[2] * B->x[6];
        m.x[1] = A->x[0] * B->x[1] + A->x[1] * B->x[4] + A->x[2] * B->x[7];

        m.x[3] = A->x[3] * B->x[0] + A->x[4] * B->x[3] + A->x[5] * B->x[6];
        m.x[4] = A->x[3] * B->x[1] + A->x[4] * B->x[4] + A->x[5] * B->x[7];

        m.x[6] = A->x[6] * B->x[0] + A->x[7] * B->x[3] + A->x[8] * B->x[6];
        m.x[7] = A->x[6] * B->x[1] + A->x[7] * B->x[4] + A->x[8] * B->x[7];

        memcpy( A->x, m.x, 9 * sizeof( float ) );
    }
}


inline void mat3_translationMatrixf( mat3 *mat, float x, float y ) {
    if( mat ) {
        float val[9] = {  1, 0, 0,
                        0, 1, 0,
                        x, y, 1 };

        memcpy( mat->x, val, 9 * sizeof( float ) );
    }
}

inline void mat3_translationMatrixfv( mat3 *mat, const vec2 *v ) {
    if( mat && v ) {
        float val[9] = {  1,    0,    0,
                        0,    1,    0,
                        v->x, v->y, 1 };

        memcpy( mat->x, val, 9 * sizeof( float ) );
    }
}

inline void mat3_translatef( mat3 *mat, float x, float y ) {
    mat3 trans;
    mat3_translationMatrixf( &trans, x, y );
    mat3_mul( mat, &trans );
}

inline void mat3_translatefv( mat3 *mat, const vec2 *v ) {
    mat3 trans;
    mat3_translationMatrixfv( &trans, v );
    mat3_mul( mat, &trans );
}

inline void mat3_scaleMatrixf( mat3 *mat, float x, float y ) {
    if( mat ) {
        float val[9] = {  x,    0,    0,
                        0,    y,    0,
                        0,    0,    1 };

        memcpy( mat->x, val, 9 * sizeof( float ) );
    }
}

inline void mat3_scaleMatrixfv( mat3 *mat, const vec2 *v ) {
    if( mat && v ) {
        float val[9] = {  v->x, 0,    0,
                        0,    v->y, 0,
                        0,    0,    1 };

        memcpy( mat->x, val, 9 * sizeof( float ) );
    }
}

inline void mat3_scalef( mat3 *mat, float x, float y ) {
    mat3 scaleMat;
    mat3_scaleMatrixf( &scaleMat, x, y );
    mat3_mul( mat, &scaleMat );
}

inline void mat3_scalefv( mat3 *mat, const vec2 *v ) {
    mat3 scaleMat;
    mat3_scaleMatrixfv( &scaleMat, v );
    mat3_mul( mat, &scaleMat );
}


inline void mat3_rotationMatrixf( mat3 *mat, float angle ) {
    if( mat ) {
        angle = Deg2Rad( angle );
        float val[9] = {  cos( angle ), - sin( angle ),   0,
                        sin( angle ), cos( angle ),     0,
                        0,            0,                1 };

        memcpy( mat->x, val, 9 * sizeof( float ) );
    }
}

inline void mat3_rotatef( mat3 *mat, float angle ) {
    mat3 rot;
    mat3_rotationMatrixf( &rot, angle );
    mat3_mul( mat, &rot );
}


inline void mat3_ortho( mat3 *mat, float left, float right, float bottom, float top ) {
    if( mat ) {
        mat->x[0] = ( 2.f / ( right - left ) );
        mat->x[1] = 0.f;
        mat->x[2] = 0.f;

        mat->x[3] = 0.f;
        mat->x[4] = ( 2.f / ( top - bottom ) );
        mat->x[5] = 0.f;

        mat->x[6] = - ( right + left ) / ( right - left );
        mat->x[7] = - ( top + bottom ) / ( top - bottom );
        mat->x[8] = 0.f;
    }
}

inline void mat3_print( const mat3 *mat ) {
    if( mat )
        log_info(   "\n%f\t%f\t%f\n"
                    "%f\t%f\t%f\n"
                    "%f\t%f\t%f\n", mat->x[0], mat->x[1], mat->x[2],
                                    mat->x[3], mat->x[4], mat->x[5],
                                    mat->x[6], mat->x[7], mat->x[8] );
}

