#ifndef BYTE_VECTOR_H
#define BYTE_VECTOR_H

#include "common.h"

typedef struct s_vec2 {
    f32 x, y;
} vec2;

vec2 vec2_add( const vec2 *A, const vec2 *B );
vec2 vec2_sub( const vec2 *A, const vec2 *B );
vec2 vec2_mul( const vec2 *A, f32 pFactor );
vec2 vec2_div( const vec2 *A, f32 pFactor );

f32  vec2_len( const vec2 *A );
f32  vec2_sqlen( const vec2 *A );

f32  vec2_dot( const vec2 *A, const vec2 *B );
void vec2_normalize( vec2 *A );

#endif
