#include "color.h"
#include "shader.h"
#include "device.h"
#include "camera.h"
#include "GL/glew.h"
#include "scene.h"
#include "world.h"
#include "renderer.h"
#include "context.h"

#include "ft2build.h"
#include FT_FREETYPE_H

FT_Face face;

void DrawText( u32 vbo, u32 shader, u32 tex, const char *text, f32 posX, f32 posY ) {
    Color c = { .r = 1, .g = 0, .b = 0, .a = 1 };
    Renderer_useShader( shader );
    Shader_sendColor( "Color", &c );

    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glEnableVertexAttribArray( 0 );
    glEnableVertexAttribArray( 1 );
    glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, 0 );
    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(4 * 2 * sizeof( f32 )) );

    Renderer_useTexture( tex, 0 );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

    const char *p;
    vec2 ws = Context_getSize();

    f32 sx = 2.f / ws.x;
    f32 sy = 2.f / ws.y;

    f32 x = posX, y = posY;

    for( p = text; *p; ++p ) {
        if( FT_Load_Char( face, *p, FT_LOAD_RENDER ) ) 
            continue;    

        FT_GlyphSlot g = face->glyph;


        glTexImage2D(   GL_TEXTURE_2D, 
                        0, 
                        GL_RGB, 
                        g->bitmap.width,
                        g->bitmap.rows,
                        0,
                        GL_RED,
                        GL_UNSIGNED_BYTE,
                        g->bitmap.buffer );

    CheckGLError();
        f32 x2 = x + g->bitmap_left * sx;
        f32 y2 = -y - g->bitmap_top * sy;
        f32 w = g->bitmap.width * sx;
        f32 h = g->bitmap.rows * sy;

        float data[] = {
            x2,     -y2 - h,
            x2 + w, -y2 - h,
            x2,     -y2,
            x2 + w, -y2,
            0, 1,
            1, 1,
            0, 0,
            1, 0,
        };

        glBufferData( GL_ARRAY_BUFFER, sizeof( data ), data, GL_DYNAMIC_DRAW );
        glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    //CheckGLError();
        x += (g->advance.x >> 6) * sx;
        y += (g->advance.y >> 6) * sy;
    }
}

