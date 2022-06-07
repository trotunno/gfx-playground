# compile with gcc, write build output to file
CC=gcc -o

# compiler flags
CFLAGS=-g -O0 -std=c17 -Wall -Wunused -Werror

# linker flags
LFLAGS=-lm -lSDL2 

# include paths
INCLUDE=-I/inc -I/inc/SDL2

# source / header file locations
CFILES=src/main.c src/gfx.c inc/common.h inc/main.h inc/gfx.h

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
	@$(CC) $(BINARY) $(CFILES) $(CFLAGS) $(LFLAGS) $(INCLUDE)
	@if [ !? == 0 ]; then echo -n "Build Failed!"; else echo -n "Build Successful!"; fi

# deletes the target exe + any other files that can be re-generated
clean:
	@clear
	@echo "Cleaning..."
	@rm $(BINARY)
	@if test -f "$(BINARY)"; then echo "Clean Successful!"; else echo "Clean Failed! - '$(BINARY)'  does not exist"; fi

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