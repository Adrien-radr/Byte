NAME = Byte-Project
VERSION_MAJOR = 0
VERSION_MINOR = 1
VERSION_PATCH = 3
VERSION = $(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)

ARCH = 64

################################################################

CC = gcc

USE_GLDL = 0

# Use GLDL lib for debug if wanted, instead of glew
ifeq ($(USE_GLDL), 1)
GL_LIB = ext/GL/gldl.o
GL_FLAG = -DUSE_GLDL
else
GL_LIB = ext/GL/glew$(ARCH).o
GL_FLAG = 
endif

.PHONY: all, client, server, ext, clean, cleaner


all: 
	@echo "---- Building $(NAME) $(VERSION) ----"
	@echo ""
	@echo "	-- Building external libs --"
	@echo "(This may take some time, please wait)"
	@make ext
	@echo "	-- External libs built --"
	@echo ""
	@make server
	@make client
	@echo "---- $(NAME) $(VERSION) built ----"

ext:
	@make -C ext/ ARCH=$(ARCH)

server:
	@make -C src/server ARCH=$(ARCH)

client:
	@make -C src/client GL_LIB=$(GL_LIB) ARCH=$(ARCH)

clean:
	rm -f src/common/*.o
	@make -C src/server clean
	@make -C src/client clean

cleaner: clean
	make -C ext/ ARCH=$(ARCH) clean
	rm -f bin/sv bin/cl




CFLAGS = -Wall -ggdb3 -Isrc/ -Iext/ -Iext/freetype -lX11 -lXrandr -lGL -lz -lbz2 -lpng -llua -std=c99 -D_DEBUG -lm -lpthread -ldl -lrt $(GL_FLAG) -D_POSIX_C_SOURCE=200112L

