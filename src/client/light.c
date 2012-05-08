#include "light.h"
#include "common/map.h"

void Light_set( Light *l, const vec2i *pos, f32 radius, f32 height, const Color *col, f32 cst, f32 lin, f32 quad ) {
    if( l ) {
        vec2i_cpy( &l->tile, pos );
        Color_cpy( &l->diffuse, col );

        l->position = Map_isoToGlobal( &l->tile );
        l->radius = radius;
        l->height = height;
        l->cst_att = cst;
        l->lin_att = lin;
        l->quad_att = quad;
    }
}
