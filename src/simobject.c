/*
 *  simobject.c
 *
 *  Created on: Jun 8, 2022
 *      Author: Dylan
 */

/* ---------------------------------------------------------------------------------------- */

#include <float.h>

#include "../inc/common.h"
#include "../inc/simulation.h"
#include "../inc/simobject.h"

/* ---------------------------------------------------------------------------------------- */

static void simobject_update_acceleration(simobject_t *obj, fieldproperties_t props);
static void simobject_update_velocity(simobject_t *obj, fieldproperties_t props);
static void simobject_update_position(simobject_t *obj, fieldproperties_t props);

/* ---------------------------------------------------------------------------------------- */

simobject_t * createObject(float mass, float x_pos, float y_pos, float x_vel, float y_vel, float x_acc, float y_acc)
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
    simobject_update_position(obj, props);

}

/* ---------------------------------------------------------------------------------------- */

static void simobject_update_acceleration(simobject_t *obj, fieldproperties_t props)
{

    #if (SIMULATION_CONSTANT_ACCELRATION)
    {
        obj->x_acc = props.xacc_constant;
        obj->y_acc = props.yacc_constant;
    }
    #endif

}

static void simobject_update_velocity(simobject_t *obj, fieldproperties_t props)
{

    float dt = props.timestep;

    #if (SIMULATION_CONSTANT_ACCELRATION)
    {
        // dv = int(a*dt) ... a == constant so... dv = at
        obj->x_vel += props.xvel_constant + (obj->x_acc * dt);
        obj->y_vel += props.yvel_constant + (obj->y_acc * dt);
    }
    #endif

}

static void simobject_update_position(simobject_t *obj, fieldproperties_t props)
{

    float dt = props.timestep;

    #if (SIMULATION_CONSTANT_ACCELRATION)
    {
        // dx = int(v*dt)
        obj->x_pos += (obj->x_vel * dt) + ( 0.5f * (obj->x_acc * dt) );
        obj->y_pos += (obj->y_vel * dt) + ( 0.5f * (obj->y_acc * dt) );
    }
    #endif

}


