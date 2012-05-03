#include "map.h"

void Map_init( map *m ) {
    if( m ) {
        for( int i = 0; i < LOCAL_MAP_SIZE; ++i ) {
            m->tiles[i].walkable = true;
            // other ?? IF NOT, memset would be better
        }
    }
}

void Map_setWalkable( map *m, u32 i, u32 j, bool walkable ) {
    if( m && i < LOCAL_MAP_WIDTH*2 && j < LOCAL_MAP_HEIGHT/2 ) 
        m->tiles[j*2*LOCAL_MAP_WIDTH+i].walkable = walkable;
}

bool Map_isWalkable( map *m, u32 i, u32 j ) {
    if( m && i < LOCAL_MAP_WIDTH*2 && j < LOCAL_MAP_HEIGHT/2 ) 
        return m->tiles[j*2*LOCAL_MAP_WIDTH+i].walkable;
    return false;
}
