/*
 * simobject.c
 *
 *  Created on: Jun 8, 2022
 *      Author: Dylan
 */

#include <SDL2/SDL.h>
#include "../inc/simulation.h"
#include "../inc/simobject.h"

/* ---------------------------------------------------------------------------------------- */

simobject_t *createObject(simobject_t obj)
{
    return malloc(sizeof(obj));
}

void destroyObject(simobject_t *obj)
{
    free(obj);
}