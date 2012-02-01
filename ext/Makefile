# This makefile receive ARCH parameter from ../Makefile.
# This makefiel is used to compile external lib src

CC = gcc

CFLAGS = -Wall -O2 -ggdb3 -IGL/ -Ijson/ -std=c99

OBJ = \
GL/glew$(ARCH).o\
json/cJSON$(ARCH).o

.PHONY: all, clean

all: $(OBJ)

GL/glew$(ARCH).o : GL/glew.c
	@ $(CC) $(CFLAGS) -c $< -o $@
	@echo "CC	$@"

json/cJSON$(ARCH).o : json/cJSON.c
	@ $(CC) $(CFLAGS) -c $< -o $@
	@echo "CC	$@"

clean:
	rm $(OBJ)
