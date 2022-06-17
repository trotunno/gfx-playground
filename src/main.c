/*
 *  main.c
 *
 *  Created on: Jun 4, 2022
 *      Author: Dylan
 */

#define SDL_MAIN_HANDLED

#include "../inc/SDL2/SDL.h"
#include "../inc/SDL2/SDL_render.h"
#include "../inc/SDL2/SDL_video.h"
#include "../inc/SDL2/SDL_surface.h"
#include "../inc/main.h"
#include "../inc/simulation.h"
#include "../inc/simobject.h"

/* ---------------------------------------------------------------------------------------- */

int main(int argc, char **argv)
{   

    simulation_t *simulation = malloc(sizeof(simulation_t));

    printf("initializing...\n");

    simulation_init(simulation);

    printf("starting sim...\n");

    simulation_start(simulation);

    printf("killing sim...\n");

    simulation_kill(simulation);

}