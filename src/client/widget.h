#ifndef BYTE_WIDGET_H
#define BYTE_WIDGET_H

#include "common/handlemanager.h"
#include "common/matrix.h"
#include "text.h"

/// Data-oriented array storing widgets existing in the scene
typedef struct {
    HandleManager   *used;         ///< HandleManager telling if an index is used

    u32             *meshes;
    u32             *textures;
    u32             *texts;
    int              *depths;
    vec2            *positions;     ///< Widget position (screen coords)
    vec2            *textOffsets;


    u32             count,
                    size,
                    max_index;
} WidgetArray;


typedef enum {
    WA_Texture,
    WA_Position,
    WA_Depth,
} WidgetAttrib;

/// Initialize and allocate a new WidgetArray
WidgetArray* WidgetArray_init( u32 size );

/// Add a new widget to the widget array and returns its handle (or -1 if any error)
int WidgetArray_add( WidgetArray* wa );

/// Remove a widget from the given widget array, by its index
void WidgetArray_remove( WidgetArray* wa, u32 index );

/// Clears the whole given widget array
void WidgetArray_clear( WidgetArray* wa );

/// Destroy and free the given widget array
void WidgetArray_destroy( WidgetArray* wa );

#endif // BYTE_WIDGET
