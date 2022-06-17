/*
 *  simulation.c
 *
 *  Created on: Jun 8, 2022
 *      Author: Dylan
 */

/* ---------------------------------------------------------------------------------------- */

#define TEXTURE_SCALING_FACTOR    (float)(0.80)

/* ---------------------------------------------------------------------------------------- */

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include "../inc/common.h"
#include "../inc/eventhandler.h"
#include "../inc/simobject.h"
#include "../inc/simulation.h"

/* ---------------------------------------------------------------------------------------- */

static void simulation_render_objects(simulation_t *sim);
static void simulation_update_object_states(simulation_t *sim);
static void simulation_init_background(simulation_t *sim);

static void sdl_initialize(simulation_t *sim);
static void sdl_report_error(void);

/* ---------------------------------------------------------------------------------------- */

void simulation_init(simulation_t *sim)
{

    // allocate memory for all the simulation structures
    sim->sdl              = malloc(sizeof(sdlstructures_t));
    sim->properties       = malloc(sizeof(simproperties_t));
    sim->userinteractions = malloc(sizeof(simproperties_t));
    sim->fieldproperties  = malloc(sizeof(fieldproperties_t));
    sim->objects          = malloc(sizeof(simobject_t) * SIMULATION_NUM_OBJECTS);

    // set up SDL2
    sdl_initialize(sim);

    // set window size members
    SDL_GetWindowSize(sim->sdl->window, &sim->properties->windowLength, &sim->properties->windowHeight);

    // set field properties
    sim->fieldproperties->timestep = 1.0f;

    sim->fieldproperties->xvel_constant = 1.0f;
    sim->fieldproperties->yvel_constant = 0.0f;

    sim->fieldproperties->xacc_constant = 0.0f;
    sim->fieldproperties->yacc_constant = -9.81f;

    sim->fieldproperties->positive_x_boundary = sim->properties->windowLength /  2.0f;
    sim->fieldproperties->negative_x_boundary = sim->properties->windowLength / -2.0f;
    sim->fieldproperties->positive_y_boundary = sim->properties->windowHeight /  2.0f;
    sim->fieldproperties->negative_x_boundary = sim->properties->windowHeight / -2.0f;

    // set user interaction default states
    sim->userinteractions->space_pressed = false;
    sim->userinteractions->escape_pressed = false;

    // set simulation characteristics
    sim->properties->fps = SIMULATION_FPS;
    sim->properties->running = true;

}

// starts and maintains the simulation (window, renderer, objects)
void simulation_start(simulation_t *sim)
{

    //simulation_init_background(sim);

    SDL_Event event;
    while(sim->properties->running)
    {
        
        // check for SDL-related events first before updating the simulation's state 
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

        // update state of each object in the simulation
        simulation_update_object_states(sim);

        // update the render
        simulation_render_objects(sim);

        // wait for 1 frame before looping again so we can achieve 60 FPS
        SDL_Delay(1000/sim->properties->fps);

    }
    
    // kill the SDL process
    simulation_kill(sim);

}

// destroys all SDL objects, frees all dynamically allocated memory, then quits
void simulation_kill(simulation_t *sim)
{

    if (sim->sdl->window)   SDL_DestroyWindow(sim->sdl->window);
    if (sim->sdl->renderer) SDL_DestroyRenderer(sim->sdl->renderer);
    if (sim->sdl->texture)  SDL_DestroyTexture(sim->sdl->texture);

    free(sim->sdl);
    free(sim->properties);
    free(sim->userinteractions);
    free(sim->fieldproperties);
    free(sim->objects);
    
    free(sim);

    SDL_Quit();

}

/* ---------------------------------------------------------------------------------------- */

static void simulation_init_background(simulation_t *sim)
{
    
    const SDL_Rect background = { 0, 0, sim->properties->windowLength, sim->properties->windowHeight };

    SDL_SetRenderDrawColor(sim->sdl->renderer, 0xFF, 0xAA, 0xCC, 0xA0);                  // set the color
    SDL_RenderFillRect(sim->sdl->renderer, &background);                                 // fill

    SDL_RenderPresent(sim->sdl->renderer);                                               // update screen

}

static void simulation_update_object_states(simulation_t *sim)
{

    for(uint32_t i = 0; i < SIMULATION_NUM_OBJECTS; i++)
    {
        simobject_update_state(&sim->objects[i], *sim->fieldproperties);
    }

}

static void simulation_render_objects(simulation_t *sim)
{

    simobject_t obj;

    SDL_SetRenderDrawColor(sim->sdl->renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(sim->sdl->renderer);

    for (uint32_t i = 0; i < SIMULATION_NUM_OBJECTS; i++)
    {
        obj = sim->objects[i];                                                  

        float window_x_pos = sim->properties->windowHeight - obj.x_pos;                  // convert object's x-y coordinates to window coordinates
        float window_y_pos = sim->properties->windowLength - obj.y_pos;

        SDL_FRect rect = { window_x_pos, window_y_pos, obj.height, obj.width };          // create rectangle from the object's state

        SDL_SetRenderDrawColor(sim->sdl->renderer, 0xFF, 0x00, 0x00, 0xFF);              // set object color
        SDL_RenderDrawRectF(sim->sdl->renderer, &rect);                                  // draw object
        SDL_RenderFillRectF(sim->sdl->renderer, &rect);                                  // color fill)
    }

    SDL_RenderPresent(sim->sdl->renderer);

}

/* ---------------------------------------------------------------------------------------- */

static void sdl_initialize(simulation_t *sim)
{

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        sdl_report_error();
    }

    sim->sdl->window = SDL_CreateWindow("Hello World!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!sim->sdl->window)
    {
        sdl_report_error();
    }

    sim->sdl->renderer = SDL_CreateRenderer(sim->sdl->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
    if (!sim->sdl->renderer)
    {
        sdl_report_error();
    }

    sim->sdl->texture = SDL_CreateTexture(sim->sdl->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WINDOW_WIDTH * TEXTURE_SCALING_FACTOR, WINDOW_HEIGHT * TEXTURE_SCALING_FACTOR);
    if (!sim->sdl->texture)
    {
        sdl_report_error();
    }

}

static void sdl_report_error()
{

    char errmsg[SDL_ERRMSG_SIZE];

    SDL_GetErrorMsg(errmsg, SDL_ERRMSG_SIZE);
    printf("%s\n", errmsg);
    
}