#include "world.h"
#include "renderer.h"
#include "context.h"
#include "resource.h"
#include "game.h"

#include "GL/glew.h"


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
        FT_Library *ft = Game_getFreetype(); 

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

        GLint curr_alignment;
        glGetIntegerv( GL_UNPACK_ALIGNMENT, &curr_alignment );
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

        glPixelStorei( GL_UNPACK_ALIGNMENT, curr_alignment );

        return true;
    }

error:
    return false;
}


Font *Font_get( const char *pName, u32 pSize ) {
    char *font = NULL;
    if( pName ) {
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
        font = byte_alloc( strlen( pName ) + 4 );
        strcpy( font, pName );
        strcat( font, size );

        // get resource and get the font associated to it from the renderer
        int font_resource = ResourceManager_get( font );
        check( font_resource >= 0, "Cant set font \"%s\", it has not been loaded as a resource!\n", pName );


        DEL_PTR( font );
        return Renderer_getFont( font_resource );
    }
error:
    DEL_PTR( font );
    return NULL;
}

void Text_setString( u32 pMeshVbo, const Font *pFont, const char *pStr ) {
    if( !pStr )
        return;

    // 6 vertices of 4 float(pos<x,y>, texCoord<s,t>), for each character
    const size_t str_len = strlen( pStr );
    const size_t c_size = 6 * 4;
    const size_t text_data_size = c_size * str_len;
    f32 data[text_data_size];
    memset( data, 0.f, sizeof(data) );

    int n_pos = 0, n_tex = text_data_size/2;
    int fw = pFont->mTextureSize.x, fh = pFont->mTextureSize.y;

    f32 x_left = 0;
    f32 x = 0, y = 0;


    const Glyph *glyphs = pFont->mGlyphs;

    for( const char *p = pStr; *p; ++p ) {
        int i = (int)*p;

        // if char is a carriage return, advance y to the next line, and x to the
        // line begining. Increase n_pos and n_tex as if a char was rendered.
        if( '\n' == *p ) {
            y += (fh + 4);
            x = x_left;
            n_pos += 12;
            n_tex += 12;
            continue;

        // if char is a space, advance x by a reasonable amount depending on font
        // size and advance n_pos and n_tex like with \n
        } else if ( 32 == *p ) {
            x += (fh/3);
            n_pos += 12;
            n_tex += 12;
            continue;
        }

        f32 x2 = x + glyphs[i].position.x;
        f32 y2 = y + ( fh - glyphs[i].position.y );
        f32 w  = glyphs[i].size.x;
        f32 h  = glyphs[i].size.y;

        if( !w || !h ) continue;

        x += glyphs[i].advance.x;
        y += glyphs[i].advance.y;

        // positions triangle 1
        data[n_pos++] = x2;         data[n_pos++] = y2;
        data[n_pos++] = x2;         data[n_pos++] = y2 + h;
        data[n_pos++] = x2 + w;     data[n_pos++] = y2 + h;

        // positions triangle 2
        data[n_pos++] = x2;         data[n_pos++] = y2;
        data[n_pos++] = x2 + w;     data[n_pos++] = y2 + h;
        data[n_pos++] = x2 + w;     data[n_pos++] = y2;

        // texcoords triangle 1
        data[n_tex++] = glyphs[i].x_offset;                                         data[n_tex++] = 0;
        data[n_tex++] = glyphs[i].x_offset;                                         data[n_tex++] = (f32)glyphs[i].size.y / (f32)fh;
        data[n_tex++] = glyphs[i].x_offset + (f32)glyphs[i].size.x / (f32)fw;       data[n_tex++] = (f32)glyphs[i].size.y / (f32)fh;

        // texcoords triangle 2
        data[n_tex++] = glyphs[i].x_offset;                                         data[n_tex++] = 0;
        data[n_tex++] = glyphs[i].x_offset + (f32)glyphs[i].size.x / (f32)fw;       data[n_tex++] = (f32)glyphs[i].size.y / (f32)fh;
        data[n_tex++] = glyphs[i].x_offset + (f32)glyphs[i].size.x / (f32)fw;       data[n_tex++] = 0;

    }

    // update mesh
    Renderer_setDynamicMeshDataBlock( pMeshVbo, data, sizeof( data ), NULL, 0 );
}



//////////////////////////////////////////////////////////

TextArray *TextArray_init( u32 pSize ) {
    TextArray *arr = byte_alloc( sizeof( TextArray ) );
    check_mem( arr );

    arr->mUsed = HandleManager_init( pSize );
    arr->mFonts = byte_alloc( pSize * sizeof( Font* ) );
    arr->mMeshes = byte_alloc( pSize * sizeof( u32 ) );
    arr->mColors = byte_alloc( pSize * sizeof( Color ) );
    arr->mStrings = byte_alloc( pSize * sizeof( char* ) );
    arr->mPositions = byte_alloc( pSize * sizeof( vec2 ) );
    arr->mDepths = byte_alloc( pSize * sizeof( int ) );

    arr->mSize = pSize;

error :
    return arr;
}

int TextArray_add( TextArray *arr ) {
    int handle = -1;
    if( arr ) {
        handle = HandleManager_addHandle( arr->mUsed, 1 );

        if( handle >= 0 ) {
            // resize our text array if the handle manager had to be resized
            if( arr->mUsed->mSize != arr->mSize ) {
                arr->mSize = arr->mUsed->mSize;
                arr->mFonts = byte_realloc( arr->mFonts, arr->mSize * sizeof( Font* ) );
                arr->mMeshes = byte_realloc( arr->mMeshes, arr->mSize * sizeof( u32 ) );
                arr->mColors = byte_realloc( arr->mColors, arr->mSize * sizeof( Color ) );
                arr->mStrings = byte_realloc( arr->mStrings, arr->mSize * sizeof( char* ) );
                arr->mPositions = byte_realloc( arr->mPositions, arr->mSize * sizeof( vec2 ) );
                arr->mDepths = byte_realloc( arr->mDepths, arr->mSize * sizeof( int ) );
            }

            // create mesh used by text
            check( (arr->mMeshes[handle] = Renderer_createDynamicMesh( GL_TRIANGLES )) >= 0, "Failed to create mesh of Text!\n" );

            ++arr->mMaxIndex;
            ++arr->mCount;

            return handle;
        }
    }
error:
    if( handle >= 0 )
        HandleManager_remove( arr->mUsed, handle );
    return -1;
}

void TextArray_remove( TextArray *arr, u32 pIndex ) {
    if( arr && pIndex < arr->mUsed->mMaxIndex )  {
        HandleManager_remove( arr->mUsed, pIndex );
        DEL_PTR( arr->mStrings[pIndex] );
        --arr->mCount;
    }
}

void TextArray_clear( TextArray *arr ) {
    if( arr ) {
        HandleManager_clear( arr->mUsed );
        for( u32 i = 0; i < arr->mMaxIndex; ++i ) {
            DEL_PTR( arr->mStrings[i] );
        }
        arr->mMaxIndex = 0;
        arr->mCount = 0;
    }
}

void TextArray_destroy( TextArray *arr ) {
    if( arr ) {
        HandleManager_destroy( arr->mUsed );
        DEL_PTR( arr->mFonts );
        DEL_PTR( arr->mMeshes );
        DEL_PTR( arr->mColors );
        DEL_PTR( arr->mPositions );
        DEL_PTR( arr->mDepths );
        for( u32 i = 0; i < arr->mMaxIndex; ++i ) {
            DEL_PTR( arr->mStrings[i] );
        }
        DEL_PTR( arr->mStrings );
        DEL_PTR( arr );
    }
}

