/*
 *  simobject.h
 *
 *  Created on: Jun 8, 2022
 *      Author: Dylan
 */

#ifndef _INC_SIMOBJECT_H
#define _INC_SIMOBJECT_H

/* ---------------------------------------------------------------------------------------- */

#include "SDL2/SDL.h"
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

    float max_x_pos;
    float max_y_pos;
    float max_x_vel;
    float max_y_vel;
    float max_x_acc;
    float max_y_acc;

} fieldproperties_t;

// manages an object in the simulation
typedef struct simobject_t
{

    uint8_t color_r, color_g, color_b;

    float mass;

    float width;
    float height;

    float x_pos, y_pos;
    float x_vel, y_vel;
    float x_acc, y_acc;

    float intr_x_vel, intr_y_vel;
    float intr_x_acc, intr_y_acc;

    float momentum;

} simobject_t;

/* ---------------------------------------------------------------------------------------- */

void destroyObject(simobject_t *obj);
simobject_t* createObject
(
    float mass, float x_pos, float y_pos, float x_vel, float y_vel, float x_acc, float y_acc,
    float intr_x_vel, float intr_y_vel, float intr_x_acc, float intr_y_acc
);
void simobject_update_state(simobject_t *obj, fieldproperties_t props);
void simobject_collision(simobject_t *obj1, simobject_t *obj2, fieldproperties_t props);

#endif