#ifndef BYTE_ACTOR_HPP
#define BYTE_ACTOR_HPP

#include "common.h"
#include "vector.h"

typedef struct {
    // base
    str32   mFirstname,
            mLastname,
            mSurname;

    vec2    mPosition;

    // rendering
    u32     mMesh_id,
            mTexture_id;
} Actor;

bool Actor_load( Actor *pActor, const char *pFile );

#endif // BYTE_ACTOR
