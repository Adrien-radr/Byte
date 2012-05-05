#include "map.h"

void Map_init( Map *map ) {
    if( map ) {
        for( int i = 0; i < LOCAL_MAP_SIZE; ++i ) {
            map->tiles[i].walkable = true;
            // other ?? IF NOT, memset would be better
        }
    }
}

void Map_setWalkable( Map *map, const vec2i *tile, bool walkable ) {
    if( map && tile->x < LOCAL_MAP_WIDTH*2 && tile->y < LOCAL_MAP_HEIGHT/2 ) 
        map->tiles[tile->y*2*LOCAL_MAP_WIDTH+tile->x].walkable = walkable;
}

bool Map_isWalkable( Map *map, const vec2i *tile ) {
    if( map && tile->x < LOCAL_MAP_WIDTH*2 && tile->y < LOCAL_MAP_HEIGHT/2 ) 
        return map->tiles[tile->y*2*LOCAL_MAP_WIDTH+tile->x].walkable;
    return false;
}


// ##################################################################
//      A STAR Pathfinding
// ##################################################################

struct asPath_ {
    u32     count;      ///< Number of nodes in path
    f32     cost;       ///< Total path cost
    s8      nodes[];    ///< Array of nodes in the path
};

struct asOpenList_ {
    u32     capacity;
    u32     count;
    f32     *costs;
    vec2i   *nodes;
};

/// Add the given node (map tile) neighbors that are walkable
static void pathNodeNeighbors( asOpenList *ol, const Map *map, const vec2i *node ) {
    vec2i v = { node->x + 1, node->y + (node->x&1) };

    // check tile to the right
    if( Map_isWalkable( map, &v ) )
        Map_addNodeToOpenList( ol, &v );

    // check tile to the left
    v = vec2_c( node->x - 1, node->y - (node->x&0) );
    if( Map_isWalkable( map, &v ) )
        Map_addNodeToOpenList( ol, &v );

    // check tile above
    v = vec2_c( node->x + 1, node->y - (node->x&0) );
    if( Map_isWalkable( map, &v ) )
        Map_addNodeToOpenList( ol, &v );

    // check tile below
    v = vec2_c( node->x - 1, node->y + (node->x&1) );
    if( Map_isWalkable( map, &v ) )
        Map_addNodeToOpenList( ol, &v );
}

/// Manhattan heuristic between two nodes
static inline float pathNodeHeuristic( const vec2i *from, const vec2i *to ) {
    // get squared-tiles coordinates from iso-coordinates of vector TO
    // X :  y + (x '+ 1')    ( the +1 is here only if to->X is ODD )
    //          ---------    
    //              2        
    //                       
    // Y :  y - (x '- 1')    ( the -1 is here only if to->X is ODD )
    //          ---------    
    //              2        
    vec2i sq_tile = { to->y + ( to->x + (to->x&1) ) / 2, 
                      to->y - ( to->x - (to->x&1) ) / 2  };

    // return Manhattan distance between sq_tile and from
    return ( fabs( from->x - sq_tile.x ) + fabs( from->y - sq_tile.y ) );
}

inline f32 Map_getPathCost( const asPath *path ) {
    return path ? path->cost : INFINITY;
}

/// Return the number of nodes in the given path
u32  Map_getPathCount( const asPath *path ) {
    return path ? path->count : 0;
}
