/*
 *  simobject.c
 *
 *  Created on: Jun 8, 2022
 *      Author: Dylan
 */

#ifndef _INC_SHAPES_H
#define _INC_SHAPES_H

/* ---------------------------------------------------------------------------------------- */

#include <float.h>
#include "SDL2/SDL.h"
#include "simulation.h"

typedef struct
{

    SDL_FRect r1, r2;

} shapes_plus;

/* ---------------------------------------------------------------------------------------- */

uint8_t shapes_render_circle(simulation_t *sim, simobject_t *obj);

/* ---------------------------------------------------------------------------------------- */

#endif