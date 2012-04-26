#include "light.h"

void Light_set( light *l, const vec2 *pos, f32 height, const Color *col, f32 cst, f32 lin, f32 quad ) {
    if( l ) {
        vec2_cpy( &l->position, pos );
        Color_cpy( &l->diffuse, col );
        l->height = height;
        l->cst_att = cst;
        l->lin_att = lin;
        l->quad_att = quad;
    }
}
