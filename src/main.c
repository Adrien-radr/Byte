#include "game.h"

#include "color.h"
#include "actor.h"

#include "GL/glew.h"

#define ACT_NUM 50

int main() {
    int return_val = -1;

    check( Game_init(), "Error while initializing Game. Exiting program!\n" );


    printf( "Hello, Byte World!!\n" );

    str64 date;
    str16 time;
    GetTime( date, 64, DateFmt );
    GetTime( time, 16, TimeFmt );

    strncat( date, " - ", 3 );
    strncat( date, time, 16 );
    printf( "%s\n\n", date );


    Actor actor1;
    check( Actor_load( &actor1, "data/actors/actor1.json" ), "Error while loading actor1!\n" );

    Actor actors[ACT_NUM*ACT_NUM];
    for( int i = 0; i < ACT_NUM*ACT_NUM; ++i )
        check( Actor_load( &actors[i], "data/actors/actor2.json" ), "Error while loading actors[%d]!\n", i );
    //Actor actor2;





    // ###############################34096
    //      Entities
    u32 ent2_depth = 5;

    int actor1_entity = Scene_addEntityFromActor( game->mScene, &actor1 );
    check( actor1_entity >= 0, "error creating actor1_entity!\n" );
    Scene_modifyEntity( game->mScene, actor1_entity, EA_Depth, &ent2_depth );

    int actor_ent;
    for( int i = 0; i < ACT_NUM; ++i ) {
        for( int j = 0; j < ACT_NUM; ++j ) {
            actor_ent = Scene_addEntityFromActor( game->mScene, &actors[i*ACT_NUM + j] );
            check( actor_ent >= 0, "error creating actor_ent(%d)!\n", i*ACT_NUM+j );
            Scene_modifyEntity( game->mScene, actor_ent, EA_Depth, &ent2_depth );
            mat3_translatef( &actors[i*ACT_NUM + j].mPosition, i * 10.f, j * 10.f );
        }
    }

    // ###############################3
    //      TEXT
    Font *f = Font_get( "DejaVuSans.ttf", 12 );
    Color col = { 0.6f, 0.6f, 0.6f, 1.f };
    Color col2 = { 1.f, 1.f, 0.5f, 1.f };
    int text = Scene_addText( game->mScene, f, col );

    check( text >= 0, "error creating text!\n" );

    vec2 textPos;
    textPos.x = 10;
    textPos.y = 10;

    Scene_modifyText( game->mScene, text, TA_String, "Hello World!!" );
    Scene_modifyText( game->mScene, text, TA_Position, &textPos );


    u32 mesh = -1;
    mesh =  World_getResource("widgetmesh.json");
    u32 tex = -1;
    tex = World_getResource("widgettexture.jpg");

    u32 mesh2 = -1;
    mesh2 =  World_getResource("widgetmesh2.json");
    u32 tex2 = -1;
    tex2 = World_getResource("crate.jpg");

    u32 mesh3 = -1;
    mesh3 =  World_getResource("widgetmesh2.json");


    mat3 widgetPos;
    mat3_identity(&widgetPos);
    mat3_translatef(&widgetPos, 50, 50);

    mat3 widgetPos2;
    mat3_identity(&widgetPos2);
    mat3_translatef(&widgetPos2, 100, 100);

    mat3 widgetPos3;
    mat3_identity(&widgetPos3);
    mat3_translatef(&widgetPos3, 300, 200);

    u32 depth = 0;

    int master = -1;
    master = Scene_addWidget( game->mScene, WT_Master, NULL, -1 );

    int widget = -1;
    WidgetButtonAttributes wba;
    wba.mMesh = mesh;
    wba.mTexture = tex;
    wba.mMM = &widgetPos;
    wba.mFont = f;
    wba.mColor = col;
    wba.mBounds.x = 70.f;
    wba.mBounds.y = 20.f;

    widget = Scene_addWidget( game->mScene, WT_Button, &wba, master );
    Scene_modifyWidget( game->mScene, widget, WA_String, "Button" );
    Scene_modifyWidget( game->mScene, widget, WA_TextPosition, &textPos );
    Scene_modifyWidget( game->mScene, widget, WA_Depth, &depth );

    int widget2 = -1;
    WidgetSpriteAttributes wsa;
    wsa.mMesh = mesh2;
    wsa.mTexture = tex2;
    wsa.mMM = &widgetPos2;
    wsa.mBounds.x = 70.f;
    wsa.mBounds.y = 20.f;
    widget = Scene_addWidget( game->mScene, WT_Sprite, &wsa, master );
    Scene_modifyWidget( game->mScene, widget2, WA_Depth, &depth );

    int widget3 = -1;
    WidgetButtonAttributes wba2;
    wba2.mMesh = mesh3;
    wba2.mTexture = tex;
    wba2.mMM = &widgetPos3;
    wba2.mFont = f;
    wba2.mColor = col;
    wba2.mBounds.x = 70.f;
    wba2.mBounds.y = 20.f;
    widget3 = Scene_addWidget( game->mScene, WT_Button, &wba2, master );
    Scene_modifyWidget( game->mScene, widget3, WA_String, "Button" );
    Scene_modifyWidget( game->mScene, widget3, WA_TextPosition, &textPos );
    //Scene_modifyWidget( game->mScene, widget3, WA_Matrix, &widgetPos3 );
    Scene_modifyWidget( game->mScene, widget3, WA_Depth, &depth );

    int widget4 = -1;
    WidgetTextAttributes wta;
    wta.mBounds.x = 20.f;
    wta.mBounds.y = 20.f;
    wta.mColor = col2;
    wta.mFont = f;
    wta.mPosition.x = 60.f;
    wta.mPosition.y = 120.f;

    widget4 = Scene_addWidget( game->mScene, WT_Text, &wta, master );
    Scene_modifyWidget( game->mScene, widget4, WA_String, "Text" );

    ////////////////////////////////////

    int cpt = 0;
    f32 accum = 0;
    f32 frame_time;

    f32 p_update = 0.f;
    f32 dt = 0.01f;     // IA/Physics updated at 100 FPS

    while( !IsKeyUp( K_Escape ) && Context_isWindowOpen() ) {
        Device_beginFrame();
            // Timer stuff
            frame_time = Device_getFrameTime();
            accum += frame_time;
            p_update += frame_time;


            // AI GAMEPLAY LOOP (fixed at 1/dt FPS)
            while( p_update >= dt ) {
                vec2 ent2_move = { 0, 0 };
                if( IsKeyDown( K_Up ) )
                    ent2_move.y -= 1;
                if( IsKeyDown( K_Down ) )
                    ent2_move.y += 1;
                if( IsKeyDown( K_Left ) )
                    ent2_move.x -= 1;
                if( IsKeyDown( K_Right ) )
                    ent2_move.x += 1;
                if( ent2_move.x != 0 || ent2_move.y != 0 ){
                    mat3_translatefv( &actor1.mPosition, &ent2_move );
                }
                Scene_update( game->mScene );

                p_update -= dt;
            }

            // Stuff hapenning each 1 second
            if( accum > 1.f ) {
                ++cpt;
                accum = 0.f;

                str64 fps_str;
                MSG( fps_str, 64, "FPS : %4.0f", (1.f/frame_time) );
                Scene_modifyText( game->mScene, text, TA_String, fps_str );
            }
            //Scene_updateWidgets( game->mScene, )
            // Render Frame
            Scene_render( game->mScene );
        Device_endFrame();
    }

    return_val = 0;

error :

    Game_destroy();
    return return_val;
}

