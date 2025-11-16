VERSION = 6.6

CC = gcc
SRC = suffracom.c gui.c main.c 

SDL2LIBS = -lSDL2 -lSDL2_image -SDL2_ttf
CFLAGS = -std=c99 -Wall -Wno-deprecated-declarations -0s 

all: suffracom

suffracom: suffracom.c gui.o
	$(CC) -o 

clean:
	rm -rf ${OBJ}
