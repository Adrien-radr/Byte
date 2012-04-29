#ifndef BYTE_LIGHT_H
#define BYTE_LIGHT_H

#include "common/color.h"
#include "common/vector.h"

typedef struct {
    vec2    position;   ///< Position on map (middle of a map tile)
    f32     height;     ///< Height on map tile

    Color   diffuse;

    f32     cst_att,
            lin_att,
            quad_att;
} Light;

void Light_set( Light *l, const vec2 *pos, f32 height, const Color *col, f32 cst, f32 lin, f32 quad );


#endif // BYTE_LIGHT
