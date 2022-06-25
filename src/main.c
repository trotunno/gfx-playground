/*
 *  main.c
 *
 *  Created on: Jun 4, 2022
 *      Author: Dylan
 */

#define SDL_MAIN_HANDLED

#include <stdio.h>
#include <stdbool.h>

#include "../inc/SDL2/SDL.h"
#include "../inc/main.h"
#include "../inc/simulation.h"
#include "../inc/simobject.h"

/* ---------------------------------------------------------------------------------------- */

int main(int argc, char **argv)
{   

    char input;

    // disable stdout buffering
    setbuf(stdout, NULL);

    simulation_t *simulation;

    simulation = malloc(sizeof(simulation_t));

    printf("initializing...\n");
    simulation_init(simulation);

    printf("starting sim...\n");
    simulation_start(simulation);

    //* TODO: stop this from making me lose focus on my console window
    printf("killing sim...\n");
    simulation_kill(simulation);

    return 0;

}

