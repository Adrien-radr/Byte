#	///////////  	COMMON		//////////////////////
CO_OBJ = $(patsubst %.c, %.o, $(wildcard ../common/*.c) ) 


#	///////////  	CLIENT		//////////////////////
CFLAGS = -Wall -ggdb3 -I../ -I../../ext/ -I/usr/include/freetype2 -lm $(GL_FLAG) -lGL -lXrandr -lX11 -lpng -lz -lbz2 -lrt -ldl -lpthread -std=c99 -D_DEBUG

OBJ = $(patsubst %.c, %.o, $(wildcard *.c) )

LIB = \
../../ext/libglfw$(ARCH).a\
../../ext/json/cJSON$(ARCH).o\
../../$(GL_LIB) \
/usr/lib/libfreetype.a

all: $(OBJ) $(CO_OBJ)
	@ $(CC) $(CFLAGS) $(CO_OBJ) $(OBJ) $(LIB) -o ../../bin/cl
	@echo "		-- Client compiled --"

%.o: %.c
	@ $(CC) $(CFLAGS) -c $< -o $@ 
	@echo "CC	$@"

clean:
	rm -f $(OBJ)
