#include "wall.h"

inline void Wall_init( Wall *w, const vec2i *tile, MapDirection dir, f32 height ) {
    vec2i_cpy( &w->tile, tile );
    w->dir = dir;
    w->height = height;
}

