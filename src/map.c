#include "map.h"
 
//////////////////
//  Map constants 
const int tile_w = 100;
const int tile_h = 50;
const int tile_hw = 50;
const int tile_hh = 25;

void Map_init( Map *map ) {
    if( map ) {
        for( int i = 0; i < lmap_size; ++i ) {
            map->tiles[i].walkable = true;
            // other ?? IF NOT, memset would be better
        }
    }
}

inline void Map_setWalkable( Map *map, const vec2i *tile, bool walkable ) {
    if( map && tile->x >= 0 && tile->y >= 0 && tile->x < lmap_width*2 && tile->y < lmap_height/2 ) 
        map->tiles[tile->y*2*lmap_width+tile->x].walkable = walkable;
}

inline bool Map_isWalkable( const Map *map, const vec2i *tile ) {
    if( map && tile->x >= 0 && tile->y >= 0 && tile->x < lmap_width*2 && tile->y < lmap_height/2 ) 
        return map->tiles[tile->y*2*lmap_width+tile->x].walkable;
    return false;
}

inline vec2i Map_isoToSquare( const vec2i *vec ) {
    // X :  y + (x '+ 1')    ( the +1 is here only if x is ODD )
    //          ---------    
    //              2        
    //                       
    // Y :  y - (x '- 1')    ( the -1 is here only if x is ODD )
    //          ---------    
    //              2        
    return (vec2i){ vec->y + ( vec->x + (vec->x % 2 ? 1 : 0) ) / 2,
                    vec->y - ( vec->x - (vec->x % 2 ? 1 : 0) ) / 2  };
}

inline vec2  Map_isoToGlobal( const vec2i *tile ) {
    return (vec2){ (tile->x+1) * tile_hw, 
                    tile->y * tile_h + (1+((tile->x % 2) ? 1 : 0)) * tile_hh };
}




// ##################################################################
//      A STAR Pathfinding
// ##################################################################

struct Path_ {
    u32     count;      ///< Number of nodes in path
    f32     cost;       ///< Total path cost
    vec2i   *nodes;     ///< Array of nodes in the path
};

typedef struct {
    u32     capacity;
    u32     count;
    f32     *costs;
    vec2i   *nodes;
} NeighborList;

typedef struct {
    bool        is_closed;              ///< Is in Closed List
    bool        is_open;                ///< Is in Open List
    bool        is_goal;                ///< Is the path goal
    bool        has_parent;
    bool        has_estimated_cost;     

    f32         cost;                   ///< Actual cost (G)
    f32         estimated_cost;         ///< Estimated cost by heuristic (H)
    u32         open_index;             ///< Index in open list (if in)
    u32         parent_index;           ///< Index of parent node
    vec2i       tile;                   ///< Node data (map tile)
} NodeRecord;

typedef struct {
    u32         node_records_capacity;
    u32         node_records_count;
    u32         *node_records_index;    ///< array of indices of NodeRecords
                                        ///< sorted by NodeRecord->key
    NodeRecord  *node_records;          ///< array of NodeRecords, indexed by ^

    u32         open_list_capacity;
    u32         open_list_count;
    u32         *open_list;             ///< binary heap of NodeRecord indices.
                                        ///< sorted by NodeRecord->F
} VisitedNodes;

typedef struct {
    VisitedNodes    *vn;                ///< pointer on the active VisitedNodes
    u32             index;              ///< Node index
} Node;

static Node NullNode = { NULL, -1 };




// ##################################################################
//      NEIGHBOR LIST

static inline NeighborList *NeighborList_init() {
    NeighborList *l = byte_alloc( sizeof(NeighborList) );

    l->capacity = 10;
    l->costs = byte_alloc( l->capacity * sizeof(f32) );
    l->nodes = byte_alloc( l->capacity * sizeof(vec2i) );
    return l;
}

static inline void NeighborList_destroy( NeighborList *l ) {
    DEL_PTR( l->nodes );
    DEL_PTR( l->costs );
    DEL_PTR( l );
}