int main() {
    check( Device_init(), "Error while creating Device, exiting program.\n" );


    printf( "Hello, Byte World!!\n" );

    str64 date;
    str16 time;
    GetTime( date, 64, DateFmt );
    GetTime( time, 16, TimeFmt );

    strncat( date, " - ", 3 );
    strncat( date, time, 16 );
    printf( "%s\n\n", date );




    World *world = World_new();
    check( world, "Error in world creation!\n" );

    /*
    World_loadAllResources( world );

    // ###############################3
    //      TEXTURE
    int t1 = World_getResource( world, "crate.jpg" );
    int texture = World_getResource( world, "img_test.png" );
    check( texture >= 0 && t1 >= 0, "Error in texture creation. Exiting program!\n" );
    Renderer_useTexture( t1, 0 );


    // ###############################3
    //      SHADER
    int shader = World_getResource( world, "defaultShader.json" ); 
    check( shader >= 0, "Error in shader creation. Exiting program!\n" );



    // ###############################3
    //      MESH
    vec2 data[] = {
        { .x = -5.f, .y = -5.f },
        { .x = -5.f, .y = 5.f },
        { .x = 5.f, .y = 5.f },
        { .x = 5.f, .y = -5.f }
    };

    vec2 texcoords[] = {
        { .x = 0.f, .y = 0.f },
        { .x = 0.f, .y = 1.f },
        { .x = 1.f, .y = 1.f },
        { .x = 1.f, .y = 0.f }
    };
    
    u32 indices[] = {
        0, 1, 2, 0, 2, 3
    };

    int vao = Renderer_beginVao();
    check( vao >= 0, "Could not create vao!\n" );
    glEnableVertexAttribArray( 0 );
    glEnableVertexAttribArray( 1 );


    u32 mesh = Renderer_createMesh( indices, sizeof( indices ), data, sizeof( data ), texcoords, sizeof( texcoords ) );
    check( mesh >= 0, "Error while creating mesh!\n" );

    
    // ###############################3
    //      MATRICES
    mat3 ModelMatrix, MM;
    mat3_identity( &ModelMatrix );
    mat3_rotatef( &ModelMatrix, 45.f );
    mat3_scalef( &ModelMatrix, 2.f, 1.f );

    mat3_translatef( &ModelMatrix, 400.f, 300.f );

    mat3_identity( &MM );
    mat3_scalef( &MM, 3.f, 3.f );
    mat3_translatef( &MM, 500.f, 400.f );



    Scene *scene = Scene_new();

    Entity e = { .mMesh = mesh, .mShader = shader, .mModelMatrix = &ModelMatrix, .mTexture = t1 };

    int entity = Scene_addEntity( scene, &e );
    check( entity >= 0, "Failed to create entity!\n" );
*/
    ////////////////////////////////////
    // FREETYPE TEST

    int textShader = World_loadResource( world, RT_Shader, "textShader.json" );
    check( textShader >= 0, "Error in text shader creation. Exiting!\n" );

    FT_Library *ft = Device_getFreetype();

    check( !FT_New_Face( *ft, "/usr/share/fonts/TTF/LiberationMono-Regular.ttf", 0, &face ), "Could not open font visitor!\n" );


    FT_Set_Pixel_Sizes( face, 0, 16 );
/*
    FT_GlyphSlot g = face->glyph;

    int w = 0, h = 0;

    for( u32 i = 32; i < 128; ++i ) {
        check( !FT_Load_Char( face, i, FT_LOAD_RENDER ), "Could not load char %d : %c\n", i, (char)i );

        w += g->bitmap.width;
        h = max( h, g->bitmap.rows );
    }

    log_info( "w = %d, h = %d\n", w, h );


    
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, 0 );
CheckGLError();
    int x = 0;

    for( u32 i = 32; i < 128; ++i ) {
        if( FT_Load_Char( face, i, FT_LOAD_RENDER ) )
            continue;

        printf( "%d\n", g->bitmap.rows );
        glTexSubImage2D( GL_TEXTURE_2D, 0, x, 0, g->bitmap.width, 
                                                 g->bitmap.rows,
                                                 GL_RED, 
                                                 GL_UNSIGNED_BYTE,
                                                 g->bitmap.buffer );

CheckGLError();
        x += g->bitmap.width;
    }

    glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );


*/


    u32 tex;
    glActiveTexture(GL_TEXTURE0);
    glGenTextures( 1, &tex );
    glBindTexture( GL_TEXTURE_2D, tex );
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );



    u32 textVbo;
    glGenBuffers( 1, &textVbo );
    glBindBuffer( GL_ARRAY_BUFFER, textVbo );

    /*
    Color c = { .r = 1, .g = 0, .b = 0, .a = 1 };
    Renderer_useShader( textShader );
    Shader_sendColor( "Color", &c );

    glEnableVertexAttribArray( 0 );
    glEnableVertexAttribArray( 1 );
    glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, 0 );
    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(4 * 2 * sizeof( f32 )) );

    Renderer_useTexture( tex, 0 );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

    const char text[] = "H";
    const char *p = text;
    vec2 ws = Context_getSize();

    f32 sx = 2.f / ws.x;
    f32 sy = 2.f / ws.y;

    f32 x = 0, y = 0;

        if( FT_Load_Char( face, *p, FT_LOAD_RENDER ) ) 
            return -1;

        FT_GlyphSlot g = face->glyph;


        glTexImage2D(   GL_TEXTURE_2D, 
                        0, 
                        GL_RGBA, 
                        g->bitmap.width,
                        g->bitmap.rows,
                        0,
                        GL_RGBA,
                        GL_UNSIGNED_BYTE,
                        g->bitmap.buffer );

    CheckGLError();
        f32 x2 = x + g->bitmap_left * sx;
        f32 y2 = -y - g->bitmap_top * sy;
        f32 w = g->bitmap.width * sx;
        f32 h = g->bitmap.rows * sy;

        float har[] = {
            x2,     -y2 - h,
            x2 + w, -y2 - h,
            x2,     -y2,
            x2 + w, -y2,

            0, 1,
            1, 1,
            0, 0,
            1, 0,
        };

        glBufferData( GL_ARRAY_BUFFER, sizeof( har ), har, GL_DYNAMIC_DRAW );

*/


    ////////////////////////////////////

    int cpt = 0;
    f32 accum = 0;

    while( !IsKeyUp( K_Escape ) && Context_isWindowOpen() ) {
        Device_beginFrame();
            //Scene_update( scene );

            // stuff
            accum += Device_getFrameTime();
            if( accum > 1.f ) {
                ++cpt;
               /* accum = 0.f;

                if( cpt == 2 ) {
                    Scene_removeEntity( scene, shader, entity );
                }
                if( cpt == 3 ) {
                    e.mTexture = texture;
                    Scene_addEntity( scene, &e );
                }
                */
                //log_info( "Camera Position : <%f, %f>\n", cam->mPosition.x, cam->mPosition.y );
                //log_info( "Cam zoom = %f\n", cam->mZoom );
            }


            //Scene_render( scene );

            vec2 ws = Context_getSize();
            f32 sx = 2.f / ws.x;
            f32 sy = 2.f / ws.y;
//        glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
            DrawText( textVbo, textShader, tex, "1234567890-=_+\\|}][{'\";:/?.>,<mM!@#$%^&**()HThe quick brown fox jumps over the lazy dog", -1*400*sx, -1*-288*sy );
        Device_endFrame();
    }


    //Scene_destroy( scene );
    World_destroy( world );
    Device_destroy();

    return 0;

error :

    //Scene_destroy( scene );
    World_destroy( world );
    Device_destroy();
    return -1;
}

