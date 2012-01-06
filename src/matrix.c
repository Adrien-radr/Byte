#include "matrix.h"


// inline function declarations
//extern inline Matrix Matrix_new( f32, f32, f32, f32, f32, f32, f32, f32, f32 );
extern inline void mat3_print( const mat3 *mat );


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

