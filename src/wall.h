#ifndef BYTE_WALL_H
#define BYTE_WALL_H

#include "map.h"
#include "render/static.h"

typedef struct {
    vec2i           tile;       ///< Tile where the wall is
    MapDirection    dir;        ///< Direction on this tile where it is located

    f32             height;     ///< Height of the wall

    StaticObject    assets;     ///< Assets data of the wall
} Wall;

void Wall_init( Wall *w, const vec2i *tile, MapDirection dir, f32 height );

#endif /* BYTE_WALL */
