NAME = Byte-Project
VERSION_MAJOR = 0
VERSION_MINOR = 1
VERSION_PATCH = 5
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

.PHONY: all, client, server, common, ext, coclean, clclean, svclean, clean, cleaner


all: 
	@echo "---- Building $(NAME) $(VERSION) ----"
	@echo ""
	@echo "	-- Building external libs --"
	@echo "(This may take some time, please wait)"
	@make ext
	@echo "	-- External libs built --"
	@echo ""
	@make common
	@make server
	@make client
	@echo "---- $(NAME) $(VERSION) built ----"

ext:
	@make -C ext/ ARCH=$(ARCH)

common:
	@make -C src/common ARCH=$(ARCH)

server:
	@make -C src/server ARCH=$(ARCH)

client:
	@make -C src/client GL_FLAG=$(GL_FLAG) GL_LIB=$(GL_LIB) ARCH=$(ARCH)

coclean:
	@make -C src/common clean

clclean:
	@make -C src/client clean

svclean:
	@make -C src/server clean

clean:
	@make -C src/common clean
	@make -C src/server clean
	@make -C src/client clean

cleaner: clean
	make -C ext/ ARCH=$(ARCH) clean
	rm -f bin/sv bin/cl

