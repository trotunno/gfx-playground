# compile with gcc, write build output to file
CC=gcc -o

# compiler flags
CFLAGS=-g -O0 -std=c11 -Werror

# library links
LFLAGS=-lm -LC:/msys64/mingw64/lib -lSDL2

# include paths
INCLUDE=-I/inc -IC:/msys64/mingw64/include/SDL2 -I/inc/sdl2_gfx

# library header files
LHFILES=inc/gfx-primitives/primitives.h

# header files
HFILES=inc/common.h inc/shapes.h inc/simobject.h inc/userinteractions.h inc/simulation.h inc/eventhandler.h inc/collisions.h inc/main.h

# library source files
LCFILES=inc/gfx-primitives/primitives.c

# source files
CFILES= src/common.c src/shapes.c src/simobject.c src/simulation.c src/eventhandler.c src/collisions.c src/main.c 

# build directory 
BUILD=builds

# exe location
BINARY=$(BUILD)/gfx-playground.exe

# file descriptor that allows for output to be piped into oblivion, never to be seen again
FD=</dev/null >/dev/null 2>&1 &

# default build target
all: gfx-playground

# cleans and builds the exe
fresh: clrscrn clean newline gfx-playground newline run

# builds the exe, stderr goes to terminal
gfx-playground: $(CFILES)
	@echo "Building..."
	@$(CC) $(BINARY) $(LHFILES) $(HFILES) $(LCFILES) $(CFILES) $(CFLAGS) $(LFLAGS) $(INCLUDE)
	@if [ !? == 0 ]; then echo -n "Build Failed!"; else echo -n "Build Successful!"; fi

# deletes the target exe + any other files that can be re-generated
clean:
	@clear
	@echo "Cleaning..."
	@rm $(BINARY)
	@if test -f "$(BINARY)"; then echo "Clean Successful!"; else echo "Clean Failed! - '$(BINARY)' does not exist"; fi

# runs the target exe
run: clrscrn gfx-playground
	@echo " "
	@echo " "
	@echo "Running..."
	@echo "-------------------------"
	@./$(BINARY)
	@echo "-------------------------"
	@echo "Done!"

# clears the terminal screen
clrscrn:
	@clear

# prints a newline
newline:
	@echo " "