static inline void NeighborList_add( NeighborList *nl, const vec2i *node, f32 cost ) {
    // resize list if full
    if( nl->count == nl->capacity ) {
        nl->capacity *= 2;
        nl->costs = byte_realloc( nl->costs, nl->capacity * sizeof(f32) );
        nl->nodes = byte_realloc( nl->nodes, nl->capacity * sizeof(vec2i) );
    }

    // set cost and node position of the newly added node
    nl->costs[nl->count] = cost;
    vec2i_cpy( &nl->nodes[nl->count], node );
    ++nl->count;
}

static inline f32  NeighborList_getCostTo( NeighborList *l, u32 i ) {
    return l->costs[i];
}

static inline const vec2i *NeighborList_getTile( NeighborList *l, u32 i ) {
    return &l->nodes[i];
}


// ##################################################################
//      MAP Dependant functions

/// Add the given node (map tile) neighbors that are walkable
static void pathNodeNeighbors( NeighborList *nl, const Map *map, const vec2i *node ) {
    const int offset = (node->x%2 == 0) ? 1 : 0;

    // Add a little shifting in the order of added nodes, for the path to seems more 'organic'
    static int curr_n = 0;       // index in following array

    // array of neighbors to check and possibly add.
    vec2i neighbors[] = {
        { node->x + 1, node->y + (1-offset) },  // right node
        { node->x - 1, node->y - offset },      // left node
        { node->x + 1, node->y - offset },      // above node
        { node->x - 1, node->y + (1-offset) }   // bellow node
    }; 

    if( Map_isWalkable( map, &neighbors[curr_n] ) )  
        NeighborList_add( nl, &neighbors[curr_n], 1 );

    curr_n = (curr_n+1) % 4;

    if( Map_isWalkable( map, &neighbors[curr_n] ) )  
        NeighborList_add( nl, &neighbors[curr_n], 1 );

    curr_n = (curr_n+1) % 4;

    if( Map_isWalkable( map, &neighbors[curr_n] ) )  
        NeighborList_add( nl, &neighbors[curr_n], 1 );

    curr_n = (curr_n+1) % 4;

    if( Map_isWalkable( map, &neighbors[curr_n] ) )  
        NeighborList_add( nl, &neighbors[curr_n], 1 );
}

/// Manhattan heuristic between two nodes 
static inline float pathNodeHeuristic( const vec2i *from, const vec2i *to ) {
    vec2i ito = Map_isoToSquare( to );
    vec2i ifrom = Map_isoToSquare( from );

    return abs( ito.x - ifrom.x ) + abs( ito.y - ifrom.y );
}




// ##################################################################
//      VISITED NODES
static inline VisitedNodes *VisitedNodes_init() {
    VisitedNodes *vn = byte_alloc( sizeof(VisitedNodes) );

    vn->node_records_capacity = 10;
    vn->node_records_index = byte_alloc( vn->node_records_capacity * sizeof(u32) );
    vn->node_records = byte_alloc( vn->node_records_capacity * sizeof(NodeRecord) );

    vn->open_list_capacity = 10;
    vn->open_list = byte_alloc( vn->open_list_capacity * sizeof(u32) );
    return vn;
}

static inline void VisitedNodes_destroy( VisitedNodes *vn ) {
    DEL_PTR( vn->node_records );
    DEL_PTR( vn->node_records_index );
    DEL_PTR( vn->open_list );
    DEL_PTR( vn );
}




// ##################################################################
//      NODE FUNCTIONS

static inline bool Node_isNull( Node *n ) {
    return ( n->index == NullNode.index ) && ( n->vn == NullNode.vn );
}

static inline NodeRecord *Node_getRecord( Node *n ) {
    return &n->vn->node_records[n->index]; 
}

static inline bool Node_isGoal( Node n ) {
    return Node_getRecord( &n )->is_goal;
}

