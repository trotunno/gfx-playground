/*
 * simulation.c
 *
 *  Created on: Jun 8, 2022
 *      Author: Dylan
 */

/* ---------------------------------------------------------------------------------------- */

#include <SDL2/SDL.h>
#include "../inc/eventhandler.h"
#include "../inc/simobject.h"
#include "../inc/simulation.h"

/* ---------------------------------------------------------------------------------------- */

static void sdl_reportError(void);

/* ---------------------------------------------------------------------------------------- */

void simulation_init(simulation_t *sim)
{

    sim->running = true;
    sim->createObject = &createObject;
    sim->destroyObject = &destroyObject;

    sim->userinteractions.space_pressed = false;
    sim->userinteractions.escape_pressed = false;

    // initialize the library
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        sdl_reportError();
    }

    // create & configure the window
    sim->window = SDL_CreateWindow("Hello World!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
    if (!sim->window)
    {
        sdl_reportError();
    }

    // create & configure the renderer
    sim->renderer = SDL_CreateRenderer(sim->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
    if (!sim->renderer)
    {
        sdl_reportError();
    }

}

// starts the main while loop
void simulation_start(simulation_t *sim)
{

    SDL_Event event;
    while(sim->running)
    {
        
        // check for events first before updating the simulation's state
        while (SDL_PollEvent(&event))
        {
            switch(event.type)
            {
              
                case SDL_QUIT:
                    evt_sdl_quit_handler(&event, sim);
                    break;
                
                case SDL_KEYDOWN:
                    evt_sdl_keydown_handler(&event, sim);
                    break;

                case SDL_KEYUP:
                    evt_sdl_keyup_handler(&event, sim);
                    break;
                
                default:
                    break;
            }
        }
    
        // wipe the screen
        SDL_SetRenderDrawColor(sim->renderer, 0, 0, 0, 255);
        SDL_RenderClear(sim->renderer);

        // calculate + update the position of each object in the simulation


        // update the texture via blitting


        // wait for 1 frame before looping again so we can achieve 60 FPS
        SDL_Delay(1000/sim->FPS);

    }
    
    // kill the SDL process
    simulation_kill(sim);

}

// destroys all SDL objects and quits
void simulation_kill(simulation_t *sim)
{
    if (sim->window)   SDL_DestroyWindow(sim->window);
    if (sim->renderer) SDL_DestroyRenderer(sim->renderer);

    size_t size = sizeof(sim->simobjects) / sizeof(simobject_t);
    for (int i = 0; i < size; i++)
    {
        sim->destroyObject(&sim->simobjects[i]);
    }
    free(sim);

    SDL_Quit();
}

/* ---------------------------------------------------------------------------------------- */

// wrapper function for reporting SDL-related errors to the terminal
static void sdl_reportError()
{
    char errmsg[SDL_ERRMSG_SIZE];

    SDL_GetErrorMsg(errmsg, SDL_ERRMSG_SIZE);
    printf("%s\n", errmsg);
}