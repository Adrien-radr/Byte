#ifndef BYTE_LIGHT_H
#define BYTE_LIGHT_H

#include "common/color.h"
#include "common/vector.h"

typedef struct {
    vec2i   tile;       ///< Location on map
    vec2    position;   ///< Global absolute position
    f32     height;     ///< Height on map tile

    Color   diffuse;

    f32     radius;

    bool    active;
} Light;

void Light_set( Light *l, const vec2i *pos, f32 radius, f32 height, const Color *col );


#endif // BYTE_LIGHT
