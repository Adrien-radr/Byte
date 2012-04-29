#ifndef BYTE_ACTOR_HPP
#define BYTE_ACTOR_HPP

#include "common.h"
#include "matrix.h"

typedef struct {
    // base
    str32                   mFirstname,     ///< Actor first name
                            mLastname,      ///< Actor last name
                            mSurname;       ///< Actor surname

    vec2                    mPosition;      ///< Actor position on map (absolute)


    // Rendering stuff
    struct {
        str256  mesh;
        str256  texture[2];             ///< 0 : albedo. 1 : heightmap
        int     tex_n;
    }                       assets;


        int     used_sprite;            ///< Sprite used to render the actor (-1 if none)
        int     mesh_id,                ///< Mesh Resource ID used by the actor
                texture_ids[2];         ///< Texture resource ID used by the actor
        vec2    size;                   ///< Sprite size
} Actor;

/// Loads an actor from a JSON file and extract all components
bool Actor_load( Actor *pActor, const char *pFile );

/// Move an actor by a vector, and correctly update its sprite if one exist
void Actor_move( Actor *pActor, vec2 *v );

/// Set the position of an actor and correctly update its sprite if one exist
void Actor_setPositionfv( Actor *pActor, vec2 *v );
void Actor_setPositionf( Actor *pActor, f32 x, f32 y );

#endif // BYTE_ACTOR
