#	///////////  	COMMON		//////////////////////
CO_OBJ = $(patsubst %.c, %.o, $(wildcard common/*.c) ) 

#	///////////  	RENDER		//////////////////////
RE_OBJ = $(patsubst %.c, %.o, $(wildcard render/*.c) ) 

#	///////////  	GAME		//////////////////////
GA_OBJ = $(patsubst %.c, %.o, $(wildcard *.c) )


CFLAGS = -Wall -ggdb3 -I./ -I../ext/ -I/usr/include/freetype2 -lm -lGL -lXrandr -lX11 -lpng -lz -lbz2 -lrt -ldl -lpthread -std=c99 -D_DEBUG -DPNG_SKIP_SETJMP_CHECK -D_POSIX_C_SOURCE=199309L


LIB = \
../ext/libglfw$(ARCH).a\
../ext/json/cJSON$(ARCH).o\
../ext/GL/glew$(ARCH).o \
/usr/lib/x86_64-linux-gnu/libfreetype.a

all: $(CO_OBJ) $(RE_OBJ) $(GA_OBJ)
	@ $(CC) $(CO_OBJ) $(RE_OBJ) $(GA_OBJ) $(LIB) $(CFLAGS) -o ../bin/byte
	@echo "		-- Game compiled --"

%.o: %.c
	@ $(CC) $(CFLAGS) -c $< -o $@ 
	@echo "CC	$@"

clean:
	rm -f $(CO_OBJ) $(RE_OBJ) $(GA_OBJ)
