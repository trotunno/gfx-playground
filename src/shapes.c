/*
 *  simobject.c
 *
 *  Created on: Jun 8, 2022
 *      Author: Dylan
 */

#include <stdint.h>
#include <stdio.h>

#include "../inc/SDL2/SDL.h"
#include "../inc/gfx-primitives/primitives.h"
#include "../inc/simulation.h"
#include "../inc/shapes.h"

void shapes_render_circle(simulation_t *sim)
{

    filledEllipseColor
    (
        sim->sdl->renderer, 
        200, 
        200,
        50,
        50,
        0xffffffff
    );
    SDL_RenderPresent(sim->sdl->renderer);

}