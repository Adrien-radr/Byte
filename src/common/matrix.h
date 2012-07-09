#ifndef BYTE_MATRIX_H
#define BYTE_MATRIX_H

#include "vector.h"

/// 3x3 Matrix structure
typedef struct s_mat3 {
    float x[9];       ///< Matrix data
} mat3;



mat3 mat3_new( float x00, float x01, float x02,
               float x10, float x11, float x12,
               float x20, float x21, float x22 );


/// make the given matrix an identity matrix
void mat3_identity( mat3 *A );

/// Copy a matrix into another B->A
void mat3_cpy( mat3 *A, const mat3 *B );

/// Multiply AxB. Result is in A
void mat3_mul( mat3 *A, const mat3 *B );

/// Create a translation matrix
void mat3_translationMatrixf( mat3 *mat, float x, float y );
void mat3_translationMatrixfv( mat3 *mat, const vec2 *v );

/// Add a translation component to an existing matrix
void mat3_translatef( mat3 *mat, float x, float y );
void mat3_translatefv( mat3 *mat, const vec2 *v );

/// Create a scale matrix
void mat3_scaleMatrixf( mat3 *mat, float x, float y );
void mat3_scaleMatrixfv( mat3 *mat, const vec2 *v );

/// Scale a matrix
void mat3_scalef( mat3 *mat, float x, float y );
void mat3_scalefv( mat3 *mat, const vec2 *v );

/// Create a rotation matrix
void mat3_rotationMatrixf( mat3 *mat, float angle );

/// Rotate an existing matrix
void mat3_rotatef( mat3 *mat, float angle );

/// Set the given matrix to be an orthographic matrix
void mat3_ortho( mat3 *mat, float left, float right, float bottom, float top );

/// print the given matrix
void mat3_print( const mat3 *mat );

#endif // BYTE_MATRIX
