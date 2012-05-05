#ifndef BYTE_GUI_H
#define BYTE_GUI_H

#include "common/common.h"
#include "common/vector.h"

typedef struct {
    bool                visible;
    vec2i               position;
    vec2i               size;
    int                 depth;

    struct {
        u32 mesh;
        u32 texture;
    }                   assets;
} Widget;

/// Initialize the given widget (build the mesh);
void Widget_init( Widget *widget, const char *mesh, const char *texture );

#endif /* BYTE_GUI */
