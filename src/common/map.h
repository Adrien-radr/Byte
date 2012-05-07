#ifndef BYTE_MAP_H
#define BYTE_MAP_H

#include "common.h"
#include "vector.h"

// Width and Height of local and world maps
// You got a nice quad if height = 2 * width
#define LOCAL_MAP_WIDTH  12     // Can be any size
#define LOCAL_MAP_HEIGHT 24     // Must be divisible by 2(int)
#define LOCAL_MAP_SIZE (LOCAL_MAP_WIDTH*LOCAL_MAP_HEIGHT)
#define WORLD_MAP_SIZE  10

typedef struct {
    bool    walkable;
} MapTile;

typedef struct {
    MapTile    tiles[LOCAL_MAP_WIDTH*LOCAL_MAP_HEIGHT];
} Map;

/// Initialize map tiles to all be walkable (what else?)
void Map_init( Map *map );

/// Set a tile of the given map to be walkable or not
void Map_setWalkable( Map *map, const vec2i *tile, bool walkable );

/// Returns whether or not the given map tile is walkable
bool Map_isWalkable( const Map *map, const vec2i *tile );


// ##################################################################
//      A STAR Pathfinding
//
//  Uses a bineary heap to implement the priority queue, and an 
//  indexed array for fast lookups in the open list.
// ##################################################################

/// Forward decl. Definition in map.c
typedef struct Path_ Path;

/// Add a new node to the current NeighborList
//void Map_addNodeToNeighborList( asNeighborList *nl, const vec2i *node, f32 cost );

/// Create a new path from node start to node end.
/// Uses the map.tiles structures in iso format
Path *Map_createPath( const Map *m, const vec2i *start, const vec2i *end );

/// Destroy and free the given path
void Map_destroyPath( Path *path );

/// Return the total cost of the given path
f32  Map_getPathCost( const Path *path );

/// Return the number of nodes in the given path
u32  Map_getPathCount( const Path *path );

/// Return the node (vec2i) of a path, at a given index
vec2i *Map_getPathNode( const Path *path, u32 i );

#endif // BYTE_MAP_H
