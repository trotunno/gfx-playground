/*
 *  simulation.c
 *
 *  Created on: Jun 8, 2022
 *      Author: Dylan
 */

/* ---------------------------------------------------------------------------------------- */

#define TEXTURE_SCALING_FACTOR    (float)(0.80)

/* ---------------------------------------------------------------------------------------- */

#include <float.h>

#include "../inc/SDL2/SDL.h"
#include "../inc/SDL2/SDL_render.h"
#include "../inc/SDL2/SDL_surface.h"
#include "../inc/SDL2/SDL_video.h"

#include "../inc/common.h"
#include "../inc/eventhandler.h"
#include "../inc/simobject.h"
#include "../inc/simulation.h"

/* ---------------------------------------------------------------------------------------- */

static void simulation_render_objects(simulation_t *sim);
static void simulation_update_object_states(simulation_t *sim);
static void simulation_init_background(simulation_t *sim);
static void simulation_init_border(simulation_t *sim);

static void sdl_initialize(simulation_t *sim);
static void sdl_process_events(simulation_t *sim);
static void sdl_redraw_background(simulation_t *sim);
static void sdl_redraw_border(simulation_t *sim);
static void sdl_report_error(void);

/* ---------------------------------------------------------------------------------------- */

void simulation_init(simulation_t *sim)
{

    // allocate memory for all simulation structures
    sim->sdl              = malloc(sizeof(sdlstructures_t));
    sim->properties       = malloc(sizeof(simproperties_t));
    sim->userinteractions = malloc(sizeof(simproperties_t));
    sim->fieldproperties  = malloc(sizeof(fieldproperties_t));
    sim->objects          = malloc(sizeof(simobject_t) * SIMULATION_NUM_OBJECTS);

    // set up SDL2
    sdl_initialize(sim);

    // set simulation properties
    SDL_GetWindowSize(sim->sdl->window, &sim->properties->windowLength, &sim->properties->windowHeight);
    SDL_GetWindowPosition(sim->sdl->window, &sim->properties->windowPos_x, &sim->properties->windowPos_y);

    printf("window length: %d\nwindow height: %d\n", sim->properties->windowLength, sim->properties->windowHeight);
    printf("window pos_x: %d\nwindow pos_y: %d\n", sim->properties->windowPos_x, sim->properties->windowPos_y);

    sim->properties->fps = SIMULATION_FPS;
    sim->properties->running = true;

    // set user interaction default states
    sim->userinteractions->space_pressed = false;
    sim->userinteractions->escape_pressed = false;

    // set field properties
    sim->fieldproperties->timestep = 1.0f;
    
    sim->fieldproperties->xvel_constant =  1.0f;
    sim->fieldproperties->yvel_constant =  0.0f;
    sim->fieldproperties->xacc_constant =  0.0f;
    sim->fieldproperties->yacc_constant = -9.81f;

    //^ TODO: this doesn't work how it's s'posed to!!
    sim->fieldproperties->positive_x_boundary = (float)((sim->properties->windowPos_x) + (sim->properties->windowLength * 0.8f));
    sim->fieldproperties->negative_x_boundary = (float)((sim->properties->windowPos_x) + (sim->properties->windowLength * 0.2f));
    sim->fieldproperties->positive_y_boundary = (float)((sim->properties->windowPos_y) + (sim->properties->windowHeight * 0.2f));
    sim->fieldproperties->negative_y_boundary = (float)((sim->properties->windowPos_y) + (sim->properties->windowHeight * 0.8f));

    printf("positive x boundary = %f\n", sim->fieldproperties->positive_x_boundary);
    printf("negative x boundary = %f\n", sim->fieldproperties->negative_x_boundary);
    printf("positive y boundary = %f\n", sim->fieldproperties->positive_y_boundary);
    printf("negative y boundary = %f\n", sim->fieldproperties->negative_y_boundary);

    // initialize the background & border for the simulation
    simulation_init_background(sim);
    simulation_init_border(sim);

    //! add an object to the simulation
    sim->objects[0] = createObject(10, 20, 0, 0, 0, 0, 0);
    sim->objects[1] = createObject(10, -20, 0, 0, 0, 0, 0);
    sim->objects[2] = createObject(10, 0, 0, 0, 0, 0, 0);

}

