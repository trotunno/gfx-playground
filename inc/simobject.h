/*
 *  simobject.h
 *
 *  Created on: Jun 8, 2022
 *      Author: Dylan
 */

#ifndef _INC_SIMOBJECT_H
#define _INC_SIMOBJECT_H

/* ---------------------------------------------------------------------------------------- */

#define DOF 2

/* ---------------------------------------------------------------------------------------- */

#include "common.h"

// possible types to allocate memory for the simulation object
typedef enum simobject_type_t
{
    point,
    line,
    rectangle,
    textureimage

} simobject_type_t;

// manages an object in the simulation
typedef struct simobject_t
{

    float x_pos, y_pos;
    float x_vel, y_vel;
    float x_acc, y_acc;

} simobject_t;

/* ---------------------------------------------------------------------------------------- */

simobject_t *createObject(float x_pos_i, float y_pos_i, float x_vel_i, float y_vel_i, float x_acc_i, float y_acc_i);
void destroyObject(simobject_t *obj);

#endif