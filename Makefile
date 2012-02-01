NAME = Byte-Project
VERSION_MAJOR = 0
VERSION_MINOR = 0
VERSION_PATCH = 13
VERSION = $(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)

ARCH = 64

################################################################

CC = gcc

CFLAGS = -Wall -ggdb3 -Isrc/ -Iext/ -Iext/freetype -lX11 -lXrandr -lGL -lz -lbz2 -llua -std=c99 -D_DEBUG


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
ext/libSOIL$(ARCH).a\
ext/libfreetype.a\
ext/GL/glew$(ARCH).o\
ext/json/cJSON$(ARCH).o


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
