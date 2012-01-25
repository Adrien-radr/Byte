#ifndef BYTE_TEXT_HPP
#define BYTE_TEXT_HPP

#include "common.h"
#include "color.h"
#include "world.h"


// forward declarations (check C src file for defs)
typedef struct s_Font Font;
typedef struct s_Text Text;

Font *Font_new();

void Font_destroy( Font *pFont );

bool Font_createAtlas( Font *pFont, const char *pFile, u32 pSize );


/// TEXT
Text *Text_new();

void Text_destroy( Text *pText );

void Text_render( Text *pText );

void Text_setFont( Text *t, World *world, const char *pFontName, u32 pSize );

void Text_setColor( Text *t, const Color *pColor );

void Text_setText( Text *t, const char *pStr );

void Text_setShader( Text *t, u32 pShader );



#endif // BYTE_TEXT
