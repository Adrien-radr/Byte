#include "text.h"
#include "device.h"
#include "texture.h"
#include "renderer.h"
#include "context.h"

#include "GL/glew.h"

typedef struct {
    vec2    advance,        ///< Advance to this glyph
            size,           ///< Size of this glyph
            position;       ///< Position of this glyph after offset

    f32     x_offset;       ///< X Offset of the glyph in the texture
} Glyph;

typedef struct s_Font {
    Glyph   mGlyphs[128];   ///< 128 ASCII chars used in fonts
    FT_Face mFace;          ///< Freetype font face

    vec2    mTextureSize;   ///< Size of font texture atlas
    u32     mTexture;       ///< Handle to the font texture
} Font;

typedef struct s_Text {
    Font        *mFont;         ///< Used font to draw the text
    u32         mMesh;          ///< Mesh used to render text
    int         mShader;        ///< Shader used to render text

    Color       mColor;         ///< Font color
    str512      mStr;           ///< Message displayed
    u32         mStrLength;     ///< Size of the message
} Text;


Font *Font_new() {
    Font *f = byte_alloc( sizeof( Font ) );
    check_mem( f );

error:
    return f;
}

void Font_destroy( Font *pFont ) {
    if( pFont ) {
        FT_Done_Face( pFont->mFace );
        DEL_PTR( pFont );
    }
}

bool Font_createAtlas( Font *pFont, const char *pFile, u32 pSize ) {
    if( pFont && pFile ) {
        FT_Library *ft = Device_getFreetype(); 

        check( !FT_New_Face( *ft, pFile, 0, &pFont->mFace ), "Could not open font file \"%s\"!\n", pFile );

        FT_Set_Pixel_Sizes( pFont->mFace, 0, pSize );

        FT_GlyphSlot g = pFont->mFace->glyph;

        // get texture size to store entire font
        for( u32 i = 32; i < 128; ++i ) {
            check( !FT_Load_Char( pFont->mFace, i, FT_LOAD_RENDER ), "Could not load char %d : %c\n", i, (char)i );

            pFont->mTextureSize.x += g->bitmap.width;
            pFont->mTextureSize.y = max( pFont->mTextureSize.y, g->bitmap.rows );
        }

        // make an empty texture to hold our font
        Texture *t = NULL;
        pFont->mTexture = Renderer_allocateEmptyTexture( &t );

        glActiveTexture( GL_TEXTURE0 );
        glGenTextures( 1, &t->mID );
        glBindTexture( GL_TEXTURE_2D, t->mID );
        glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

        
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, pFont->mTextureSize.x,
                                                pFont->mTextureSize.y,
                                                0,
                                                GL_RED, 
                                                GL_UNSIGNED_BYTE,
                                                0 );

        // iterate over each char of font and write them in font
        int x = 0;
        for( u32 i = 32; i < 128; ++i ) {
            if( FT_Load_Char( pFont->mFace, i, FT_LOAD_RENDER ) )
                continue;

            glTexSubImage2D( GL_TEXTURE_2D, 0, x, 0, g->bitmap.width,
                                                     g->bitmap.rows,
                                                     GL_RED, 
                                                     GL_UNSIGNED_BYTE,
                                                     g->bitmap.buffer );

            pFont->mGlyphs[i].advance.x = g->advance.x >> 6;
            pFont->mGlyphs[i].advance.y = g->advance.y >> 6;

            pFont->mGlyphs[i].size.x = g->bitmap.width;
            pFont->mGlyphs[i].size.y = g->bitmap.rows;

            pFont->mGlyphs[i].position.x = g->bitmap_left;
            pFont->mGlyphs[i].position.y = g->bitmap_top;

            pFont->mGlyphs[i].x_offset = (f32)x / (f32)pFont->mTextureSize.x;

            x += g->bitmap.width;
        }

        // size of space is size of 'm' letter
        pFont->mGlyphs[32].advance.x = pFont->mGlyphs[(int)'m'].advance.x;

        glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );

        return true;
    }

error:
    return false;
}



Text *Text_new() {
    Text *t = byte_alloc( sizeof( Text ) );
    check_mem( t );

    // generate font vbo
    glGenBuffers( 1, &t->mMesh );

    t->mStr[0] = 0;

    // black color
    t->mColor.a = 1.f;

    // no shader at first
    t->mShader = -1;

error:
    return t;
}

void Text_destroy( Text *t ) {
    if( t ) {
        glDeleteBuffers( 1, &t->mMesh );
        DEL_PTR( t );
    }
}

void Text_render( Text *t ) {
    Renderer_useShader( t->mShader );
    Shader_sendColor( "Color", &t->mColor );
    Renderer_useTexture( t->mFont->mTexture, 0 );

    glBindBuffer( GL_ARRAY_BUFFER, t->mMesh );
    glEnableVertexAttribArray( 0 );
    glEnableVertexAttribArray( 1 );
    glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, 0 );
    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(f32)*t->mStrLength*6*2) );
    glDrawArrays( GL_TRIANGLES, 0, t->mStrLength * 6 );
}

