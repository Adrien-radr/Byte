#include "light.h"
#include "map.h"
#include "game.h"
#include "render/scene.h"

inline void Light_init( Light *l, const vec2i *pos, f32 radius, f32 height, const Color *col ) {
    if( l ) {
        vec2i_cpy( &l->tile, pos );
        Color_cpy( &l->diffuse, col );

        l->position = Map_isoToGlobal( &l->tile );
        l->radius = radius;
        l->height = height;
        l->active = true;
        l->scene_id = -1;
    }
}

inline void Light_setPosition( Light *l, const vec2 *pos ) {
    vec2_cpy( &l->position, pos );
    l->tile = Map_globalToIso( pos );

    // update light for shaders, if present in scene
    if( l->scene_id >= 0 ) 
        Scene_modifyLight( game->scene, l, LA_Position );
}

inline void Light_setLocation( Light *l, const vec2i *loc ) {
    vec2i_cpy( &l->tile, loc );
    l->position = Map_isoToGlobal( loc );

    // update light for shaders, if present in scene
    if( l->scene_id >= 0 ) 
        Scene_modifyLight( game->scene, l, LA_Position );
}

inline void Light_setColor( Light *l, const Color *color ) {
    memcpy( &l->diffuse, color, sizeof(Color) );

    // update light for shaders, if present in scene
    if( l->scene_id >= 0 ) 
        Scene_modifyLight( game->scene, l, LA_Color );
}

inline void Light_setRadius( Light *l, f32 radius ) {
    l->radius = radius;

    // update light for shaders, if present in scene
    if( l->scene_id >= 0 ) 
        Scene_modifyLight( game->scene, l, LA_Radius );
}

inline void Light_setHeight( Light *l, f32 height ) {
    l->height = height;

    // update light for shaders, if present in scene
    if( l->scene_id >= 0 ) 
        Scene_modifyLight( game->scene, l, LA_Height );
}
