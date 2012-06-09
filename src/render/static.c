#include "static.h"
#include "resource.h"
#include "json/cJSON.h"

SimpleArrayDef( StaticObject, Statics )

void StaticObject_cpy( StaticObject *dst, const StaticObject *src ) {
    if( dst && src ) {
        memcpy( dst, src, sizeof(StaticObject) );
        dst->scene_id = -1;
        dst->position = (vec2){0,0};
    }
}

bool StaticObject_load( StaticObject *s, const char *file ) {
    cJSON *root = NULL, *item = NULL;
    char *json_file = NULL;
    bool return_val = false;

    if( s && file ) {
        // read and parse json file
        Byte_ReadFile( &json_file, file );
        check( json_file, "Error while loading static object!\n" );

        root = cJSON_Parse( json_file );
        check( root, "JSON parse error [%s] before :\n%s\n", file, cJSON_GetErrorPtr() );


        // not directly initialized stuff 
        // (set when a static object is used as a world obj's representation)
        s->position = (vec2){0,0};
        s->depth = 0;
        s->scene_id = -1;

        // Load obj width and height
        item = cJSON_GetObjectItem( root, "width" );
        check( item, "Error while loading static obj '%s', need entry 'width'.\n", file );

        s->mesh_size.x = item->valueint;

        item = cJSON_GetObjectItem( root, "height" );
        check( item, "Error while loading static obj '%s', need entry 'height'.\n", file );

        s->mesh_size.y = item->valueint;


        // Load obj texcoords position on atlas
        item = cJSON_GetObjectItem( root, "tex_x" );
        check( item, "Error while loading static obj '%s', need entry 'tex_x'.\n", file );

        s->texcoords_pos.x = item->valueint;

        item = cJSON_GetObjectItem( root, "tex_y" );
        check( item, "Error while loading static obj '%s', need entry 'tex_y'.\n", file );

        s->texcoords_pos.y = item->valueint;

        // Load textures
        item = cJSON_GetObjectItem( root, "texture" );
        s->tex_n = cJSON_GetArraySize( item );
        check( s->tex_n > 0, "Error while loading static obj '%s', need entry(array) 'texture'.\n", file );

        for( int i = 0; i < s->tex_n; ++i ) {
            cJSON *tex = cJSON_GetArrayItem( item, i );
            if( tex ) {
                const char *tex_s = tex->valuestring;

                // ask resourcemanager for the texture handle
                s->tex_id[i] = ResourceManager_get( tex_s );
                check( s->tex_id[i] >= 0, "Error while loading static obj '%s' texture. Texture '%s' is not a loaded resource.\n", file, tex_s );
            }
        }

        // For Albedo texture atlas(2048x2048), compute texcoords_size of sprite
        s->texcoords_size = (vec2){ s->mesh_size.x / 64, s->mesh_size.y / 200 };

        return_val = true;
    }

error:
    DEL_PTR( json_file );
    if( root ) cJSON_Delete( root );
    return return_val;
}






StaticObjectArray *StaticObjectArray_init( u32 size ) {
    StaticObjectArray *arr = byte_alloc( sizeof(StaticObjectArray) );
    check_mem( arr );

    arr->used = HandleManager_init( size );
    arr->objs = byte_alloc( size * sizeof(StaticObject*) );

    arr->size = size;

error: 
    return arr;
}

int StaticObjectArray_add( StaticObjectArray *arr ) {
    int handle = -1;

    if( arr ) {
        handle = HandleManager_addHandle( arr->used, 1 );

        if( handle >= 0 ) {
            // resize array if HM has been resized
            if( arr->used->mSize != arr->size ) {
                arr->size = arr->used->mSize;
                arr->objs = byte_realloc( arr->objs, arr->size * sizeof(StaticObject*) );
            }

            ++arr->max_index;
            ++arr->count;
        }
    }

    return handle;
}

void StaticObjectArray_remove( StaticObjectArray *arr, u32 index ) {
    if( arr && index < arr->used->mMaxIndex ) {
        HandleManager_remove( arr->used, index );
        --arr->count;
    }
}

void StaticObjectArray_clear( StaticObjectArray *arr ) {
    if( arr ) {
        HandleManager_clear( arr->used );
        arr->max_index = 0;
        arr->count = 0;
    }
}

void StaticObjectArray_destroy( StaticObjectArray *arr ) {
    if( arr ) {
        HandleManager_destroy( arr->used );
        DEL_PTR( arr->objs );
        DEL_PTR( arr );
    }
}
