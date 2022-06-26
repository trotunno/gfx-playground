/*
 *  collisions.h
 *
 *  Created on: Jun 24, 2022
 *      Author: Dylan
 */

#ifndef _INC_COLLISIONS_H
#define _INC_COLLISIONS_H

/* ---------------------------------------------------------------------------------------- */

#include <stdint.h>
#include <stdbool.h>

#include "SDL2/SDL.h"

/* ---------------------------------------------------------------------------------------- */

uint8_t detect_object_collision(SDL_FRect rect1, SDL_FRect rect2);
uint8_t detect_border_collision(SDL_FRect rect1, SDL_FRect border);

/* ---------------------------------------------------------------------------------------- */

#endif
