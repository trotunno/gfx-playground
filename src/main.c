/*
 * main.c
 *
 *  Created on: Jun 4, 2022
 *      Author: Dylan
 */

#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include "../inc/main.h"
#include "../inc/simulation.h"

/* ---------------------------------------------------------------------------------------- */

int main(int argc, char **argv)
{

    simulation_t *simulation = malloc(sizeof(simulation_t));

    simulation_init(simulation);

    simulation_start(simulation);

    return 0;
}