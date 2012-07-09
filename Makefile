NAME = Byte-Project
VERSION_MAJOR = 0
VERSION_MINOR = 1
VERSION_PATCH = 9
VERSION = $(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)

ARCH = 64

################################################################

CC = gcc

GL_LIB = 

.PHONY: all, common, ext, clean, cleaner

all: 
	@echo "---- Building $(NAME) $(VERSION) ----"
	@echo ""
	@echo "	-- Building external libs --"
	@echo "(This may take some time, please wait)"
	@make ext
	@echo "	-- External libs built --"
	@echo ""
	@make game
	@echo "---- $(NAME) $(VERSION) built ----"

ext:
	@make -C ext/ ARCH=$(ARCH)

game:
	@make -C src/ ARCH=$(ARCH)

clean:
	@make -C src/ clean

cleaner: clean
	make -C ext/ ARCH=$(ARCH) clean
	rm -f bin/byte

