CC=gcc
CFLAGS=-g -O0 -std=c17 -lm -F -lSDL2main -lSDL2 -Werror
SDLFLAGS=`sdl2-config --cflags --libs`
CFILES=src/main.c inc/common.h src/gfx/gfx.c src/gfx/gfx.h
DEBUG=builds/debug/gfx_debug.o
RELEASE=builds/release/gfx_release.o

debug: $(CFILES)
	$(CC) -o $(DEBUG) $(CFILES) $(SDLFLAGS) $(CFLAGS) 

release: $(CFILES)
	$(CC) -o $(RELEASE) $(CFILES) $(SDLFLAGS) $(CFLAGS)

clean:
	rm $(DEBUG) $(RELEASE)