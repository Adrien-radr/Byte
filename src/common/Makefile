#	///////////  	COMMON		//////////////////////
CFLAGS = -Wall -ggdb3 -I../ -I../../ext/ -lm -std=c99 -D_DEBUG -D_POSIX_C_SOURCE=200112L

OBJ = $(patsubst %.c, %.o, $(wildcard *.c) ) 

LIB = \
../../ext/json/cJSON$(ARCH).o

all: $(OBJ)
	@echo "		-- Common compiled --"

%.o: %.c
	@ $(CC) $(CFLAGS) -c $< -o $@ 
	@echo "CC	$@"

clean:
	rm -f $(OBJ)
