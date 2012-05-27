#include "game.h"
#include "map.h"


Path *p = NULL;
u32 path_index = 1;
f32 up_time = 0.f;

int master, button1;

int a_h[10];
Agent *agt[10];

int a0_h;
Agent *a0;

f32 test = 0.f;

Light lights[3];

void mousecb( const Event *e, void *data ) {
    if( e->type == EMouseReleased ) {
        if( e->button == MB_Right ) {
            vec2i dest = Scene_screenToIso( game->scene, &e->v );

            // create path
            f32 begin = Game_getElapsedTime();

            if( p ) {
                Map_destroyPath( p );
                path_index = 1;
                up_time = 0.f;
            }

            p = Map_createPath( &game->world->local_map, &a0->location, &dest );

            printf( "Path creation time : %f\n", Game_getElapsedTime() - begin );
        } else if( e->button == MB_Left ) {
            bool walkable = Map_isWalkable( &game->world->local_map, &game->mouse_tile, (NW|NE|SW|SE) );
            SceneMap_redTile( game->scene, &game->mouse_tile, walkable );
            Map_setWalkable( &game->world->local_map, &game->mouse_tile, (NW|NE|SW|SE), !walkable );
        }
    }
}


void init_callback() {
    for( int i = 0; i < 10; ++i ) {
        a_h[i] = World_loadAgent( game->world, "data/game/agent1.json" );
        if( a_h[i] >= 0 ) {
            agt[i] = World_getAgent( game->world, a_h[i] );

            Scene_addAgentSprite( game->scene, agt[i] );

            vec2i rand_pos = RandomVec2i( 3, 15 );
            Agent_setPosition( agt[i], &rand_pos );
        } else
            log_info( "Unable to load agent %d!!\n", i );
    }

    a0_h = World_loadAgent( game->world, "data/game/agent0.json" );
    if( a0_h >= 0 ) {
        a0 = World_getAgent( game->world, a0_h );

        Scene_addAgentSprite( game->scene, a0 );
        Agent_setPosition( a0, &(vec2i){ 12,5 } );
    } else
        log_info( "Unable to load agent 0!!\n" );

    // GUI
    Widget button;
    button.position = (vec2i){ 3, 3 };
    button.size = (vec2i){ 120, 50 };
    button.depth = -5;
    Widget_init( &button, "quadmesh.json", "widgettexture.png" );

    button1 = Scene_addWidget( game->scene, &button );

    EventManager_addListener( LT_MouseListener, mousecb, NULL );

    // lights
    Color diffuse = { 1.5f, 0.4f, 0.3f, 1.f };

    Light_init( &lights[0], &(vec2i){12,5}, 400.f, 200.f, &diffuse );

    diffuse = (Color){ 0.3f, 0.4f, 1.f, 1.f };
    Light_init( &lights[1], &(vec2i){17,8}, 300.f, 200.f, &diffuse );
        
    diffuse = (Color){ 1.f, 1.f, 1.f, 1.f };
    Light_init( &lights[2], &(vec2i){8,8}, 350.f, 200.f, &diffuse );

    Scene_addLight( game->scene, &lights[0] );
    Scene_addLight( game->scene, &lights[1] );
    Scene_addLight( game->scene, &lights[2] );
}
 
bool frame_callback( f32 frame_time ) {
    if( p ) {
        // path step
        if( up_time >= 0.15f ) {
            vec2i *next = Map_getPathNode( p, path_index++ );
            Agent_setPosition( a0, next );
            up_time = 0.f;
        } else
            up_time += frame_time;


        if( path_index == Map_getPathCount( p ) ) {
            Map_destroyPath( p );
            p = NULL;
            up_time = 0.f;
            path_index = 1;
        }
    }

    if( test >= 3.f ) {
        Agent_playAnimation( a0, MAN_IDLE_SE );
        test = -1.f;

        // light modif test
        Light_setLocation( &lights[2], &(vec2i){ 3,5 } );

    } else if( test >= 0.f ) {
        test += frame_time;
    }

    return true;
}

int main() {
    if( !Game_init( init_callback, frame_callback ) ) 
        return 1;

    Game_run();

    return 0;
}

