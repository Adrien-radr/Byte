#ifndef BYTE_COLOR_HPP
#define BYTE_COLOR_HPP

#include "common.h"

/// Typical Color structure 
typedef struct {
    f32 r, g, b, a;
} Color;

void Color_cpy( Color *A, const Color *B );
void Color_set( Color *col, f32 r, f32 g, f32 b, f32 a );

void Color_rgb( const Color *pColor, f32 tab[3] );
void Color_rgba( const Color *pColor, f32 tab[4] );

bool Color_cmp( const Color *pColA, const Color *pColB );

extern const Color Black;
extern const Color White;
extern const Color Grey;
extern const Color Red;
extern const Color Green;
extern const Color Blue;
extern const Color Cyan;
extern const Color Magenta;
extern const Color Yellow;
extern const Color Orange;

#endif
