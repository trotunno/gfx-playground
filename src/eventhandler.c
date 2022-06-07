/*
 * eventhandler.c
 *
 *  Created on: Jun 6, 2022
 *      Author: Dylan
 */

#include "../inc/eventhandler.h"
#include "../inc/gfx.h"

void evt_sdl_keydown_handler(SDL_Event *event, simulation_t *sim)
{

    switch (event->key.keysym.scancode)
    {
        case SDL_SCANCODE_UP:
        case SDL_SCANCODE_SPACE:
            sim->userinteractions.jump_pressed = true;
            break;

        case SDL_SCANCODE_A:
        case SDL_SCANCODE_LEFT:
            sim->userinteractions.leftarrow_pressed = true;
            break;

        case SDL_SCANCODE_D:
        case SDL_SCANCODE_RIGHT:
            sim->userinteractions.rightarrow_pressed = true;
            break;

        default:
            break;
    }
}