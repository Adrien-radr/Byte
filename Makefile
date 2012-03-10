NAME = Byte-Project
VERSION_MAJOR = 0
VERSION_MINOR = 1
VERSION_PATCH = 0
VERSION = $(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)

ARCH = 64

################################################################

CC = gcc

USE_GLDL = 0

# Use GLDL lib for debug if wanted, instead of glew
ifeq ($(USE_GLDL), 1)
GL_LIB = ext/libgldl.a
GL_FLAG = -DUSE_GLDL
else
GL_LIB = ext/GL/glew$(ARCH).o
GL_FLAG = 
endif

CFLAGS = -Wall -ggdb3 -Isrc/ -Iext/ -Iext/freetype -lX11 -lXrandr -lGL -lz -lbz2 -lpng -llua -std=c99 -D_DEBUG -lm -lpthread -ldl $(GL_FLAG)


OBJ = \
src/actor.o\
src/camera.o\
src/clock.o\
src/color.o\
src/context.o\
src/common.o\
src/debug.o\
src/device.o\
src/entity.o\
src/event.o\
src/game.o\
src/handlemanager.o\
src/main.o\
src/matrix.o\
src/mesh.o\
src/renderer.o\
src/resource.o\
src/shader.o\
src/scene.o\
src/text.o\
src/texture.o\
src/vector.o\
src/world.o


LIB = \
ext/libglfw$(ARCH).a\
ext/libfreetype.a\
ext/json/cJSON$(ARCH).o\
$(GL_LIB)


.PHONY: all, clean, byte, cleaner

all: 
	@echo "---- Building $(NAME) $(VERSION) ----"
	@echo ""
	@echo "	-- Building external libs --"
	@echo "(This may take some time, please wait)"
	@make -C ext/ ARCH=$(ARCH)
	@echo "	-- External libs built --"
	@echo ""
	@make byte
	@echo "---- $(NAME) $(VERSION) built ----"
	
byte: $(OBJ)
	@ $(CC) $(CFLAGS) $(OBJ) $(LIB) -o bin/byte
	
%.o: %.c
	@ $(CC) $(CFLAGS) -c $< -o $@ 
	@echo "CC	$@"

clean:
	rm -f $(OBJ)

cleaner: clean
	make -C ext/ ARCH=$(ARCH) clean
