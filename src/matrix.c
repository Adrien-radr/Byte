#include "matrix.h"


// inline function declarations
//extern inline Matrix Matrix_new( f32, f32, f32, f32, f32, f32, f32, f32, f32 );
extern inline void mat3_print( const mat3 *mat );
extern inline mat3 mat3_new( f32 x00, f32 x01, f32 x02, f32 x10, f32 x11, f32 x12, f32 x20, f32 x21, f32 x22 );

void mat3_mul( mat3 *A, const mat3 *B ) {
    if( A && B ) {
        mat3 m;
        m.x[0] = A->x[0] * B->x[0] + A->x[1] * B->x[3] + A->x[2] * B->x[6];
        m.x[1] = A->x[0] * B->x[1] + A->x[1] * B->x[4] + A->x[2] * B->x[7];
//        m.x[2] = A->x[0] * B->x[2] + A->x[1] * B->x[5] + A->x[2] * B->x[8];

        m.x[3] = A->x[3] * B->x[0] + A->x[4] * B->x[3] + A->x[5] * B->x[6];
        m.x[4] = A->x[3] * B->x[1] + A->x[4] * B->x[4] + A->x[5] * B->x[7];
//        m.x[5] = A->x[3] * B->x[2] + A->x[4] * B->x[5] + A->x[5] * B->x[8];

//        m.x[6] = A->x[6] * B->x[0] + A->x[7] * B->x[3] + A->x[8] * B->x[6];
//        m.x[7] = A->x[6] * B->x[1] + A->x[7] * B->x[4] + A->x[8] * B->x[7];
//        m.x[8] = A->x[6] * B->x[2] + A->x[7] * B->x[5] + A->x[8] * B->x[8];

        memcpy( A->x, m.x, 9 * sizeof( f32 ) );
    }
}


void mat3_translationf( mat3 *mat, f32 x, f32 y ) {
    if( mat ) {
        f32 val[9] = {  1, 0, 0,
                        0, 1, 0,
                        x, y, 1 };

        memcpy( mat->x, val, 9 * sizeof( f32 ) );
    }
}

void mat3_translationfv( mat3 *mat, const vec2 *v ) {
    if( mat && v ) {
        f32 val[9] = {  1,    0,    0,
                        0,    1,    0,
                        v->x, v->y, 1 };

        memcpy( mat->x, val, 9 * sizeof( f32 ) );
    }
}

void mat3_translatef( mat3 *mat, f32 x, f32 y ) {
    if( mat ) {
        mat->x[6] += x;
        mat->x[7] += y;
    }
}

void mat3_translatefv( mat3 *mat, const vec2 *v ) {
    if( mat && v ) {
        mat->x[6] += v->x;
        mat->x[7] += v->y;
    }
}


void mat3_scalef( mat3 *mat, f32 x, f32 y ) {
    if( mat ) {
        f32 val[9] = {  x, 0, 0,
                        0, y, 0,
                        0, 0, 1 };

        memcpy( mat->x, val, 9 * sizeof( f32 ) );
    }
}

void mat3_scalefv( mat3 *mat, const vec2 *v ) {
    if( mat && v ) {
        f32 val[9] = {  v->x, 0,    0,
                        0,    v->y, 0,
                        0,    0,    1 };

        memcpy( mat->x, val, 9 * sizeof( f32 ) );
    }
}


void mat3_rotationf( mat3 *mat, f32 angle ) {
    if( mat ) {
        angle = Deg2Rad( angle );
        f32 val[9] = {  cos( angle ), - sin( angle ),   0,
                        sin( angle ), cos( angle ),     0,
                        0,            0,                1 };

        memcpy( mat->x, val, 9 * sizeof( f32 ) );
    }
}

void mat3_rotatef( mat3 *mat, f32 angle ) {
    if( mat ) {
        mat3 rot;
        mat3_rotationf( &rot, angle );
        mat3_mul( mat, &rot );
    }
}


void mat3_ortho( mat3 *mat, f32 left, f32 right, f32 bottom, f32 top ) {
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

