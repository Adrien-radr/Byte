NAME = Byte-Project
VERSION_MAJOR = 0
VERSION_MINOR = 0
VERSION_PATCH = 7
VERSION = $(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)

ARCH = 64


################################################################


CC = gcc

CFLAGS = -Wall -ggdb3 -Isrc/ -lX11 -lXrandr -lGL -llua -lIL -lILU -std=c99 -D_DEBUG

OBJ = \
src/clock.o\
src/color.o\
src/context.o\
src/common.o\
src/device.o\
src/event.o\
src/main.o\
src/matrix.o\
src/mesh.o\
src/renderer.o\
src/shader.o\
src/vector.o


LIB = \
ext/libglfw$(ARCH).a\
ext/GL/glew$(ARCH).o


.PHONY: all, clean, byte

all: 
	@echo "---- Building $(NAME) $(VERSION) ----"
	@make byte
	@echo "---- $(NAME) $(VERSION) built ----"
	
byte: $(OBJ)
	@ $(CC) $(CFLAGS) $(OBJ) $(LIB) -o bin/byte
	
%.o: %.c
	@ $(CC) $(CFLAGS) -c $< -o $@
	@echo "CC	$@"

clean:
	rm $(OBJ)
