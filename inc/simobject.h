/*
 *  simobject.h
 *
 *  Created on: Jun 8, 2022
 *      Author: Dylan
 */

#ifndef _INC_SIMOBJECT_H
#define _INC_SIMOBJECT_H

/* ---------------------------------------------------------------------------------------- */

#include <SDL2/SDL.h>
#include "common.h"

// contains properties of the field the object's physics must adhere to
typedef struct fieldproperties_t
{

    float timestep;

    float xvel_constant;
    float yvel_constant;

    float xacc_constant;
    float yacc_constant;

    float positive_x_boundary;
    float negative_x_boundary;
    float positive_y_boundary;
    float negative_y_boundary;

} fieldproperties_t;

// manages an object in the simulation
typedef struct simobject_t
{

    float mass;

    float width;
    float height;

    float x_pos, y_pos;
    float x_vel, y_vel;
    float x_acc, y_acc;

} simobject_t;

/* ---------------------------------------------------------------------------------------- */

void destroyObject(simobject_t *obj);
simobject_t* createObject(SDL_FRect rect, float mass, float x_vel, float y_vel, float x_acc, float y_acc);

void simobject_update_state(simobject_t *obj, fieldproperties_t props);

#endif