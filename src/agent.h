#ifndef BYTE_AGENT_H
#define BYTE_AGENT_H

#include "render/sprite.h"

/// An agent is a AI representing a game living entity
typedef struct {
    str64   firstname,  ///< First name   (essential)
            lastname,   ///< Last name    (optional)
            surname;    ///< Alias/pseudo (optional)

    vec2i   location;   ///< Location on map (tile)



    Sprite  sprite;     ///< Sprite representation
} Agent;

/// Loads an agent from a JSON file
bool Agent_load( Agent *a, const char *file );

/// Change the agent's location and its sprite position
void Agent_setPosition( Agent *a, const vec2i *pos );

/// Sets the currently played animation
void Agent_playAnimation( Agent *a, Animation anim );

#endif /* BYTE_AGENT */
