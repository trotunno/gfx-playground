/*
 * simulation.h
 *
 *  Created on: Jun 8, 2022
 *      Author: Dylan
 */

#ifndef _INC_SIMULATION_H
#define _INC_SIMULATION_H

/* ---------------------------------------------------------------------------------------- */

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define DIMENSIONS 2

/* ---------------------------------------------------------------------------------------- */

#include <SDL2/SDL.h>
#include "simobject.h"
#include "userinteractions.h"
#include "common.h"

/* ---------------------------------------------------------------------------------------- */

#define SDL_ERRMSG_SIZE (150)

/* ---------------------------------------------------------------------------------------- */

// manages the simulation as a whole
typedef struct simulation_t
{
    
    bool                running;                              // main loop on/off
    
    SDL_Renderer        *renderer;                            // SDL renderer the simulation is using
    SDL_Window          *window;                              // SDL window the simulation is using

    uint8_t             FPS;                                  // how many times the simulation is updated per second

    simobject_t         simobjects[1];                       // array of objects in the simulation
    userinteractions_t  userinteractions;                     // structure of possible user interactions

    void                (*destroyObject)(simobject_t *);      // function for removing simulation objects
    simobject_t*        (*createObject)(simobject_t);         // function for adding simulation objects

} simulation_t;

/* ---------------------------------------------------------------------------------------- */

void simulation_init(simulation_t *sim);
void simulation_start(simulation_t *sim);
void simulation_kill(simulation_t *sim);

#endif