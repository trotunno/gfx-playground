/*
 * gfx.h
 *
 *  Created on: Jun 4, 2022
 *      Author: Dylan
 */

#ifndef _INC_GFX_H
#define _INC_GFX_H

/* ---------------------------------------------------------------------------------------- */

#include <SDL2/SDL.h>
#include "common.h"

/* ---------------------------------------------------------------------------------------- */

#define ERRMSG_SIZE (150)

/* ---------------------------------------------------------------------------------------- */

// manages an object in the simulation
typedef struct simobject_t
{

    SDL_Rect rectangle;             // object's shape

    float x_pos;                    // object's x position
    float y_pos;                    // object's y position
    float x_vel;                    // object's x velocity
    float y_vel;                    // object's y velocity
    float x_accel;                  // object's x velocity
    float y_accel;                  // object's y acceleration

} simobject_t;

// manages user interactions
typedef struct userinteractions_t
{
    
    bool can_jump;
    bool jump_pressed;
    bool leftarrow_pressed;         // left arrow key pushed state
    bool rightarrow_pressed;        // right arrow key pushed state
    bool uparrow_pressed;           // up arrow key pushed state
    bool downarrow_pressed;         // down arrow key pushed state

} userinteractions_t;

// manages the simulation
typedef struct simulation_t
{
    
    bool running;                                      // main loop on/off
    simobject_t *simobjects[1];                        // array of objects in the simulation
    userinteractions_t userinteractions;               // structure of possible user interactions

    void         (*destroyObject)(simobject_t *);      // function for removing simulation objects
    simobject_t* (*createObject)(simobject_t);         // function for adding simulation objects
    
} simulation_t;

/* ---------------------------------------------------------------------------------------- */

void simulation_init(simulation_t *sim);
void simulation_start(simulation_t *sim);
void simulation_kill(void);

#endif