#ifndef BYTE_MAP_H
#define BYTE_MAP_H

#include "common/common.h"

#define WORLD_MAP_SIZE  10

// Width and Height of local and world maps
// You got a nice quad if height = 3 * width + width/2
#define lmap_width 24    // Can be any size
#define lmap_height 84   // Must be divisible by 2(int)
#define lmap_size (lmap_width*lmap_height)

extern const int tile_w;        ///< Tile width
extern const int tile_h;        ///< Tile height
extern const int tile_hw;       ///< Tile half width
extern const int tile_hh;       ///< Tile half height


typedef enum {
    NW = 1,
    NE = 2,
    SW = 4,
    SE = 8
} MapDirection;

typedef struct {
    u32     walkable;           ///< Flag of each edge walkability
    //bool    walkable[4];        ///< Can walk in dir ? { NW, NE, SW, SE }
} MapTile;

typedef struct {
    MapTile    tiles[lmap_size];
} Map;

/// Initialize map tiles to all be walkable (what else?)
void Map_init( Map *map );

/// Set the direction(s) from a tile walkable or not
void Map_setWalkable( Map *map, const vec2i *tile, MapDirection dir, bool walkable );

/// Returns whether or not the given direction is walkable from the given tile
bool Map_isWalkable( const Map *map, const vec2i *tile, MapDirection dir );

/// Returns the square coordinate equivalent of an isometric coordinate
vec2i Map_isoToSquare( const vec2i *vec );

/// Returns the global floating position of the center of a tile
vec2  Map_isoToGlobal( const vec2i *tile );

/// Returns the iso tile under a global position
vec2i Map_globalToIso( const vec2 *pos );

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
