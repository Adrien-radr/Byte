#ifndef BYTE_ANIM_H
#define BYTE_ANIM_H

#include "common/common.h"

typedef enum {
    MAN_IDLE_SE,
    MAN_IDLE_SW,

    ANIM_N
} Animation;

extern str32 Animation_str[ANIM_N];

typedef struct {
    vec2    *frames;    ///< st coords of top left corner of frame
    f32     *frame_t;   ///< Duration of each frame

    int     frame_n;    ///< Number of frames
    int     curr_n;     ///< Current frame to display

    bool    running;
    f32     curr_t;     ///< Current accumulated time
} Anim;

void Anim_loadFromFile( Anim *a, const char *file );
void Anim_destroy( Anim *a );
void Anim_cpy( Anim *a, const Anim *b );
bool Anim_update( Anim *a, f32 frame_time );

void Anim_start( Anim *a );
void Anim_stop( Anim *a );
void Anim_restart( Anim *a );
void Anim_pause( Anim *a );
void Anim_toggle( Anim *a );


extern str32 AnimDirectory;

typedef struct {
    Anim    animations[ANIM_N];
} AnimManager;

void AnimManager_loadAll( AnimManager *am );
void AnimManager_unloadAll( AnimManager *am );
Anim *AnimManager_get( AnimManager *am, Animation a );
Anim *AnimManager_gets( AnimManager *am, const char *anim_name );


#endif /* BYTE_ANIM */
