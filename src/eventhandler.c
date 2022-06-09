/*
 * eventhandler.c
 *
 *  Created on: Jun 6, 2022
 *      Author: Dylan
 */

/* ---------------------------------------------------------------------------------------- */

#include "../inc/simulation.h"
#include "../inc/eventhandler.h"

/* ---------------------------------------------------------------------------------------- */

void evt_sdl_quit_handler(SDL_Event *event, simulation_t *sim)
{
    sim->running = false;
}

void evt_sdl_keydown_handler(SDL_Event *event, simulation_t *sim)
{

    switch (event->key.keysym.scancode)
    {

        case SDL_SCANCODE_SPACE:
            sim->userinteractions.space_pressed = true;
            break;

        case SDL_SCANCODE_ESCAPE:
            sim->userinteractions.escape_pressed = true;
            break;

        default:
            break;
    }

}

void evt_sdl_keyup_handler(SDL_Event *event, simulation_t *sim)
{

    switch (event->key.keysym.scancode)
    {

        case SDL_SCANCODE_SPACE:
            sim->userinteractions.space_pressed = false;
            break;

        case SDL_SCANCODE_ESCAPE:
            sim->userinteractions.escape_pressed = false;

        default:
            break;
    }

}