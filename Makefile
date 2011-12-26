NAME = Byte-Project
VERSION_MAJOR = 0
VERSION_MINOR = 0
VERSION_PATCH = 1
VERSION = $(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)

ARCH = 64


################################################################


CC = gcc

CFLAGS = -Wall -g -Isrc/ -lX11 -lXrandr -lGL -llua -lIL -lILU

OBJ = \
src/main.o


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
	
%.o: %.cpp
	@ $(CC) $(CFLAGS) -c $< -o $@
	@echo "CC	$@"

clean:
	rm $(OBJ)