void Text_setFont( Text *t, World *world, const char *pFontName, u32 pSize ) {
    char *font = NULL;
    if( t && pFontName ) {
        // string representation of the size
        u32 s = pSize;
        char size[4];
        if( 12 == s )
            strcpy( size, "/12\0" );
        else if( 32 == s )
            strcpy( size, "/32\0" );
        else
            strcpy( size, "/20\0" );

        // string containing font name and size : "fontName.ttf/fontSize"
        font = byte_alloc( strlen( pFontName ) + 4 );
        strcpy( font, pFontName );
        strcat( font, size );

        // get resource and get the font associated to it from the renderer
        int font_resource = World_getResource( world, font );
        check( font_resource >= 0, "Cant set font \"%s\", it has not been loaded as a resource!\n", pFontName );
        t->mFont = Renderer_getFont( font_resource );
            
    }
error:
    DEL_PTR( font );
}

void Text_setColor( Text *t, const Color *pColor ) {
    if( t ) 
        memcpy( &t->mColor.r, &pColor->r, 4 * sizeof( f32 ) );
}

void Text_setText( Text *t, const char *pStr ) {
    //change only if two strings are not equal
    if( t && pStr && strcmp( pStr, t->mStr ) ) {
        const size_t newstr_len = strlen( pStr );
        if( 512 <= newstr_len )
            log_err( "String : \n\"%s\"\n\nis too long to be put as a Text string (max:512 chars)!\n", pStr );

        strncpy( t->mStr, pStr, 512 );

        t->mStrLength = newstr_len;


        // create VBO from string

        // 6 vertices of 4 float(pos<x,y>, texCoord<s,t>), for each character
        const size_t c_size = 6 * 4;
        const size_t text_data_size = c_size * newstr_len;
        f32 data[text_data_size];

        glBindBuffer( GL_ARRAY_BUFFER, t->mMesh );

        int n_pos = 0, n_tex = text_data_size/2;
        int fw = t->mFont->mTextureSize.x, fh = t->mFont->mTextureSize.y;

        vec2 ws = Context_getSize();
        f32 sx = 2.f / ws.x, sy = 2.f / ws.y;

        f32 x_left = -1;
        f32 x = -1, y = (ws.y/2.f - fh)*sy;


        const Glyph *glyphs = t->mFont->mGlyphs;

        for( const char *p = t->mStr; *p; ++p ) {
            int i = (int)*p;
            if( '\n' == *p ) {
                y -= (fh + 4) * sx;
                x = x_left;
                continue;
            } else if ( 32 == *p ) {
                x += (fh/3) * sx;
                continue;
            }

            f32 x2 = x  + glyphs[i].position.x * sx;
            f32 y2 = -y - glyphs[i].position.y * sy;
            f32 w  = glyphs[i].size.x * sx;
            f32 h  = glyphs[i].size.y * sy;

            if( !w || !h ) continue;

            x += glyphs[i].advance.x * sx;
            y += glyphs[i].advance.y * sy;

            // positions triangle 1
            data[n_pos++] = x2;         data[n_pos++] = -y2;
            data[n_pos++] = x2;         data[n_pos++] = -y2 - h;
            data[n_pos++] = x2 + w;     data[n_pos++] = -y2 - h;

            // positions triangle 2
            data[n_pos++] = x2;         data[n_pos++] = -y2;
            data[n_pos++] = x2 + w;     data[n_pos++] = -y2 - h;
            data[n_pos++] = x2 + w;     data[n_pos++] = -y2;

            // texcoords triangle 1
            data[n_tex++] = glyphs[i].x_offset;                                         data[n_tex++] = 0;
            data[n_tex++] = glyphs[i].x_offset;                                         data[n_tex++] = (f32)glyphs[i].size.y / (f32)fh;
            data[n_tex++] = glyphs[i].x_offset + (f32)glyphs[i].size.x / (f32)fw;       data[n_tex++] = (f32)glyphs[i].size.y / (f32)fh;

            // texcoords triangle 2
            data[n_tex++] = glyphs[i].x_offset;                                         data[n_tex++] = 0;
            data[n_tex++] = glyphs[i].x_offset + (f32)glyphs[i].size.x / (f32)fw;       data[n_tex++] = (f32)glyphs[i].size.y / (f32)fh;
            data[n_tex++] = glyphs[i].x_offset + (f32)glyphs[i].size.x / (f32)fw;       data[n_tex++] = 0;
                
        }

        glBufferData( GL_ARRAY_BUFFER, sizeof( data ), data, GL_DYNAMIC_DRAW );
    }
}

void Text_setShader( Text *t, u32 pShader ) {
    if( t )
        t->mShader = pShader;
}
