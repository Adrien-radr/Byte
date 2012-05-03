#ifndef BYTE_MAP_H
#define BYTE_MAP_H

#include "common.h"

// Width and Height of local and world maps
// You got a nice quad if height = 2 * width
#define LOCAL_MAP_WIDTH  12     // Can be any size
#define LOCAL_MAP_HEIGHT 24     // Must be divisible by 2(int)
#define LOCAL_MAP_SIZE (LOCAL_MAP_WIDTH*LOCAL_MAP_HEIGHT)
#define WORLD_MAP_SIZE  10

typedef struct {
    bool    walkable;
} map_tile;

typedef struct {
    map_tile    tiles[LOCAL_MAP_WIDTH*LOCAL_MAP_HEIGHT];
} map;

void Map_setWalkable( map *m, u32 i, u32 j, bool walkable );

bool Map_isWalkable( map *m, u32 i, u32 j );

#endif // BYTE_MAP_H
