#ifndef BYTE_STATIC_H
#define BYTE_STATIC_H

#include "common.h"

typedef struct {
    int     index;      ///< Index in static objects rendered in scene
    vec2i   position;   ///< Static object position on map


    struct {
        str128  mesh;           ///< Mesh filename
        int     mesh_id;        ///< Mesh resource ID
        vec2    mesh_size;      ///< Mesh size

        str128  texture[2];     ///< Texture filenames. 0:albedo. 1:heightmap
        int     texture_id[2];  ///< Texture resource IDs
        int     tex_n;          ///< Number of textures
    } assets;
} StaticObject;

#endif /* BYTE_STATIC */
