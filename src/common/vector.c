#include "vector.h"

inline vec2  vec2_c( f32 x, f32 y ) {
    return (vec2){ x, y };
}

inline vec2i vec2i_c( int x, int y ) {
    return (vec2i){ x, y };
}

inline vec2 vec2_vec2i( const vec2i *B ) {
    return (vec2){ (f32)B->x, (f32)B->y };
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
    return (vec2i){ .x = -A->x, .y = -A->y };
}

inline vec2 vec2_add( const vec2 *A, const vec2 *B ) {
    return (vec2){ .x = A->x + B->x, .y = A->y + B->y };
}

inline vec2i vec2i_add( const vec2i *A, const vec2i *B ) {
    return (vec2i){ .x = A->x + B->x, .y = A->y + B->y };
}

inline vec2 vec2_sub( const vec2 *A, const vec2 *B ) {
    return (vec2){ .x = A->x - B->x, .y = A->y - B->y };
}

inline vec2i vec2i_sub( const vec2i *A, const vec2i *B ) {
    return (vec2i){ .x = A->x - B->x, .y = A->y - B->y };
}

inline vec2 vec2_mulv( const vec2 *A, const vec2 *B ) {
    return (vec2){ .x = A->x * B->x, .y = A->y * B->y };
}

inline vec2i vec2i_mulv( const vec2i *A, const vec2i *B ) {
    return (vec2i){ .x = A->x * B->x, .y = A->y * B->y };
}

inline vec2 vec2_mul( const vec2 *A, f32 pFactor ) {
    return (vec2){ .x = A->x * pFactor, .y = A->y * pFactor };
}

inline vec2i vec2i_mul( const vec2i *A, int pFactor ) {
    return (vec2i){ .x = A->x * pFactor, .y = A->y * pFactor };
}

inline vec2 vec2_div( const vec2 *A, f32 pFactor ) {
    return (vec2){ .x = A->x / pFactor, .y = A->y / pFactor };
}

inline vec2i vec2i_div( const vec2i *A, int pFactor ) {
    return (vec2i){ .x = A->x / pFactor, .y = A->y / pFactor };
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


