#include "sprite.h"
#include "resource.h"
#include "renderer.h"
#include "json/cJSON.h"

void Sprite_cpy( Sprite *dst, const Sprite *src ) {
    if( dst && src ) {
        memcpy( dst, src, sizeof(Sprite) );
        dst->used_sprite = -1;
        dst->position = (vec2){0,0};

        Anim_stop( &dst->animation );
    }
}

bool Sprite_load( Sprite *s, const char *file ) {
    cJSON *root = NULL, *item = NULL;
    char *json_file = NULL;
    bool return_val = false;

    if( s && file ) {
        // read and parse json file
        Byte_ReadFile( &json_file, file );
        check( json_file, "Error while loading entity!\n" );

        root = cJSON_Parse( json_file );
        check( root, "JSON parse error [%s] before :\n%s\n", file, cJSON_GetErrorPtr() );


        // not directly initialized stuff 
        // (set when an Agent use this sprite as its representation)
        s->position = (vec2){0,0};
        s->used_sprite = -1;
        Anim_stop( &s->animation );


        // Load sprite width and height
        item = cJSON_GetObjectItem( root, "width" );
        check( item, "Error while loading sprite '%s', need entry 'width'.\n", file );

        s->mesh_size.x = item->valueint;

        item = cJSON_GetObjectItem( root, "height" );
        check( item, "Error while loading sprite '%s', need entry 'height'.\n", file );

        s->mesh_size.y = item->valueint;


        // Load textures
        item = cJSON_GetObjectItem( root, "texture" );
        s->tex_n = cJSON_GetArraySize( item );
        check( s->tex_n > 0, "Error while loading sprite '%s', need entry(array) 'texture'.\n", file );

        for( int i = 0; i < s->tex_n; ++i ) {
            cJSON *tex = cJSON_GetArrayItem( item, i );
            if( tex ) {
                const char *tex_s = tex->valuestring;

                // ask resourcemanager for the texture handle
                s->tex_id[i] = ResourceManager_get( tex_s );
                check( s->tex_id[i] >= 0, "Error while loading sprite '%s' texture. Texture '%s' is not a loaded resource.\n", file, tex_s );
            }
        }

        // For Albedo texture atlas(2048x2048), compute texcoords_size of sprite
        s->texcoords_size = (vec2){ s->mesh_size.x / 2048, s->mesh_size.y / 2048 };



        // Load generic sprite mesh (resized quad)
        str256 scaled_mesh_str;
        str16 mesh_size;
        snprintf( mesh_size, 16, "%d.%d", (int)s->mesh_size.x, (int)s->mesh_size.y );

        strcpy( scaled_mesh_str, "quadmesh.json" );
        strcat( scaled_mesh_str, mesh_size );

        s->mesh_id = ResourceManager_get( scaled_mesh_str );

        // if correctly sized mesh is not yet loaded, create it
        if( -1 == s->mesh_id ) {
            s->mesh_id = ResourceManager_get( "quadmesh.json" );
            check( s->mesh_id >= 0, "Error while loading sprite '%s' mesh. Mesh 'quadmesh.json' is not a loaded resource.\n", file );

            // resize
            int scaled_mesh = Renderer_createRescaledMesh( s->mesh_id, &s->mesh_size, &s->texcoords_size );
            check( scaled_mesh >= 0, "Error while creating scaled mesh for sprite '%s'. \n", file );
            
            s->mesh_id = scaled_mesh;

            // add newly rescaled mesh to resource manager
            ResourceManager_add( scaled_mesh_str, scaled_mesh );
        }   

        return_val = true;
    }

error:
    DEL_PTR( json_file );
    if( root ) cJSON_Delete( root );
    return return_val;
}


SpriteArray *SpriteArray_init( u32 pSize ) {
    SpriteArray *arr = byte_alloc( sizeof( SpriteArray ) );
    check_mem( arr );

    arr->mUsed = HandleManager_init( pSize );
    arr->mMeshes = byte_alloc( pSize * sizeof( u32 ) );
    arr->mTextures0 = byte_alloc( pSize * sizeof( u32 ) );
    arr->mTextures1 = byte_alloc( pSize * sizeof( int ) );
    arr->mAttributes = byte_alloc( pSize * sizeof( mat3* ) );
    arr->anims = byte_alloc( pSize * sizeof( Anim ) );

    arr->mSize = pSize;

error:
    return arr;
}

int SpriteArray_add( SpriteArray *arr ) {
    int handle = -1;
    if( arr ) {
        handle = HandleManager_addHandle( arr->mUsed, 1 );

        if( handle >= 0 ) {
            // resize our sprite array if the handle manager had to be resized
            if( arr->mUsed->mSize != arr->mSize ) {
                arr->mSize = arr->mUsed->mSize;
                arr->mMeshes = byte_realloc( arr->mMeshes, arr->mSize * sizeof( u32 ) );
                arr->mTextures0 = byte_realloc( arr->mTextures0, arr->mSize * sizeof( u32 ) );
                arr->mTextures1 = byte_realloc( arr->mTextures1, arr->mSize * sizeof( int ) );
                arr->mAttributes = byte_realloc( arr->mAttributes, arr->mSize * sizeof( mat3* ) );
                arr->anims = byte_realloc( arr->anims, arr->mSize * sizeof( Anim ) );
            }

            ++arr->mMaxIndex;
            ++arr->mCount;
        }
    }

    return handle;
}

void SpriteArray_remove( SpriteArray *arr, u32 pIndex ) {
    if( arr && pIndex < arr->mUsed->mMaxIndex )  {
        HandleManager_remove( arr->mUsed, pIndex );
        --arr->mCount;
    }
}

void SpriteArray_clear( SpriteArray *arr ) {
    if( arr ) {
        HandleManager_clear( arr->mUsed );
        arr->mMaxIndex = 0;
        arr->mCount = 0;
    }
}

void SpriteArray_destroy( SpriteArray *arr ) {
    if( arr ) {
        HandleManager_destroy( arr->mUsed );
        DEL_PTR( arr->mTextures0 );
        DEL_PTR( arr->mTextures1 );
        DEL_PTR( arr->mMeshes );
        DEL_PTR( arr->mAttributes );
        DEL_PTR( arr->anims );
        DEL_PTR( arr );
    }
}
