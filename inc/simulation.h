/*
 *  simulation.h
 *
 *  Created on: Jun 8, 2022
 *      Author: Dylan
 */

#ifndef _INC_SIMULATION_H
#define _INC_SIMULATION_H

/* ---------------------------------------------------------------------------------------- */

#define SDL_ERRMSG_SIZE (150)

#define WINDOW_WIDTH 1152
#define WINDOW_HEIGHT 648

#define SIMULATION_FPS 240
#define SIMULATION_NUM_OBJECTS 3
#define SIMULATION_CONSTANT_ACCELERATION 1
#define SIMULATION_PERFECTLY_ELASTIC 1

/* ---------------------------------------------------------------------------------------- */

#include "SDL2/SDL.h"
#include "simobject.h"
#include "userinteractions.h"
#include "common.h"

/* ---------------------------------------------------------------------------------------- */

typedef struct simproperties_t
{
    bool                running;                                // simulation on/off
    uint8_t             fps;                                    // how many times the simulation is updated per second

    int32_t             windowHeight;                           // the window's height in screen coordinates
    int32_t             windowLength;                           // the window's length in screen coordinates
    int32_t             windowPos_x;                            // the window's x position in screen coordinates
    int32_t             windowPos_y;                            // the window's y position in screen coordinates

    SDL_FRect           background;                             // rectangle containing the background's information
    SDL_FRect           border;                                 // rectangle containing the border's information

} simproperties_t;

typedef struct sdlstructures_t
{

    SDL_Renderer        *renderer;                              // SDL renderer the simulation is using
    SDL_Window          *window;                                // SDL window the simulation is using
    SDL_Texture         *texture;                               // SDL texture that is updated and displayed each frame

} sdlstructures_t;

typedef struct simulation_t
{
    
    sdlstructures_t     *sdl;                                   // SDL objects used by the simluation
    simproperties_t     *properties;                            // simulation properties
    userinteractions_t  *userinteractions;                      // structure of possible user interactions
    fieldproperties_t   *fieldproperties;                       // physics field properties
    simobject_t         **objects;                              // array of (pointers to) objects in the simulation

} simulation_t;

/* ---------------------------------------------------------------------------------------- */

void simulation_init(simulation_t *sim);
void simulation_start(simulation_t *sim);
void simulation_kill(simulation_t *sim);

#endif