#include "gui.h"
#include "game.h"
#include "resource.h"
#include "renderer.h"

void Widget_init( Widget *widget, const char *mesh, const char *texture ) {
    check( game, "Can't init widget without running game.\n" );

    if( widget && mesh && texture ) {
        // load scaled mesh
        str256 scaled_mesh_str;
        str16 mesh_size;
        snprintf( mesh_size, 16, "%d.%d", widget->size.x, widget->size.y );
        
        strcpy( scaled_mesh_str, mesh );
        strcat( scaled_mesh_str, mesh_size );

        widget->assets.mesh = ResourceManager_get( scaled_mesh_str );
        // if correctly sized mesh is not yet loaded, create it
        if( -1 == widget->assets.mesh ) {
            widget->assets.mesh = ResourceManager_get( mesh );
            check( widget->assets.mesh >= 0, "Error while loading widget mesh. Mesh '%s' is not a loaded resource.\n", mesh );

            // resize
            vec2 size = vec2_vec2i( &widget->size );
            int scaled_mesh = Renderer_createRescaledMesh( widget->assets.mesh, &size );
            check( scaled_mesh >= 0, "Error while creating scaled mesh for widget.\n" );

            widget->assets.mesh = scaled_mesh;

            // add newly rescaled mesh to resource manager
            ResourceManager_add( scaled_mesh_str, scaled_mesh );
        }

        // texture
        widget->assets.texture = ResourceManager_get( texture );
    }

error:
    return;
}
