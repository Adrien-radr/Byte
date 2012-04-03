#	///////////  	COMMON		//////////////////////
CO_OBJ = $(patsubst %.c, %.o, $(wildcard ../common/*.c) ) 


#	///////////  	SERVER		//////////////////////
CFLAGS = -Wall -ggdb3 -I../ -I../../ext/ -lm -lrt -std=c99 -D_DEBUG -D_POSIX_C_SOURCE=200112L

OBJ = $(patsubst %.c, %.o, $(wildcard *.c) )

LIB = \
../../ext/json/cJSON$(ARCH).o



all: $(OBJ) $(CO_OBJ)
	@ $(CC) $(CFLAGS) $(CO_OBJ) $(OBJ) $(LIB) -o ../../bin/sv
	@echo "		-- Server compiled --"

%.o: %.c
	@ $(CC) $(CFLAGS) -c $< -o $@ 
	@echo "CC	$@"

clean:
	rm -f $(OBJ)
