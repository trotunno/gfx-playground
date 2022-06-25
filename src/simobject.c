/*
 *  simobject.c
 *
 *  Created on: Jun 8, 2022
 *      Author: Dylan
 */

/* ---------------------------------------------------------------------------------------- */

#include <stdio.h>
#include <float.h>
#include <math.h>

#include "../inc/common.h"
#include "../inc/simulation.h"
#include "../inc/simobject.h"

/* ---------------------------------------------------------------------------------------- */

static void simobject_update_acceleration(simobject_t *obj, fieldproperties_t props);
static void simobject_update_velocity(simobject_t *obj, fieldproperties_t props);
static void simobject_update_momentum(simobject_t *obj, fieldproperties_t props);
static void simobject_update_position(simobject_t *obj, fieldproperties_t props);

/* ---------------------------------------------------------------------------------------- */

//* TODO: make these function arguments variadic, default values all == 0. Supplying all these for an object is goofy
simobject_t * createObject
(
    float mass, float x_pos, float y_pos, float x_vel, float y_vel, float x_acc, float y_acc,
    float intr_x_vel, float intr_y_vel, float intr_x_acc, float intr_y_acc
)
{

    simobject_t *obj = malloc(sizeof(simobject_t));

    obj->mass = mass;

    obj->width = mass * 0.5f;
    obj->height = mass * 0.5f;

    obj->x_pos = x_pos;
    obj->y_pos = y_pos;
    obj->x_vel = x_vel;
    obj->y_vel = y_vel;
    obj->x_acc = x_acc;
    obj->y_acc = y_acc;

    obj->intr_x_vel = intr_x_vel;
    obj->intr_y_vel = intr_y_vel;
    obj->intr_x_acc = intr_x_acc;
    obj->intr_y_acc = intr_y_acc;

    return obj;

}

void destroyObject(simobject_t *obj)
{
    free(obj);
}

void simobject_update_state(simobject_t *obj, fieldproperties_t props)
{

    simobject_update_acceleration(obj, props);
    simobject_update_velocity(obj, props);
    simobject_update_momentum(obj, props);
    simobject_update_position(obj, props);

}

/* ---------------------------------------------------------------------------------------- */

static void simobject_update_acceleration(simobject_t *obj, fieldproperties_t props)
{

    #if (SIMULATION_CONSTANT_ACCELERATION)
    {
        obj->x_acc = props.xacc_constant;
        obj->y_acc = props.yacc_constant;
    }
    #endif

    // bounding
    if (obj->x_vel >  props.max_x_acc) obj->x_vel =  props.max_x_acc;
    if (obj->x_vel < -props.max_x_acc) obj->x_vel = -props.max_x_acc;
    if (obj->y_vel >  props.max_y_acc) obj->y_vel =  props.max_y_acc;
    if (obj->y_vel < -props.max_y_acc) obj->y_vel = -props.max_y_acc;

}

static void simobject_update_velocity(simobject_t *obj, fieldproperties_t props)
{

    float dt = props.timestep;

    #if (SIMULATION_CONSTANT_ACCELERATION)
    {
        // dv = int(adt) ... a == constant so... dv = at
        obj->x_vel += props.xvel_constant + ((obj->x_acc * dt) + (obj->intr_x_acc * dt));
        obj->y_vel += props.yvel_constant + ((obj->y_acc * dt) + (obj->intr_y_acc * dt));
    }
    #endif

    // bounding
    if (obj->x_vel >  props.max_x_vel) obj->x_vel =  props.max_x_vel;
    if (obj->x_vel < -props.max_x_vel) obj->x_vel = -props.max_x_vel;
    if (obj->y_vel >  props.max_y_vel) obj->y_vel =  props.max_y_vel;
    if (obj->y_vel < -props.max_y_vel) obj->y_vel = -props.max_y_vel;

}

static void simobject_update_momentum(simobject_t *obj, fieldproperties_t props)
{

    float dt = props.timestep;

    #if (SIMULATION_CONSTANT_ACCELERATION)
    {
        // p = m||v||
        obj->momentum = obj->mass * sqrt( (obj->x_vel * obj->x_vel) + (obj->y_vel * obj->y_vel) );
    }
    #endif

}

static void simobject_update_position(simobject_t *obj, fieldproperties_t props)
{

    float dt = props.timestep;

    #if (SIMULATION_CONSTANT_ACCELERATION)
    {
        // dx = int(vdt)
        obj->x_pos += (obj->x_vel * dt) + ( 0.5f * (obj->x_acc * dt) );
        obj->y_pos += (obj->y_vel * dt) + ( 0.5f * (obj->y_acc * dt) );
    }
    #endif

    // bounding
    if (obj->x_pos >  props.max_x_pos) obj->x_pos =  props.max_x_pos;
    if (obj->x_pos < -props.max_x_pos) obj->x_pos = -props.max_x_pos;
    if (obj->y_pos >  props.max_y_pos) obj->y_pos =  props.max_y_pos;
    if (obj->y_pos < -props.max_y_pos) obj->y_pos = -props.max_y_pos;

}


