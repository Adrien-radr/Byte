#ifndef BYTE_GAME_HPP
#define BYTE_GAME_HPP

#include "common/world.h"
#include "device.h"
#include "scene.h"

/// Game structure
///     It possesses the game current scene where visible entities are drawn
typedef struct {
    Scene   *mScene;                ///< Game current scene

    bool    (*frame_func)(f32);     ///< Game frame callback

    u32     fps_text;               ///< FPS text drawn in window
} Game;

/// Game Instance
extern Game *game;

/// Initialize the Game and all its needed components
bool Game_init( void (*init_func)(), bool (*frame_func)(f32) );

/// Take care of destroying and releasing all game components/resources
void Game_destroy();

/// Load all game actor assets (mesh and textures)
bool Game_loadActorAssets( Actor *actor );

/// Run a game frame (called from client)
/// @return : false if game must be stopped
bool Game_update( f32 frame_time );

#endif // BYTE_GAME
