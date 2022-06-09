/*
 *  simobject.c
 *
 *  Created on: Jun 8, 2022
 *      Author: Dylan
 */

/* ---------------------------------------------------------------------------------------- */

#include "../inc/common.h"
#include "../inc/simobject.h"

/* ---------------------------------------------------------------------------------------- */

simobject_t *createObject(float x_pos, float y_pos, float x_vel, float y_vel, float x_acc, float y_acc)
{

    simobject_t *obj = malloc(sizeof(simobject_t));

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