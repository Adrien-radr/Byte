#include "widget.h"
#include "renderer.h"

#include "GL/glew.h"


WidgetArray* WidgetArray_init( u32 size ){
    WidgetArray *arr = byte_alloc( sizeof( WidgetArray ) );
    check_mem( arr );


    arr->used = HandleManager_init( size );
    arr->meshes = byte_alloc( size * sizeof( u32 ) );
    arr->textures = byte_alloc( size * sizeof( u32 ) );
    arr->depths = byte_alloc( size * sizeof( int ) );
    arr->positions = byte_alloc( size * sizeof( vec2 ) );


    arr->size = size;

error:
    return arr;
}

int WidgetArray_add( WidgetArray* arr ) {
    int handle = -1;
    if( arr ) {
        handle = HandleManager_addHandle( arr->used, 1 );
        if( handle >= 0 ) {
            // resize our entity array if the handle manager had to be resized
            if( arr->used->mSize != arr->size ) {
                arr->size = arr->used->mSize;
                arr->meshes = byte_realloc( arr->meshes, arr->size * sizeof( u32 ) );
                arr->textures = byte_realloc( arr->textures, arr->size * sizeof( u32 ) );
                arr->depths = byte_realloc( arr->depths, arr->size * sizeof( int ) );
                arr->positions = byte_realloc( arr->positions, arr->size * sizeof( vec2 ) );
            }

            ++arr->max_index;
            ++arr->count;
        }
    }

    return handle;
}

void WidgetArray_remove( WidgetArray *arr, u32 index ) {
    if( arr && index < arr->used->mMaxIndex )  {
        HandleManager_remove( arr->used, index );
        --arr->count;
    }
}

void WidgetArray_clear( WidgetArray *arr ) {
    if( arr ) {
        HandleManager_clear( arr->used );
        arr->max_index = 0;
        arr->count = 0;
    }
}

void WidgetArray_destroy( WidgetArray *arr ) {
    if( arr ) {
        HandleManager_destroy( arr->used );
        DEL_PTR( arr->meshes );
        DEL_PTR( arr->textures );
        DEL_PTR( arr->positions );
        DEL_PTR( arr->depths );
        DEL_PTR( arr );
    }
}
