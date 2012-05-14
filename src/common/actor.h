#ifndef BYTE_ACTOR_HPP
#define BYTE_ACTOR_HPP

#include "matrix.h"
#include "anim.h"

typedef struct {
    str32                   firstname,      ///< Actor first name
                            lastname,       ///< Actor last name
                            surname;        ///< Actor surname

    vec2i                   position;       ///< Actor tile on map
    int                     used_sprite;    ///< Sprite ID in scene (-1 if none)


    // Rendering stuff
    struct {
        str128  mesh;           ///< Mesh filename
        int     mesh_id;        ///< Mesh resource ID
        vec2    mesh_size;      ///< Mesh/sprite size

        str128  texture[2];     ///< Texture filenames. 0 : albedo. 1 : heightmap
        int     tex_n;          ///< Number of textures used
        int     texture_id[2];  ///< Texture resource ID
        vec2    texcoords_size; ///< Size of sprite on tex atlas [0..1]^2

        str128  anim_str; 
        Anim    *animation;
    }                       assets;
} Actor;

/// Loads an actor from a JSON file and extract all components
bool Actor_load( Actor *actor, const char *file );

#endif // BYTE_ACTOR
