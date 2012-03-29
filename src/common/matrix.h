#ifndef BYTE_MATRIX_HPP
#define BYTE_MATRIX_HPP

#include "common.h"
#include "vector.h"

/// 3x3 Matrix structure
typedef struct s_mat3 {
    f32 x[9];       ///< Matrix data
} mat3;



inline mat3 mat3_new(   f32 x00, f32 x01, f32 x02, 
                        f32 x10, f32 x11, f32 x12,
                        f32 x20, f32 x21, f32 x22 ) {
    mat3 m = { .x = { x00, x01, x02, x10, x11, x12, x20, x21, x22 } };
    return m;
}

/// make the given matrix an identity matrix
void mat3_identity( mat3 *A );

/// Copy a matrix into another B->A
void mat3_cpy( mat3 *A, const mat3 *B );

/// Multiply AxB. Result is in A
void mat3_mul( mat3 *A, const mat3 *B );

/// Create a translation matrix 
void mat3_translationMatrixf( mat3 *mat, f32 x, f32 y );
void mat3_translationMatrixfv( mat3 *mat, const vec2 *v );

/// Add a translation component to an existing matrix
void mat3_translatef( mat3 *mat, f32 x, f32 y );
void mat3_translatefv( mat3 *mat, const vec2 *v );

/// Create a scale matrix
void mat3_scaleMatrixf( mat3 *mat, f32 x, f32 y );
void mat3_scaleMatrixfv( mat3 *mat, const vec2 *v );

/// Scale a matrix 
void mat3_scalef( mat3 *mat, f32 x, f32 y );
void mat3_scalefv( mat3 *mat, const vec2 *v );

/// Create a rotation matrix 
void mat3_rotationMatrixf( mat3 *mat, f32 angle );

/// Rotate an existing matrix
void mat3_rotatef( mat3 *mat, f32 angle );

/// Set the given matrix to be an orthographic matrix
void mat3_ortho( mat3 *mat, f32 left, f32 right, f32 bottom, f32 top ); 

/// print the given matrix
inline void mat3_print( const mat3 *mat ) {
    if( mat )
        log_info(   "\n%f\t%f\t%f\n"
                    "%f\t%f\t%f\n"
                    "%f\t%f\t%f\n", mat->x[0], mat->x[1], mat->x[2], 
                                    mat->x[3], mat->x[4], mat->x[5], 
                                    mat->x[6], mat->x[7], mat->x[8]  );
}

#endif // BYTE_MATRIX
