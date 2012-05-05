#ifndef BYTE_ACTOR_HPP
#define BYTE_ACTOR_HPP

#include "common.h"
#include "matrix.h"

typedef struct {
    str32                   firstname,      ///< Actor first name
                            lastname,       ///< Actor last name
                            surname;        ///< Actor surname

    vec2i                   position;       ///< Actor tile on map
    int                     used_sprite;    ///< Sprite ID in scene (-1 if none)

    // Rendering stuff
    struct {
        str256  mesh;           ///< Mesh filename
        int     mesh_id;        ///< Mesh resource ID
        vec2    mesh_size;      ///< Mesh/sprite size
        str256  texture[2];     ///< Texture filenames. 0 : albedo. 1 : heightmap
        int     tex_n;          ///< Number of textures used
        int     texture_id[2];  ///< Texture resource ID
    }                       assets;
} Actor;

/// Loads an actor from a JSON file and extract all components
bool Actor_load( Actor *actor, const char *file );

/// Move an actor by a vector, and correctly update its sprite if one exist
//void Actor_move( Actor *actor, vec2 *v );

/// Set the position of an actor and correctly update its sprite if one exist
//void Actor_setPositionfv( Actor *pActor, vec2 *v );
//void Actor_setPositionf( Actor *pActor, f32 x, f32 y );

#endif // BYTE_ACTOR
