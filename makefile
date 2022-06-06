# compile with gcc, write build output to file
CC=gcc -o

# compiler flags
CFLAGS=-g -O0 -std=c11 -Wall -Wunused -Werror

# linker flags
LFLAGS=-lm -lSDL2 

# include paths
INCLUDE=-I/inc -I/inc/SDL2

# source / header file locations
CFILES=src/main.c src/gfx.c inc/common.h inc/main.h inc/gfx.h

# build directory 
BUILD=builds/

# exe location
BINARY=$(BUILD)/gfx-playground.exe

# file descriptor that allows for output to be piped into oblivion, never to be seen again
FD=</dev/null >/dev/null 2>&1 &

# flag that gets set to 1 because idk how to write bash scripts properly
FLAG=0

# default build target
all: clrscrn gfx-playground

# builds the exe, stderr goes to terminal and log file called "stderr" in the build directory
gfx-playground: $(CFILES)
	@echo "Building..."
	@$(CC) $(BINARY) $(CFILES) $(CFLAGS) $(LFLAGS) $(INCLUDE)
	@if [ !? == 0 ]; then echo -n "Build Failed!"; else echo -n "Build Successful!"; fi
	@FLAG=1

# cleans and builds the exe
fresh: clean gfx-playground run

# clears the terminal screen
clrscrn:
	@clear

# prints a newline
newline:
	@echo ""

# deletes the target exe + stderr log file
clean:
	@clear
	@echo "Cleaning..."
	@rm $(BINARY)
	@echo -n "Done!"

# runs the target exe
run: clrscrn gfx-playground newline
	@echo "Running..."
	@echo "-------------------------"
	@./$(BINARY)
	@echo "-------------------------"
	@echo -n "Done!"

# targets that have names conflicting with a file
.PHONY: clrscrn gfx-playground