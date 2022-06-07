/*
 * gfx.c
 *
 *  Created on: Jun 4, 2022
 *      Author: Dylan
 */

#include <SDL2/SDL.h>
#include <string.h>
#include "../inc/gfx.h"
#include "../inc/eventhandler.h"

/* ---------------------------------------------------------------------------------------- */

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define SIZE 200
#define SPEED 600
#define GRAVITY 60
#define JUMP -1200
#define FPS 60

/* ---------------------------------------------------------------------------------------- */

static void sdl_reportAndDestroy(void);

/* ---------------------------------------------------------------------------------------- */

SDL_Window *window;
SDL_Renderer *renderer;

/* ---------------------------------------------------------------------------------------- */

simobject_t *createObject(simobject_t obj)
{

    SDL_Rect rect = { WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_WIDTH/2, WINDOW_HEIGHT/2 };

    obj.rectangle = rect;

    return malloc(sizeof(obj));
}

void destroyObject(simobject_t *obj)
{
    free(obj);
}

void simulation_init(simulation_t *sim)
{

    sim->running = true;
    sim->createObject = createObject;
    sim->destroyObject = destroyObject;

    sim->userinteractions.can_jump = false;
    sim->userinteractions.jump_pressed = false;
    sim->userinteractions.uparrow_pressed = false;
    sim->userinteractions.downarrow_pressed = false;
    sim->userinteractions.leftarrow_pressed = false;
    sim->userinteractions.rightarrow_pressed = false;

    simobject_t rectangle;
    rectangle.x_pos = (WINDOW_WIDTH - SIZE) / 2;
    rectangle.y_pos = (WINDOW_HEIGHT - SIZE) / 2;
    rectangle.x_vel = 0;
    rectangle.y_vel = 0;

    sim->simobjects[0] = sim->createObject(rectangle);

    // initialize the library
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        sdl_reportAndDestroy();
    }

    // create & configure the window
    window = SDL_CreateWindow("Hello World!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        sdl_reportAndDestroy();
    }

    // create & configure the renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
    if (!renderer)
    {
        sdl_reportAndDestroy();
    }

}

// starts the main while loop
void simulation_start(simulation_t *sim)
{

    SDL_Rect rectangle = sim->simobjects[0]->rectangle;
    
    // ---- Main Loop ---- //

    SDL_Event event;
    while(sim->running)
    {
        
        // check for events
        while (SDL_PollEvent(&event))
        {
            switch(event.type)
            {
              
                case SDL_QUIT:
                    sim->running = false;
                    break;
                
                case SDL_KEYDOWN:
                    evt_sdl_keydown_handler(&event, sim);
                    break;

                case SDL_KEYUP:
                    switch (event.key.keysym.scancode)
                    {
                        case SDL_SCANCODE_UP:
                        case SDL_SCANCODE_SPACE:
                            sim->userinteractions.jump_pressed = false;
                            break;

                        case SDL_SCANCODE_A:
                        case SDL_SCANCODE_LEFT:
                            sim->userinteractions.leftarrow_pressed = false;
                            break;
                        
                        case SDL_SCANCODE_D:
                        case SDL_SCANCODE_RIGHT:
                            sim->userinteractions.rightarrow_pressed = false;
                            break;

                        default:
                            break;
                    }
                    break;
                
                default:
                    break;
            }
        }
    
        // wipe the screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // update the rectangle's properties
        sim->simobjects[0]->x_vel = (sim->userinteractions.rightarrow_pressed - sim->userinteractions.leftarrow_pressed)*SPEED;
        sim->simobjects[0]->y_vel += GRAVITY;

        if (sim->userinteractions.jump_pressed && sim->userinteractions.can_jump)
        {
            sim->userinteractions.can_jump = false;
            sim->simobjects[0]->y_vel = JUMP;
        }

        sim->simobjects[0]->x_pos += (sim->simobjects[0]->x_vel / 60);
        sim->simobjects[0]->y_pos += (sim->simobjects[0]->y_vel / 60);
    
        if (sim->simobjects[0]->x_pos <= 0)
            sim->simobjects[0]->x_pos = 0;

        if (sim->simobjects[0]->x_pos >= WINDOW_WIDTH - rectangle.w)
            sim->simobjects[0]->x_pos = WINDOW_WIDTH - rectangle.w;

        if (sim->simobjects[0]->y_pos <= 0)
            sim->simobjects[0]->y_pos = 0;

        if (sim->simobjects[0]->y_pos >= WINDOW_HEIGHT - rectangle.h)
        {
            sim->simobjects[0]->y_vel = 0;
            sim->simobjects[0]->y_pos = WINDOW_HEIGHT - rectangle.h;

            if (!sim->userinteractions.jump_pressed)
                sim->userinteractions.can_jump = true;
        }

        // update the rendering object(s)
        rectangle.x = (int)sim->simobjects[0]->x_pos;
        rectangle.y = (int)sim->simobjects[0]->y_pos;

        // draw the rendering object(s)
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &rectangle);

        // update the window, wait for 1 frame before looping again
        SDL_RenderPresent(renderer);
        SDL_Delay(1000/FPS);

    }
    
    // kill the SDL process
    simulation_kill();

}

// destroys all SDL objects and quits
void simulation_kill()
{
    if (window)   SDL_DestroyWindow(window);
    if (renderer) SDL_DestroyRenderer(renderer);

    SDL_Quit();
}

/* ---------------------------------------------------------------------------------------- */

// wrapper function for reporting SDL-related errors to the terminal
static void sdl_reportAndDestroy()
{
    char errmsg[ERRMSG_SIZE];

    SDL_GetErrorMsg(errmsg, ERRMSG_SIZE);
    printf("%s\n", errmsg);

    simulation_kill();
}