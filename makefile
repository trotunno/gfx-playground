CC=gcc
CFLAGS=`sdl2-config --libs --cflags` -g -O0 -std=c17 -lm -Werror -I.
CFILES=main.c common.h gfx/gfx.c gfx/gfx.h
DEBUG=builds/debug/gfx_debug.o
RELEASE=builds/release/gfx_release.o

debug: $(CFILES)
	$(CC) -o $(DEBUG) $(CFILES) $(CFLAGS)

release: $(CFILES)
	$(CC) -o $(RELEASE) $(CFILES) $(CFLAGS)

clean:
	rm $(DEBUG) $(RELEASE)