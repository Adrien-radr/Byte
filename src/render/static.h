#ifndef BYTE_STATIC_H
#define BYTE_STATIC_H

#include "common/handlemanager.h"

typedef struct {
    int     scene_id;       ///< Index in static objects rendered in scene
    vec2    position;       ///< Global position in px
    int     depth;          ///< On-screen depth


    vec2    mesh_size;      ///< Size in global px
    vec2    texcoords_size; ///< Size of obj on texture atlas [0..1]^2
    vec2    texcoords_pos;  ///< Position of obj on texture atlas [0..1]^2

    str128  texture[2];     ///< Texture filenames. 0:albedo. 1:heightmap
    int     tex_id[2];      ///< Texture resource IDs
    int     tex_n;          ///< Number of textures
} StaticObject;

/// Array of StaticObjects, named StaticsArray
SimpleArrayDecl( StaticObject, Statics )

/// Copy a StaticObj in another (only textures/sizes references)
void StaticObject_cpy( StaticObject *dst, const StaticObject *src );

/// Loads an StaticObject from a JSON file and create its assets
bool StaticObject_load( StaticObject *s, const char *file );


/// Array containing all static objects in a world tile
typedef struct {
    HandleManager   *used;   ///< HM used as an indexing array
    StaticObject    **objs;  ///< Ptr on StaticObjects indexed by used

    u32             count,
                    size,
                    max_index;
} StaticObjectArray;

/// Initialize and allocate new SOArray
StaticObjectArray *StaticObjectArray_init( u32 size );

/// Add a new SO to the SOArray and returns its handle (or -1 if any error)
int StaticObjectArray_add( StaticObjectArray *s );

/// Remove a SO from the given SO array, by its index
void StaticObjectArray_remove( StaticObjectArray *s, u32 index );

/// Clears the whole given SO array
void StaticObjectArray_clear( StaticObjectArray *s );

/// Destroy and free the given SO array
void StaticObjectArray_destroy( StaticObjectArray *s );

#endif /* BYTE_STATIC */
