#ifndef BYTE_TEXT_HPP
#define BYTE_TEXT_HPP

#include "color.h"
#include "vector.h"
#include "handlemanager.h"
#include "device.h"


/// A Glyph is a single character in a font
typedef struct {
    vec2    advance,        ///< Advance to this glyph
            size,           ///< Size of this glyph
            position;       ///< Position of this glyph after offset

    f32     x_offset;       ///< X Offset of the glyph in the texture
} Glyph;

/// This is a font loaded with freetype 
typedef struct {
    Glyph   mGlyphs[128];   ///< 128 ASCII chars used in fonts
    FT_Face mFace;          ///< Freetype font face

    vec2    mTextureSize;   ///< Size of font texture atlas
    u32     mTexture;       ///< Handle to the font texture
} Font;

/// Creates and return a newly allocated font
Font *Font_new();

/// Destroy and free a given font
void Font_destroy( Font *pFont );

/// Create a font atlas from a font size, with a given size for the font height
/// @return : True if everything went well. False otherwise
bool Font_createAtlas( Font *pFont, const char *pFile, u32 pSize );

/// Returns a Font* corresponding to the given font name and size, or NULL if not loaded
Font *Font_get( const char *pName, u32 pSize );



// ##########################################################################3
//      Text Array 
// ##########################################################################3
    /// Data-oriented array storing all text existing in the scene
    typedef struct {
        HandleManager   *mUsed;     ///< Handlemanager saying if an index is used

        const Font      **mFonts;
        u32             *mMeshes;
        Color           *mColors;
        vec2            *mPositions;

        char            **mStrings;

        u32             mMaxIndex,
                        mCount,
                        mSize;
    } TextArray;

    /// Differents attributes of text that can be modified
    typedef enum {
        TA_Font,
        TA_Color,
        TA_String,
        TA_Position
    } TextAttrib;

    /// Initialize and allocate a new TextArray
    TextArray *TextArray_init( u32 pSize );

    /// Add a new text to the text array and returns its handle (or -1 if any error)
    int TextArray_add( TextArray *arr );

    /// Remove a text from the given text array, by its index
    void TextArray_remove( TextArray *arr, u32 pIndex );

    /// Clears the whole given text array
    void TextArray_clear( TextArray *arr );

    /// Destroy and free the given text array
    void TextArray_destroy( TextArray *arr );


    /// Sets what text string is displayed ( modify the given vbo )
    void Text_setString( u32 pMeshVbo, const Font *pFont, const char *pStr );

#endif // BYTE_TEXT
