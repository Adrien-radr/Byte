#include "color.h"
#include "shader.h"
#include "device.h"
#include "camera.h"
#include "GL/glew.h"
#include "scene.h"
#include "resource.h"

/*
            TODO



*/


void cameraMouseListener( const Event *pEvent, void *pCamera ) {
    Camera *cam = (Camera*)pCamera;
    if( pEvent->Type == E_MouseWheelMoved ) 
        Camera_zoom( cam, pEvent->Wheel );
}

void cameraUpdate( Camera *pCamera ) {
    vec2 move = { .x = 0.f, .y = 0.f };
    if( IsKeyDown( K_W ) )
        move.y -= 1.f;
    if( IsKeyDown( K_A ) )
        move.x -= 1.f;
    if( IsKeyDown( K_S ) )
        move.y += 1.f;
    if( IsKeyDown( K_D ) )
        move.x += 1.f;

    Camera_move( pCamera, &move );
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

    // resourcemanager test

    ResourceManager *resource_manager = ResourceManager_new();
    check( resource_manager, "Error in initialization or resource manager. Exiting program!\n" );

    /*/ json test
    char *json_file;
    ReadFile( &json_file, "test.json" );
    check( json_file, "Cant open test.json\n");

    cJSON *root = cJSON_Parse( json_file );
    check( root, "JSON error before : [%s]\n.", cJSON_GetErrorPtr() );

    cJSON *shaders = cJSON_GetObjectItem( root, "shaders" );
    u32 shadercount = cJSON_GetArraySize( shaders );
    cJSON *entities = cJSON_GetObjectItem( root, "entities" );
    u32 entitycount = cJSON_GetArraySize( entities );

    for( u32 i = 0; i < entitycount; ++i ) {
        cJSON *item = cJSON_GetArrayItem( entities, i );
        cJSON *render = cJSON_GetObjectItem( item, "render" );
        log_info( "name : %s, render : (w:%d, h:%d,tex:%s,shader:%s)\n", 
                cJSON_GetObjectItem( item, "name" )->valuestring,
                cJSON_GetObjectItem( render, "width" )->valueint,
                cJSON_GetObjectItem( render, "height" )->valueint,
                cJSON_GetObjectItem( render, "texture" )->valuestring,
                cJSON_GetObjectItem( render, "shader" )->valuestring );
    }

    log_info( "shaders : %d, entities : %d\n", shadercount, entitycount );
    cJSON_Delete( root );
    DEL_PTR( json_file );
    */

    // ###############################3
    //      TEXTURE
    //int texture = Renderer_createTexture( "img_test.png" );
    int t1 = ResourceManager_load( resource_manager, RT_Texture, "crate.jpg" );
    int texture = ResourceManager_load( resource_manager, RT_Texture, "img_test.png" );
    check( texture >= 0 && t1 >= 0, "Error in texture creation. Exiting program!\n" );
    Renderer_useTexture( t1, 0 );


    // ###############################3
    //      SHADER
    int shader = ResourceManager_load( resource_manager, RT_Shader, "defaultShader.json" );
    check( shader >= 0, "Error in shader creation. Exiting program!\n" );



    // ###############################3
    //      CAMERA
    Camera *cam = Camera_new();
    Camera_registerListener( cam, cameraMouseListener, LT_MouseListener );
    Camera_registerUpdateFunction( cam, cameraUpdate );

    Device_setCamera( cam );


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

    CheckGLError();

    f32 accum = 0;

    while( !IsKeyUp( K_Escape ) && Context_isWindowOpen() ) {
        Device_beginFrame();
        Camera_update( cam );

            // stuff
            accum += Device_getFrameTime();
            if( accum > 1.f ) {
                accum = 0.f;
                //log_info( "Camera Position : <%f, %f>\n", cam->mPosition.x, cam->mPosition.y );
                //log_info( "Cam zoom = %f\n", cam->mZoom );
            }



            // rendering
            Renderer_useShader( shader );
                //Shader_sendMat3( "ModelMatrix", &MM );
                //Renderer_renderMesh( mesh );

                Shader_sendMat3( "ModelMatrix", &ModelMatrix );
                Renderer_renderMesh( mesh );
            Renderer_useShader( -1 );
        Device_endFrame();
    }

    Camera_destroy( cam );

    ResourceManager_destroy( resource_manager );
    Device_destroy();

    return 0;

error :

    ResourceManager_destroy( resource_manager );
    Device_destroy();
    return -1;
}

