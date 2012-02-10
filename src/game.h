#ifndef BYTE_GAME_HPP
#define BYTE_GAME_HPP

#include "device.h"
#include "world.h"
#include "scene.h"

/// Game structure
///     It possesses the game current scene where visible entities are drawn
typedef struct {
    Scene   *mScene;        ///< Game current scene
} Game;

/// Game Instance
extern Game *game;

/// Initialize the Game and all its needed components
bool Game_init();

/// Take care of destroying and releasing all game components/resources
void Game_destroy();

#endif // BYTE_GAME
