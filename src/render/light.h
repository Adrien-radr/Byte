#ifndef BYTE_LIGHT_H
#define BYTE_LIGHT_H

#include "common/color.h"
#include "common/vector.h"

/// Different attributes that can be changed on a light
typedef enum {
    LA_Position,
    LA_Color,
    LA_Radius,
    LA_Height
} LightAttribute;


typedef struct {
    vec2i   tile;       ///< Location on map
    vec2    position;   ///< Global absolute position
    f32     height;     ///< Height on map tile

    Color   diffuse;

    f32     radius;

    bool    active;
    int     scene_id;   ///< ID of light in scene (-1 if not in scene)
} Light;

void Light_init( Light *l, const vec2i *pos, f32 radius, f32 height, const Color *col );

void Light_setPosition( Light *l, const vec2 *pos );
void Light_setLocation( Light *l, const vec2i *loc );
void Light_setColor( Light *l, const Color *color );
void Light_setRadius( Light *l, f32 radius );
void Light_setHeight( Light *l, f32 height );


#endif // BYTE_LIGHT
