#include "vector.h"

inline vec2 vec2_vec2i( const vec2i *B ) {
    vec2 v = { (f32)B->x, (f32)B->y };
    return v;
}

inline void vec2_cpy( vec2 *A, const vec2 *B ) {
    memcpy( &A->x, &B->x, 2 * sizeof( f32 ) );
}

inline void vec2i_cpy( vec2i *A, const vec2i *B ) {
    memcpy( &A->x, &B->x, 2 * sizeof( int ) );
}

inline vec2 vec2_neg( const vec2 *A ) {
    vec2 ret = { .x = -A->x, .y = -A->y };
    return ret;
}

inline vec2i vec2i_neg( const vec2i *A ) {
    vec2i ret = { .x = -A->x, .y = -A->y };
    return ret;
}

inline vec2 vec2_add( const vec2 *A, const vec2 *B ) {
    vec2 ret = { .x = A->x + B->x, .y = A->y + B->y };
    return ret;
}

inline vec2i vec2i_add( const vec2i *A, const vec2i *B ) {
    vec2i ret = { .x = A->x + B->x, .y = A->y + B->y };
    return ret;
}

inline vec2 vec2_sub( const vec2 *A, const vec2 *B ) {
    vec2 ret = { .x = A->x - B->x, .y = A->y - B->y };
    return ret;
}

inline vec2i vec2i_sub( const vec2i *A, const vec2i *B ) {
    vec2i ret = { .x = A->x - B->x, .y = A->y - B->y };
    return ret;
}

inline vec2 vec2_mulv( const vec2 *A, const vec2 *B ) {
    vec2 ret = { .x = A->x * B->x, .y = A->y * B->y };
    return ret;
}

inline vec2i vec2i_mulv( const vec2i *A, const vec2i *B ) {
    vec2i ret = { .x = A->x * B->x, .y = A->y * B->y };
    return ret;
}

inline vec2 vec2_mul( const vec2 *A, f32 pFactor ) {
    vec2 ret = { .x = A->x * pFactor, .y = A->y * pFactor };;
    return ret;
}

inline vec2i vec2i_mul( const vec2i *A, int pFactor ) {
    vec2i ret = { .x = A->x * pFactor, .y = A->y * pFactor };;
    return ret;
}

inline vec2 vec2_div( const vec2 *A, f32 pFactor ) {
    vec2 ret = { .x = A->x / pFactor, .y = A->y / pFactor };;
    return ret;
}

inline vec2i vec2i_div( const vec2i *A, int pFactor ) {
    vec2i ret = { .x = A->x / pFactor, .y = A->y / pFactor };;
    return ret;
}


inline f32  vec2_len( const vec2 *A ) {
    return sqrtf( vec2_sqlen( A ) );
}

inline f32  vec2_sqlen( const vec2 *A ) {
    return A->x * A->x + A->y * A->y;
}


inline f32  vec2_dot( const vec2 *A, const vec2 *B ) {
    return A->x * B->y + A->y * B->y;
}

inline void vec2_normalize( vec2 *A ) {
    f32 len = vec2_len( A );
    if( len > M_EPS ) 
        *A = vec2_div( A, len );
}


