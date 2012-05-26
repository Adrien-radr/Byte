#include "light.h"
#include "map.h"

void Light_set( Light *l, const vec2i *pos, f32 radius, f32 height, const Color *col ) {
    if( l ) {
        vec2i_cpy( &l->tile, pos );
        Color_cpy( &l->diffuse, col );

        l->position = Map_isoToGlobal( &l->tile );
        l->radius = radius;
        l->height = height;
        l->active = true;
    }
}