static inline vec2i *Node_getTile( Node *n ) {
    return &n->vn->node_records[n->index].tile;
}

static inline Node Node_getFirstOpen( VisitedNodes *vn ) {
    return (Node){ vn, vn->open_list[0] };
}

static inline bool Node_hasEstimatedCost( Node *n ) {
    return Node_getRecord( n )->has_estimated_cost;
}

static inline Node Node_getParent( Node *n ) {
    NodeRecord *rec = Node_getRecord( n );
    if( rec->has_parent )
        return (Node){ n->vn, rec->parent_index };
    return NullNode;
}

static Node Node_get( VisitedNodes *vn, const vec2i *node ) {
    // first, try to find it in array
    u32 first = 0;

    if( vn->node_records_count > 0 ) {
        // binary heap insertion if not found, if found, return it
        u32 last = vn->node_records_count - 1;

        while( first <= last ){
            const u32 mid = (first + last) / 2;
            // comparison with memcmp
            const int cmp = memcmp( &vn->node_records[vn->node_records_index[mid]].tile, node, sizeof(vec2i) );

            // if the mid node is smaller than ours, continue in right array side
            if( cmp < 0 )
                first = mid + 1;
            // if the mid node is greater and we are not at 0, continue in left side
            else if( cmp > 0 && mid > 0 )
                last = mid - 1;
            // if mid node is greater and is at index 0. Insert in place
            else if( cmp > 0 )
                break;
            // else, we found it !
            else
                return (Node){ vn, vn->node_records_index[mid] };
        }
    }

    // NOT FOUND, insert it at index 'first', where the search stopped

    // resize arrays if full
    if( vn->node_records_count == vn->node_records_capacity ) {
        vn->node_records_capacity *= 2;
        vn->node_records = byte_realloc( vn->node_records, sizeof(NodeRecord) * vn->node_records_capacity );
        vn->node_records_index = byte_realloc( vn->node_records_index, sizeof(u32) * vn->node_records_capacity );
    }

    // create new node for it
    Node ret = (Node){ vn, vn->node_records_count++ };

    // move all node indices to the right (from 'first')
    memmove( &vn->node_records_index[first+1], &vn->node_records_index[first],
              ( vn->node_records_capacity - first - 1 ) * sizeof(u32) );
    
    // put our node in the 'first' lace
    vn->node_records_index[first] = ret.index;

    // initialize this new node record
    NodeRecord *rec = Node_getRecord( &ret );
    memset( rec, 0, sizeof(NodeRecord) );
    vec2i_cpy( &rec->tile, node );

    return ret;
}

static inline void Node_setGoal( Node *n ) {
    if( !Node_isNull( n ) )
        Node_getRecord( n )->is_goal = true;
}

static inline void Node_setEstimatedCost( Node *n, f32 cost ) {
    NodeRecord *rec = Node_getRecord( n );
    rec->has_estimated_cost = true;
    rec->estimated_cost = cost;
}

static inline float Node_applyHeuristic( Node *a, Node *b ) {
    if( !Node_isNull( a ) && !Node_isNull( b ) )
        return pathNodeHeuristic( Node_getTile( a ), Node_getTile( b ) );
    return 0.f;
}

static inline f32  Node_getCost( Node *n ) {
    return Node_getRecord( n )->cost;
}

static inline f32  Node_getEstimatedCost( Node *n ) {
    return Node_getRecord( n )->estimated_cost;
}

// return the rank : F = G + H
static inline f32  Node_getRank( Node *n ) {
    NodeRecord *rec = Node_getRecord( n );
    return rec->cost + rec->estimated_cost;
}

static inline int  Node_compareRank( Node *a, Node *b ) {
    const f32 rank_a = Node_getRank( a );
    const f32 rank_b = Node_getRank( b );

    if( rank_a < rank_b ) return -1;
    else if( rank_a > rank_b ) return 1;
    else return 0;
}

