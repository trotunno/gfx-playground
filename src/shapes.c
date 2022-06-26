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

uint8_t shapes_render_circle(simulation_t *sim, simobject_t *obj)
{

    // retrieve the x and y origins in window space
    float window_x_origin = sim->properties->border.x + (sim->properties->border.w / 2.0f);
    float window_y_origin = sim->properties->border.y + (sim->properties->border.h / 2.0f);

    // convert object's x-y coordinates to window coordinates
    float window_x_pos = window_x_origin + obj->x_pos;
    float window_y_pos = window_y_origin + obj->y_pos;

    // create rectangle from the object's current state
    SDL_FRect rect = { window_x_pos, window_y_pos, obj->height, obj->width };

    // create the object color word
    uint32_t obj_color = ((obj->color_r << 24) | (obj->color_g << 16) | (obj->color_b << 8) | 0xFF);

    return filledEllipseColor
    (
        sim->sdl->renderer, 
        rect.x,
        rect.y,
        rect.w/1.25,
        rect.h/1.25,
        obj_color
    );

}