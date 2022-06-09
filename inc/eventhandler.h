/*
 * eventhandler.h
 *
 *  Created on: Jun 6, 2022
 *      Author: Dylan
 */

#ifndef _INC_EVENTHANDLER_H
#define _INC_EVENTHANDLER_H

/* ---------------------------------------------------------------------------------------- */

#include <SDL2/SDL.h>
#include "common.h"
#include "simulation.h"

/* ---------------------------------------------------------------------------------------- */

void evt_sdl_quit_handler(SDL_Event *event, simulation_t *sim);
void evt_sdl_keydown_handler(SDL_Event *event, simulation_t *sim);
void evt_sdl_keyup_handler(SDL_Event *event, simulation_t *sim);

#endif