static inline void Node_swapOpenNodes( VisitedNodes *vn, u32 i, u32 j ) {
    if( i != j ) {
        NodeRecord *reci = Node_getRecord( &(Node){ vn, vn->open_list[i] } );
        NodeRecord *recj = Node_getRecord( &(Node){ vn, vn->open_list[j] } );

        u32 tmp = reci->open_index;
        reci->open_index = recj->open_index;
        recj->open_index = tmp;

        tmp = vn->open_list[i];
        vn->open_list[i] = vn->open_list[j];
        vn->open_list[j] = tmp;
    }
}

static void Node_addToOpenList( Node *n, Node *parent, f32 cost ) {
    NodeRecord *rec = Node_getRecord( n );

    if( !Node_isNull( parent ) ) {
        rec->has_parent = true;
        rec->parent_index = parent->index;
    } else 
        rec->has_parent = false;

    // resize Open List if full
    if( n->vn->open_list_count == n->vn->open_list_capacity ) {
        n->vn->open_list_capacity *= 2;
        n->vn->open_list = byte_realloc( n->vn->open_list, n->vn->open_list_capacity * sizeof(u32) );
    }

    // index of insertion (back of array at first)
    u32 i = n->vn->open_list_count++;

    // insertion
    n->vn->open_list[i] = n->index;
    rec->is_open = true;
    rec->open_index = i;
    rec->cost = cost;

    // Sort Open List (binary heap sort), by their ranks (F = G + H)
    while( i > 0 ) {
        const u32 parent_i = floorf( (i-1) / 2 );

        // if parent node has a greater rank than node, sorting finished!
        if( Node_compareRank( &(Node){ n->vn, n->vn->open_list[parent_i] },
                              &(Node){ n->vn, n->vn->open_list[i] } ) < 0 ) {
            break;
        // else, swap the two and continue
        } else {
            Node_swapOpenNodes( n->vn, parent_i, i );
            i = parent_i;
        }
    }
}

static void Node_removeFromOpenList( Node *n ) {
    NodeRecord *rec = Node_getRecord( n );

    if( rec->is_open ) {
        rec->is_open = false;
        n->vn->open_list_count--;

        u32 i = rec->open_index;

        // put the last list item(low F) to the slot where Node n was.
        Node_swapOpenNodes( n->vn, i, n->vn->open_list_count );

        // re-sort heap, try to put this low F item as far as possible in array
        u32 smallest = i;
        do {
            // when we find a smaller item than Node i, swap them and continue
            if( smallest != i ) {
                Node_swapOpenNodes( n->vn, smallest, i );
                i = smallest;
            }

            // get two children of node at index i
            const u32 left = ( 2 * i ) + 1;
            const u32 right = ( 2 * i ) + 2;

            // compare the Node at i, with its to children.
            // if a children is smaller than it
            if( left < n->vn->open_list_count && 
                Node_compareRank( &(Node){ n->vn, n->vn->open_list[left] },
                                  &(Node){ n->vn, n->vn->open_list[smallest] }) < 0 ) {
                smallest = left;
            }

            if( right < n->vn->open_list_count && 
                Node_compareRank( &(Node){ n->vn, n->vn->open_list[right] },
                                  &(Node){ n->vn, n->vn->open_list[smallest] }) < 0 ) {
                smallest = right;
            }
        } while( smallest != i );
    }
}

static inline bool Node_isInOpenList( Node *n ) {
    return Node_getRecord( n )->is_open;
}

static inline void Node_addToClosedList( Node *n ) {
    Node_getRecord( n )->is_closed = true;
}

static inline void Node_removeFromClosedList( Node *n ) {
    Node_getRecord( n )->is_closed = false;
}

static inline bool Node_isInClosedList( Node *n ) {
    return Node_getRecord( n )->is_closed;
}




// ##################################################################
//      A STAR : Public functions

