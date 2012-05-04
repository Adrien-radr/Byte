#ifndef BYTE_VECTOR_H
#define BYTE_VECTOR_H

#include "common.h"

typedef struct vec2_ {
    f32 x, y;
} vec2;

typedef struct vec2i_ {
    int x, y;
} vec2i;

/// Copy a given vector in another (B->A)
void vec2_cpy( vec2 *A, const vec2 *B );
void vec2i_cpy( vec2i *A, const vec2i *B );

/// Returns the negate of a vector
vec2 vec2_neg( const vec2 *A );
vec2i vec2i_neg( const vec2i *A );

/// Add two vectors and returns the result vector
vec2 vec2_add( const vec2 *A, const vec2 *B );
vec2i vec2i_add( const vec2i *A, const vec2i *B );

/// Substract two vectors and returns the result vector
vec2 vec2_sub( const vec2 *A, const vec2 *B );
vec2i vec2i_sub( const vec2i *A, const vec2i *B );

/// Multiply two vectors returns the result vector
vec2 vec2_mulv( const vec2 *A, const vec2 *B );
vec2i vec2i_mulv( const vec2i *A, const vec2i *B );

/// Multiply a vector by a scalar and returns the result vector
vec2 vec2_mul( const vec2 *A, f32 pFactor );
vec2i vec2i_mul( const vec2i *A, int pFactor );

/// Multiply a vector by a scalar and returns the result vector
vec2 vec2_div( const vec2 *A, f32 pFactor );
vec2i vec2i_div( const vec2i *A, int pFactor );

/// Returns the length(magnitude) of a vector
f32  vec2_len( const vec2 *A );

/// Returns the square length of a vector (used by vec2_len)
f32  vec2_sqlen( const vec2 *A );

/// Returns the dot product between two vectors
f32  vec2_dot( const vec2 *A, const vec2 *B );

/// Normalize a given vector
void vec2_normalize( vec2 *A );


#endif
