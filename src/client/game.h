#ifndef BYTE_GAME_HPP
#define BYTE_GAME_HPP

#include "common/world.h"
#include "scene.h"

#include "ft2build.h"
#include FT_FREETYPE_H


/// Stores the different parameters of the engine/game
typedef struct {
    vec2i    window_size;
    u32     multisamples;
    bool    fullscreen;
} Config;

/// Filename of the config file
extern const str32 config_file;


/// Game structure
///     It possesses the game current scene where visible entities are drawn
typedef struct {
    Scene   *scene;                 ///< Game current scene
    Config  config;                 ///< Device/Game configuration parameters
    AnimManager anims;          

    bool    (*frame_func)(f32);     ///< Game frame callback

    u32     fps_text;               ///< FPS text drawn in window
    u32     mousepos_text;          ///< Mouse Position displayed
    u32     mousetile_text;         ///< Tile under mouse displayed

    vec2i   mouse_tile;             ///< Tile under the mouse


    FT_Library  freetype_lib;       ///< Instance of the Freetype library.
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

/// Returns a pointer to freetype lib
FT_Library *Game_getFreetype();


/// Sets an actor position, in its abstract representation(actor.h) and
/// in its scene sprite representation (sprite.h), it it has one
void Game_setActorPosition( Actor *actor, const vec2i *pos );

#endif // BYTE_GAME
