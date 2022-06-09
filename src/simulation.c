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
#include "../inc/common.h"
#include "../inc/eventhandler.h"
#include "../inc/simobject.h"
#include "../inc/simulation.h"

/* ---------------------------------------------------------------------------------------- */

static void sdl_initialize(simulation_t *sim);
static void sdl_report_error(void);

/* ---------------------------------------------------------------------------------------- */

void simulation_init(simulation_t *sim)
{

    sdl_initialize(sim);

    sim->simobjects = malloc(sizeof(simobject_t) * 1);

    sim->userinteractions.space_pressed = false;
    sim->userinteractions.escape_pressed = false;

    sim->FPS = SIMULATION_FPS;
    sim->running = true;

}

// starts and maintains the simulation (window, renderer, objects)
void simulation_start(simulation_t *sim)
{

    SDL_Event event;
    while(sim->running)
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
    
        // wipe the screen
        SDL_SetRenderDrawColor(sim->renderer, 0, 0, 0, 255);
        SDL_RenderClear(sim->renderer);

        // get updated state of each object in the simulation
        if (sim->userinteractions.space_pressed)
        {
            printf("hello!\n");
        }

        SDL_Rect rect = {10, 10, 800, 600};

        // update the texture via blitting (SDL_RenderCopy seems to be the way?)
        SDL_SetRenderTarget(sim->renderer, sim->texture);
        SDL_SetRenderDrawColor(sim->renderer, 0x00, 0xC0, 0xE0, 0xF0);
        SDL_RenderClear(sim->renderer);
        SDL_RenderDrawRect(sim->renderer, &rect);
        SDL_SetRenderDrawColor(sim->renderer, 0xFF, 0x00, 0x00, 0x00);
        SDL_RenderFillRect(sim->renderer, &rect);
        SDL_SetRenderTarget(sim->renderer, NULL);
        SDL_RenderCopy(sim->renderer, sim->texture, NULL, NULL);
        SDL_RenderPresent(sim->renderer);

        // wait for 1 frame before looping again so we can achieve 60 FPS
        SDL_Delay(1000/sim->FPS);

    }
    
    // kill the SDL process
    simulation_kill(sim);

}

// destroys all SDL objects, frees all dynamically allocated memory, then quits
void simulation_kill(simulation_t *sim)
{

    if (sim->window)   SDL_DestroyWindow(sim->window);
    if (sim->renderer) SDL_DestroyRenderer(sim->renderer);

    free(sim->simobjects);
    free(sim);

    SDL_Quit();

}

/* ---------------------------------------------------------------------------------------- */

static void sdl_initialize(simulation_t *sim)
{

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        sdl_report_error();
    }

    sim->window = SDL_CreateWindow("Hello World!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!sim->window)
    {
        sdl_report_error();
    }

    sim->renderer = SDL_CreateRenderer(sim->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
    if (!sim->renderer)
    {
        sdl_report_error();
    }

    sim->texture = SDL_CreateTexture(sim->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WINDOW_WIDTH * TEXTURE_SCALING_FACTOR, WINDOW_HEIGHT * TEXTURE_SCALING_FACTOR);
    if (!sim->texture)
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