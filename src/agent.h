#ifndef BYTE_AGENT_H
#define BYTE_AGENT_H

#include "map.h"
#include "render/sprite.h"

/// Global agent shared variables
extern const int human_walkspeed;
extern const int human_runspeed;

/// An agent is a AI representing a game living entity
typedef struct {
    str64   firstname,  ///< First name   (essential)
            lastname,   ///< Last name    (optional)
            surname;    ///< Alias/pseudo (optional)

    vec2i   location;   ///< Location as absolute map tile
    vec2i   world_tile; ///< Location as world tile

    u32     world_id;   ///< ID in world storage

    Path    *path;      ///< Current path (NULL if none)
    u32     path_index; ///< Current node in path
    f32     path_ctr;   ///< Time counter for path update


    Sprite  sprite;     ///< Sprite representation
} Agent;

/// Loads an agent from a JSON file
bool Agent_load( Agent *a, const char *file );

void Agent_destroy( Agent *a );

/// Change the agent's location and its sprite position
void Agent_setLocation( Agent *a, const vec2i *loc );

/// Sets the currently played animation
void Agent_playAnimation( Agent *a, Animation anim );

/// Ask to an agent to begin to move to a certain tile
void Agent_moveTo( Agent *a, const vec2i *pos );

/// Update fonction (call each frame) for an agent
/// Will update its gameplay and ai state
void Agent_update( Agent *a, f32 frame_time );

#endif /* BYTE_AGENT */
