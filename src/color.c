#include "color.h"

void Color_rgb( const Color *pColor, f32 pTab[3] ) {
    pTab[0] = pColor->r;
    pTab[1] = pColor->g;
    pTab[2] = pColor->b;
}

void Color_rgba( const Color *pColor, f32 pTab[4] ) {
    pTab[0] = pColor->r;
    pTab[1] = pColor->g;
    pTab[2] = pColor->b;
    pTab[3] = pColor->a;
}

bool Color_cmp( const Color *pColA, const Color *pColB ) {
    return Eq( pColA->r, pColB->r, M_EPS ) && Eq( pColA->g, pColB->g, M_EPS ) && Eq( pColA->b, pColB->b, M_EPS ) && Eq( pColA->a, pColB->a, M_EPS );
}

const Color Black = { .r = 0.f, .g = 0.f, .b = 0.f, .a = 1.f };
const Color White = { .r = 1.f, .g = 1.f, .b = 1.f, .a = 1.f };
const Color Grey = { .r = 0.5f, .g = 0.5f, .b = 0.5f, .a = 1.f };
const Color Red = { .r = 1.f, .g = 0.f, .b = 0.f, .a = 1.f };
const Color Blue = { .r = 0.f, .g = 0.f, .b = 1.f, .a = 1.f };
const Color Green = { .r = 0.f, .g = 1.f, .b = 0.f, .a = 1.f };
const Color Cyan = { .r = 0.f, .g = 1.f, .b = 1.f, .a = 1.f };
const Color Magenta = { .r = 1.f, .g = 0.f, .b = 1.f, .a = 1.f };
const Color Yellow = { .r = 1.f, .g = 1.f, .b = 0.f, .a = 1.f };
const Color Orange = { .r = 1.f, .g = 0.5f, .b = 0.f, .a = 1.f };

