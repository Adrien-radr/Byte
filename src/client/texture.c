#include "texture.h"
#include "renderer.h"

#ifdef USE_GLDL
#include "GL/gldl.h"
#else
#include "GL/glew.h"
#endif

#include <png.h>

typedef struct {
    u32         width, 
                height;

    GLenum      fmt;
    GLint       int_fmt;
    GLuint      id;

    GLubyte    *texels;
} texture_t;

static void Byte_GetPNGInfo( int color_type, texture_t *ti ) {
    switch( color_type ) {
    case PNG_COLOR_TYPE_GRAY :
        ti->fmt = GL_RED;
        ti->int_fmt = GL_RED;
        break;
    case PNG_COLOR_TYPE_GRAY_ALPHA :
        ti->fmt = GL_RG;
        ti->int_fmt = GL_RG;
        break;
    case PNG_COLOR_TYPE_RGB :
        ti->fmt = GL_RGB;
        ti->int_fmt = GL_RGB;
        break;
    case PNG_COLOR_TYPE_RGB_ALPHA :
        ti->fmt = GL_RGBA;
        ti->int_fmt = GL_RGBA;
        break;
    default :
        log_err( "In Byte_GetPNGInfo : invalid color type!\n" );
        break;
    }
}

static texture_t *Byte_LoadPNG( const char *filename ) {
    texture_t *t = NULL;
    FILE *f = NULL;

    png_byte    sig[8];
    png_structp img;
    png_infop   img_info;

    int bpp, color_type;
    png_bytep *row_pointers = NULL;
    png_uint_32 w, h;


    f = fopen( filename, "rb" );
    check( f, "Could not open \"%s\"!\n", filename );

    // get png file signature
    fread( sig, 1, 8, f );
    check( png_check_sig( sig, 8 ), "\"%s\" is not a valid PNG image!\n", filename );

    
    img = png_create_read_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
    check( img, "Error while creating PNG read struct.\n" );

    img_info = png_create_info_struct( img );
    check( img, "Error while creating PNG info struct.\n" );

    // our texture handle
    t = byte_alloc( sizeof(texture_t) );
    
    // setjmp for any png loading error
    if( setjmp( png_jmpbuf( img ) ) ) 
        goto error;
    
    // Load png image
    png_init_io( img, f );
    png_set_sig_bytes( img, 8 );
    png_read_info( img, img_info );

        //get info
        //bpp = png_get_bit_depth( img, img_info );
        color_type = png_get_color_type( img, img_info );

        // if indexed, make it RGB
        if( PNG_COLOR_TYPE_PALETTE == color_type )
            png_set_palette_to_rgb( img );

        // if 1/2/4 bits gray, make it 8-bits gray
        if( PNG_COLOR_TYPE_GRAY && 8 > bpp )
            png_set_expand_gray_1_2_4_to_8( img );

        if( png_get_valid( img, img_info, PNG_INFO_tRNS ) )
            png_set_tRNS_to_alpha( img );

        if( 16 == bpp ) 
            png_set_strip_16( img );
        else if( 8 > bpp )
            png_set_packing( img );

    // get img info
    png_read_update_info( img, img_info );
    png_get_IHDR( img, img_info, &w, &h, &bpp, &color_type, NULL, NULL, NULL );
    t->width = w;
    t->height = h;


    Byte_GetPNGInfo( color_type, t );


    // create actual texel array
    int rowbytes = png_get_rowbytes( img, img_info );
    t->texels = byte_alloc( rowbytes * h );
    row_pointers = byte_alloc( sizeof(png_bytep) * h );

    for( u32 i = 0; i < t->height; ++i )
        row_pointers[i] = t->texels + i * rowbytes;

    png_read_image( img, row_pointers );
    //png_read_end( img, NULL );
    png_destroy_read_struct( &img, &img_info, NULL );
    DEL_PTR( row_pointers );
    fclose( f );

    return t;

error:
    fclose( f );
    png_destroy_read_struct( &img, &img_info, NULL );
    DEL_PTR( row_pointers );
    if( t ) {
        DEL_PTR( t->texels );
        DEL_PTR( t );
    }
    return NULL;
}

Texture *Texture_new() {
    Texture *t = byte_alloc( sizeof( Texture ) );
    return t;
}

void Texture_destroy( Texture *pTexture ) {
    if( pTexture ) {
        glDeleteTextures( 1, &pTexture->mID );
        DEL_PTR( pTexture );
    }
}

bool Texture_loadFromFile( Texture *pTexture, const char *pFile, bool pMipmaps ) { 
    texture_t *t = NULL;

    if( pTexture && pFile ) {
        t = Byte_LoadPNG( pFile );
        check( t && t->texels, "Error while loading \"%s\" image.\n", pFile );

        glGenTextures( 1, &t->id );
        glBindTexture( GL_TEXTURE_2D, t->id );

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

        GLint curr_alignment;
        glGetIntegerv( GL_UNPACK_ALIGNMENT, &curr_alignment );
        glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

        glTexImage2D(   GL_TEXTURE_2D, 0,
                        t->int_fmt, 
                        t->width, t->height, 0,
                        t->fmt, GL_UNSIGNED_BYTE,
                        t->texels );
        CheckGLError();

        glPixelStorei( GL_UNPACK_ALIGNMENT, curr_alignment );

        pTexture->mID = t->id;

        /*
        u32 gl_id = SOIL_load_OGL_texture(  pFile, 
                                            SOIL_LOAD_AUTO,
                                            SOIL_CREATE_NEW_ID,
                                            (pMipmaps ? SOIL_FLAG_MIPMAPS : 0) );

        check( gl_id, "Error while loading texture \"%s\" : %s\n", pFile, SOIL_last_result() );

        //remove error due to OpenGL3 Core profile not used in SOIL provoking Error
        glGetError();


        pTexture->mID = gl_id;
       */

        DEL_PTR( t->texels );
        DEL_PTR( t );

        return true;
    }
error:
    if( t ) {
        DEL_PTR( t->texels );
        DEL_PTR( t );
    }
    return false;
}


void Texture_bind( Texture *pTexture, int pTarget ) {
    u32 tex = pTexture ? pTexture->mID : 0;

    // if pTarget < 0, no target change, else, switch the target :
    if( pTarget >=0 ) 
        glActiveTexture( GL_TEXTURE0 + pTarget );


    glBindTexture( GL_TEXTURE_2D, tex );
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
}