// starts and maintains the simulation (window, renderer, objects)
void simulation_start(simulation_t *sim)
{

    while(sim->properties->running)
    {

        // process SDL2-related events
        sdl_process_events(sim);

        // update state of each object in the simulation
        simulation_update_object_states(sim);

        // update the render
        simulation_render_objects(sim);

        // wait for 1 frame before looping again so we can achieve 60 FPS
        SDL_Delay(1000/sim->properties->fps);

    }

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

// initializes the background for the simulation
static void simulation_init_background(simulation_t *sim)
{
    
    const SDL_FRect background = { 0, 0, sim->properties->windowLength, sim->properties->windowHeight };

    SDL_SetRenderDrawColor(sim->sdl->renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderDrawRectF(sim->sdl->renderer, &background);
    SDL_RenderFillRectF(sim->sdl->renderer, &background);
    SDL_RenderClear(sim->sdl->renderer);
    SDL_RenderPresent(sim->sdl->renderer);

    sim->properties->background = background;

}

// initializes the simulation's borders within the window
static void simulation_init_border(simulation_t *sim)
{

    float border_anchor_x = sim->fieldproperties->negative_x_boundary;
    float border_anchor_y = sim->fieldproperties->positive_y_boundary;
    float border_length   = sim->fieldproperties->positive_x_boundary - sim->fieldproperties->negative_x_boundary;
    float border_height   = sim->fieldproperties->negative_y_boundary - sim->fieldproperties->positive_y_boundary;

    const SDL_FRect border = { border_anchor_x, border_anchor_y, border_length, border_height };

    SDL_SetRenderDrawColor(sim->sdl->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderDrawRectF(sim->sdl->renderer, &border);
    SDL_RenderPresent(sim->sdl->renderer);

    sim->properties->border = border;

}

// applies field properties to the objects in the simulation
static void simulation_update_object_states(simulation_t *sim)
{

    for(uint32_t i = 0; i < SIMULATION_NUM_OBJECTS; i++)
    {
        simobject_update_state(sim->objects[i], *sim->fieldproperties);
    }

}

// renders each object as a rectangle and draws them to the screen
static void simulation_render_objects(simulation_t *sim)
{

    simobject_t obj;

    uint8_t objectColors[SIMULATION_NUM_OBJECTS][3] = { {0xFF, 0xAA, 0x43}, {0xFF, 0x00, 0x00}, {0x00, 0xFF, 0x00} };

    // retrieve the x and y origins in window space
    float window_x_origin = sim->properties->border.x + (sim->properties->border.w / 2.0f);
    float window_y_origin = sim->properties->border.y + (sim->properties->border.h / 2.0f);

    sdl_redraw_background(sim);
    sdl_redraw_border(sim);

    for (uint32_t i = 0; i < SIMULATION_NUM_OBJECTS; i++)
    {

        obj = *sim->objects[i];                                                  

        // convert object's x-y coordinates to window coordinates
        float window_x_pos = window_x_origin + obj.x_pos;
        float window_y_pos = window_y_origin - obj.y_pos;

        // create rectangle from the object's current state
        SDL_FRect rect = { window_x_pos, window_y_pos, obj.height, obj.width };

        uint8_t r = objectColors[i][0];
        uint8_t g = objectColors[i][1];
        uint8_t b = objectColors[i][2];

        printf("r = 0x%02X, g = 0x%02X, b = 0x%02X\n", r, g, b);

        // set object color
        if (SDL_SetRenderDrawColor(sim->sdl->renderer, r, g, b, 0xFF))         
        {
            sdl_report_error();
        }             
        
        // draw object
        if (SDL_RenderDrawRectF(sim->sdl->renderer, &rect))
        {
            sdl_report_error();
        }
        
        // color fill
        if (SDL_RenderFillRectF(sim->sdl->renderer, &rect))
        {
            sdl_report_error();
        }
        
        // clear rendering target properties
        //if (SDL_RenderClear(sim->sdl->renderer))
        //{
            //sdl_report_error();
        //}

        SDL_RenderPresent(sim->sdl->renderer);

    }

}

/* ---------------------------------------------------------------------------------------- */

// initializes all SDL2-related components of the simulation
static void sdl_initialize(simulation_t *sim)
{

    if (SDL_Init(SDL_INIT_TIMER|SDL_INIT_EVENTS|SDL_INIT_VIDEO) != 0)
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

// loops until there are no events left to handle
static void sdl_process_events(simulation_t *sim)
{
    
    SDL_Event event;

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

}

static void sdl_redraw_background(simulation_t *sim)
{

    SDL_SetRenderDrawColor(sim->sdl->renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderDrawRectF(sim->sdl->renderer, &sim->properties->background);
    SDL_RenderFillRectF(sim->sdl->renderer, &sim->properties->background);
    SDL_RenderClear(sim->sdl->renderer);
    SDL_RenderPresent(sim->sdl->renderer);

}

static void sdl_redraw_border(simulation_t *sim)
{

    SDL_SetRenderDrawColor(sim->sdl->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderDrawRectF(sim->sdl->renderer, &sim->properties->border);
    SDL_RenderPresent(sim->sdl->renderer);

}

static void sdl_report_error()
{

    char errmsg[SDL_ERRMSG_SIZE];

    SDL_GetErrorMsg(errmsg, SDL_ERRMSG_SIZE);
    printf("%s\n", errmsg);
    
}