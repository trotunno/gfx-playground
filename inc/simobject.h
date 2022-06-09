/*
 *  simobject.h
 *
 *  Created on: Jun 8, 2022
 *      Author: Dylan
 */

#ifndef _INC_SIMOBJECT_H
#define _INC_SIMOBJECT_H

/* ---------------------------------------------------------------------------------------- */

#define DIMENSIONS 2

/* ---------------------------------------------------------------------------------------- */

#include "common.h"

// manages an object in the simulation
typedef struct simobject_t
{

    float position[DIMENSIONS];
    float velocity[DIMENSIONS];
    float acceleration[DIMENSIONS];

} simobject_t;

/* ---------------------------------------------------------------------------------------- */

simobject_t *createObject(simobject_t obj);
void destroyObject(simobject_t *obj);

#endif