Path *Map_createPath( const Map *map, const vec2i *start, const vec2i *end ) {
    if( !map || !start || !end )
        return NULL;

    // check if destination is off-bounds
    if( !Map_isWalkable( map, end ) )
        return NULL;

    NeighborList *nl = NeighborList_init();
    VisitedNodes *vn = VisitedNodes_init();

    // create (or get if existant) start and end node in array
    Node current = Node_get( vn, start );
    Node goal = Node_get( vn, end );

    // set ultimate goal
    Node_setGoal( &goal );

    Path *path = NULL;

    // begin algorithm by setting estimated cost from current to the goal Node
    Node_setEstimatedCost( &current, Node_applyHeuristic( &current, &goal ) );

    // Add current node to open list
    Node_addToOpenList( &current, &NullNode, 0 );



    // begin A* algorithm
    while( vn->open_list_count > 0 && 
            !Node_isGoal( (current=Node_getFirstOpen( vn )) ) ) {
        // while we have an open_list and the first node in it is not the goal :
    
        // CHECK EARLY EXIT !!!!

        Node_removeFromOpenList( &current );
        Node_addToClosedList( &current );

        // get all neighbors of current node
        nl->count = 0;
        pathNodeNeighbors( nl, map, Node_getTile( &current ) );

        for( u32 i = 0; i < nl->count; ++i ) {
            // get cost to go to this neighbor from current
            const f32 cost = Node_getCost( &current ) + NeighborList_getCostTo( nl, i );
            // get or create a node for this neighbor
            Node neighbor = Node_get( vn, NeighborList_getTile( nl, i ) );

            // If this neighbor does not have a estimated cost (H), do it
            if( !Node_hasEstimatedCost( &neighbor ) )
                Node_setEstimatedCost( &neighbor, Node_applyHeuristic( &neighbor, &goal ) );


            // If this neighbor is on openlist and cost to get to it from current
            // is smaller than cost it had, remove from open (better path)
            if( Node_isInOpenList( &neighbor ) && cost < Node_getCost( &neighbor ) )
                Node_removeFromOpenList( &neighbor );

            // Same here for closed list
            if( Node_isInClosedList( &neighbor ) && cost < Node_getCost( &neighbor ) )
                Node_removeFromClosedList( &neighbor );

            // If node not on closed or open list
            // Add it to openlist, with current as its parent, and the cost to get
            // to it from current, as its cost
            if( !Node_isInClosedList( &neighbor ) && !Node_isInOpenList( &neighbor ) )
                Node_addToOpenList( &neighbor, &current, cost );
        }
    }

    if( Node_isNull( &goal ) ) {
        Node_setGoal( &current );
        printf( "Goal node is null, setting it to current\n" );
    }

    // If we found the goal, reconstruct reverse path to it.
    if( Node_isGoal( current ) ) {
        Node n = current;
        u32 count = 0;

        // get node count till starting node
        while( !Node_isNull( &n ) ) {
            ++count;
            n = Node_getParent( &n );
        }

        // allocate our path
        path = byte_alloc( sizeof( Path ) );
        path->count = count;
        path->cost = Node_getCost( &current );
        path->nodes = byte_alloc( count * sizeof( vec2i ) );


        // recreate path and copy it in path->nodes (in the right order)
        n = current;
        for( u32 i = count; i > 0; --i ) {
            memcpy( &path->nodes[i-1], Node_getTile( &n ), sizeof(vec2i) );
            n = Node_getParent( &n );
        }

    }

    NeighborList_destroy( nl );
    VisitedNodes_destroy( vn );

    return path;
}

inline void Map_destroyPath( Path *path ) {
    if( path ) {
        DEL_PTR( path->nodes );
        DEL_PTR( path );
    }
}

inline f32  Map_getPathCost( const Path *path ) {
    return path ? path->cost : INFINITY;
}

inline u32  Map_getPathCount( const Path *path ) {
    return path ? path->count : 0;
}

inline vec2i *Map_getPathNode( const Path *path, u32 i ) {
    return (path && i < path->count) ? &path->nodes[i] : NULL;
}
