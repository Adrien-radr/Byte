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
    if( !Widget_callback( root->widget, e ) ) {
        if( e->type == EMouseReleased ) {
            if( e->button == MB_Left ) {
                vec2i dest = Scene_screenToIso( game->scene, &e->v );

                // create path
                /*
                f32 begin = Game_getElapsedTime();

                if( p ) {
                    Map_destroyPath( p );
                    path_index = 1;
                    up_time = 0.f;
                }

                p = Map_createPath( &game->world->active_map, &a0->location, &dest );
                */
                Agent_moveTo( a0, &dest );

                //printf( "Path creation time : %f\n", Game_getElapsedTime() - begin );
            } else if( e->button == MB_Right ) {
            }
        }
    }
}


void init_callback() {
    /*
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
*/

    a0_h = World_loadAgent( game->world, "data/game/agent0.json", 0,0 );
    if( a0_h >= 0 ) {
        a0 = World_getAgent( game->world, a0_h, 0, 0 );

        //Agent_setLocation( a0, &(vec2i){ 3,3 } );
    } else
        log_info( "Unable to load agent 0!!\n" );

    // GUI
    Font* f = Font_get( "DejaVuSans.ttf", 12 );
    Color c;
    Color_set( &c, 1.f, 1.f, 1.f, 1.f );

    root = RootWidget_init();

    window = Widget_init( WT_Window, &(vec2i){ 300, 250 }, "quadmesh.json", "widgettexture.png", -1 );
    window->position = vec2i_c( 100, 100 );
    window->size = vec2i_c( 300, 250 );
    window->depth = -5;
    window->minSize = vec2i_c( 300, 120 );

    openWindow = Widget_init( WT_Button, &(vec2i){ 15, 15 }, "quadmesh.json", "winicon.png", -1 );
    vec2i size = Context_getSize();
    openWindow->position = vec2i_c( size.x - 15, 0 );
    openWindow->callback = &openWindowCallback;

    int winName = Scene_addText( game->scene, f, c );
    Scene_modifyText( game->scene, winName, TA_String, "Window" );

    windowHead = Widget_createWindowHead( window, winName );

    int buttonText = Scene_addText( game->scene, f, c );
    Scene_modifyText( game->scene, buttonText, TA_String, "Button" );

    button = Widget_init( WT_Button, &(vec2i){ 85, 25 }, "quadmesh.json", "widgettexture.png", buttonText );
    button->textOffset = vec2i_c( 20, 5 );
    button->callback = &button1Callback;

    topleft = Widget_init( WT_Button, &(vec2i){ 85, 25 }, "quadmesh.json", "widgettexture.png", -1 );
    topleft->textOffset = vec2i_c( 72, 11 );
    topcenter = Widget_init( WT_Button, &(vec2i){ 85, 25 }, "quadmesh.json", "widgettexture.png", -1 );
    topcenter->textOffset = vec2i_c( 72, 11 );
    topright = Widget_init( WT_Button, &(vec2i){ 85, 25 }, "quadmesh.json", "widgettexture.png", -1 );
    topright->textOffset = vec2i_c( 72, 11 );
    centerleft = Widget_init( WT_Button, &(vec2i){ 85, 25 }, "quadmesh.json", "widgettexture.png", -1 );
    centerleft->textOffset = vec2i_c( 72, 11 );
    center = Widget_init( WT_Button, &(vec2i){ 85, 25 }, "quadmesh.json", "widgettexture.png", -1 );
    center->textOffset = vec2i_c( 72, 11 );
    centerright = Widget_init( WT_Button, &(vec2i){ 85, 25 }, "quadmesh.json", "widgettexture.png", -1 );
    centerright->textOffset = vec2i_c( 72, 11 );
    bottomleft = Widget_init( WT_Button, &(vec2i){ 85, 25 }, "quadmesh.json", "widgettexture.png", -1 );
    bottomleft->textOffset = vec2i_c( 72, 11 );
    bottomcenter = Widget_init( WT_Button, &(vec2i){ 85, 25 }, "quadmesh.json", "widgettexture.png", -1 );
    bottomcenter->textOffset = vec2i_c( 72, 11 );


    window->callback = &window1Callback;

    Widget_addChild( root->widget, windowHead, false, WA_Center, true );
    Widget_addChild( root->widget, openWindow, false, WA_None, true );
    Widget_addChild( window, button, true, WA_BottomRight, true );
    Widget_addChild( window, topleft, true, WA_TopLeft, true );
    Widget_addChild( window, topcenter, true, WA_TopCenter, true );
    Widget_addChild( window, topright, true, WA_TopRight, true );
    Widget_addChild( window, centerleft, true, WA_CenterLeft, true );
    Widget_addChild( window, center, true, WA_Center, true );
    Widget_addChild( window, centerright, true, WA_CenterRight, true );
    Widget_addChild( window, bottomleft, true, WA_BottomLeft, true );
    Widget_addChild( window, bottomcenter, true, WA_BottomCenter, true );


    EventManager_addListener( LT_MouseListener, mousecb, NULL );

    // lights
    Color diffuse = { 1.5f, 0.4f, 0.3f, 1.f };

    Light_init( &lights[0], &(vec2i){12,5}, 400.f, 200.f, &diffuse );

    diffuse = (Color){ 0.3f, 0.4f, 1.f, 1.f };
    Light_init( &lights[1], &(vec2i){17,8}, 300.f, 200.f, &diffuse );

    diffuse = (Color){ 1.f, 1.f, 1.f, 1.f };
    Light_init( &lights[2], &(vec2i){8,8}, 450.f, 200.f, &diffuse );

    Scene_addLight( game->scene, &lights[0] );
    Scene_addLight( game->scene, &lights[1] );
    Scene_addLight( game->scene, &lights[2] );
}

bool frame_callback( f32 frame_time ) {
    if( p ) {
        // path step
        if( up_time >= 0.15f ) {
            vec2i *next = Map_getPathNode( p, path_index++ );

            vec2i old_wt = (vec2i){ a0->world_tile.x, a0->world_tile.y };
            Agent_setLocation( a0, next );

            // TO BE DONE IN A PLAYER CLASS OR SMTHG
            // check if player moved to another world tile and update
            // 3x3 window frame if he did
            if( !vec2i_eq( &a0->world_tile, &old_wt ) ) {
                World_activeMapLocation( game->world, a0->world_tile.x-1, a0->world_tile.y-1 );
                Scene_setLocation( game->scene, a0->world_tile.x-1, a0->world_tile.y-1 );
            }

            // move view
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

