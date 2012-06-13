#ifndef BYTE_MAP_H
#define BYTE_MAP_H

#include "common/common.h"

// Starting position on map
#define wmap_initX 0
#define wmap_initY 0

// Width and Height of local and world maps
// You got a nice quad if height = 3 * width + width/2
#define lmap_width 8     // Can be any size                         24
#define lmap_height 28   // Must be divisible by 2(int)             84
#define lmap_size (lmap_width*lmap_height)

#define wmap_width 5
#define wmap_height 5
#define wmap_size (wmap_width*wmap_height)

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
    u32     walkable;       ///< Flag of each edge walkability
} MapTile;

typedef struct {
    MapTile     *tiles;     ///< Array of all map tiles

    vec2i       size;       ///< Size of map (width,height)
} Map;

/// Initialize map to the given size (all walkable by def)
void Map_init( Map *map, u32 width, u32 height );

// free the given map's tiles
void Map_destroy( Map *map );

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

/// Returns the world tile inside which the given location is
vec2i Map_globalToWorld( const vec2i *loc );

/// Returns the global map tile at the top-left corner of the given world tile
vec2i Map_worldToGlobal( const vec2i *loc );

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
