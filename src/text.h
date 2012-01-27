#ifndef BYTE_TEXT_HPP
#define BYTE_TEXT_HPP

#include "common.h"
#include "color.h"
#include "vector.h"
#include "world.h"


// forward declarations (check C src file for defs)
typedef struct s_Font Font;
typedef struct s_Text Text;

/// Creates and return a newly allocated font
Font *Font_new();

/// Destroy and free a given font
void Font_destroy( Font *pFont );

/// Create a font atlas from a font size, with a given size for the font height
/// @return : True if everything went well. False otherwise
bool Font_createAtlas( Font *pFont, const char *pFile, u32 pSize );


/// Creates and returns a newly allocated text
Text *Text_new();

/// Destroy and free a given text
void Text_destroy( Text *pText );

/// Renders a given text
void Text_render( Text *pText );

/// Sets the font of the given text
/// @param world : Pointer to the game world (it possesses the ResourceManager)
/// @param pFontName : File of the font, must be present in data/fonts/)
/// @param pSize : Size of the font, must be 12, 20 or 32 (for now )
void Text_setFont( Text *t, World *world, const char *pFontName, u32 pSize );

/// Sets the color used as the foreground color of the font
void Text_setColor( Text *t, const Color *pColor );

/// Sets what text string is displayed (creates the VBO here)
void Text_setText( Text *t, const char *pStr );

/// Sets the shader used to display the font
void Text_setShader( Text *t, u32 pShader );

/// Sets the posiiton in screen space of the text
void Text_setPosition( Text *t, const vec2 *pPosition );

/// Asks the given text to remake its vbo from scratch from the same text
/// Useful if window is resized for example
void Text_updateText( Text *t );



#endif // BYTE_TEXT
