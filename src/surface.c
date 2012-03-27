#include "surface.h"

Surface* Surface_loadTexture(const char* file){
    Surface* s = byte_alloc(sizeof(Surface));
    s->mTexture = Renderer_createTexture(file, true);
    return s;
}
