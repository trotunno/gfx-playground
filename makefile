#!/bin/bash

# compile with gcc, write build output to file
CC=gcc -o

# compile flags
CFLAGS=-g -O0 -std=c17 -lm `sdl2-config --cflags --libs` -Werror

# include directory
INCLUDE=-I/mnt/c/users/green/documents/cprojects/gfx-playground/inc

# source / header file locations
CFILES=src/main.c src/gfx.c inc/common.h inc/main.h inc/gfx.h

# build directory 
BUILD=builds/

# exe location
BINARY=$(BUILD)/gfx-playground

# stderr log location
STDERR=$(BUILD)/stderr

# file descriptor that allows for output to be piped into oblivion, never to be seen again
FD=</dev/null >/dev/null 2>&1 &

# default build target
all: gfx-playground

# builds the exe, stderr goes to terminal and log file called "stderr" in the build directory
gfx-playground: $(CFILES)
	@echo "Building..."
	@$(CC) $(BINARY) $(CFILES) $(CFLAGS) $(INCLUDE) 2>&1 1>/dev/null | tee $(STDERR)
	@if [ -s $(BUILD)\stderr ]; then echo "Build Failed!"; else echo "Build Successful!"; fi

# cleans and builds the exe
fresh: clean gfx-playground run

# deletes the target exe + stderr log file
clean:
	@clear
	@echo "Cleaning..."
	@rm $(BINARY) $(STDERR)
	@echo "Done!"

# runs the target exe
run: gfx-playground
	@echo "Running..."
	@echo "-------------------------"
	@./$(BINARY)

# targets that have names conflicting with a file
.PHONY: gfx-playground