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
#include <stdbool.h>

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

//!
static uint8_t* simulation_check_collisions(simulation_t *sim, simobject_t* obj[SIMULATION_NUM_OBJECTS]);
static void simulation_handle_collisions(simobject_t *obj[SIMULATION_NUM_OBJECTS], uint8_t *collision_matrix_unwrapped, fieldproperties_t props);
//!

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
    sim->fieldproperties->timestep = 0.25f;
    
    sim->fieldproperties->xvel_constant =  0.01f;
    sim->fieldproperties->yvel_constant =  0.0f;
    sim->fieldproperties->xacc_constant =  0.0f;
    sim->fieldproperties->yacc_constant =  0.01f;

    sim->fieldproperties->positive_x_boundary = (float)((sim->properties->windowPos_x) + (sim->properties->windowLength * 0.9f));
    sim->fieldproperties->negative_x_boundary = (float)((sim->properties->windowPos_x) + (sim->properties->windowLength * 0.1f));
    sim->fieldproperties->positive_y_boundary = (float)((sim->properties->windowPos_y) + (sim->properties->windowHeight * 0.1f));
    sim->fieldproperties->negative_y_boundary = (float)((sim->properties->windowPos_y) + (sim->properties->windowHeight * 0.9f));

    //^
    printf("positive x boundary = %f\n", sim->fieldproperties->positive_x_boundary);
    printf("negative x boundary = %f\n", sim->fieldproperties->negative_x_boundary);
    printf("positive y boundary = %f\n", sim->fieldproperties->positive_y_boundary);
    printf("negative y boundary = %f\n", sim->fieldproperties->negative_y_boundary);
    //^

    // initialize the background & border for the simulation
    simulation_init_background(sim);
    simulation_init_border(sim);

    //! add an object to the simulation
    sim->objects[0] = createObject(50, 0, 0, 0, 0, 0, 0, 0.1, 0, 0, 0.1);
    sim->objects[1] = createObject(150, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    sim->objects[2] = createObject(35, 0, 0, 0, 0, 0, 0, -0.5, 0, 0, -0.01);

}

// starts and maintains the simulation (window, renderer, objects)
void simulation_start(simulation_t *sim)
{

    while(sim->properties->running)
    {

        sdl_process_events(sim);                    // process SDL2-related events

        // dead-simple pausing feature
        if (sim->userinteractions->space_pressed == false)
        {
            simulation_update_object_states(sim);       // update state of each object in the simulation

            simulation_render_objects(sim);             // update the render

            SDL_Delay(1000/sim->properties->fps);       // wait for 1 frame before looping again so we can achieve 60 FPS

            // quit out if space is pressed
            if (sim->userinteractions->escape_pressed)
            {
                sim->properties->running = false;
            }
        }

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

//! /* ---------------------------------------------------------------------------------------- */  //!

// detects which objects have interecting locations ( naive O(N^2) )
static uint8_t* simulation_check_collisions(simulation_t *sim, simobject_t *obj[SIMULATION_NUM_OBJECTS])
{

    //* TODO: something weird happening with the first object's collision detection

    uint8_t collision_matrix[SIMULATION_NUM_OBJECTS][SIMULATION_NUM_OBJECTS] = {0};
    uint8_t *collision_matrix_unwrapped;
    uint8_t i, j;
    uint8_t nrows, ncols;
    simobject_t obj1, obj2;
    SDL_FRect rect1, rect2, border, sect;                                               // assuming the objects are all rectangles for now
    float window_x_origin, window_y_origin;

    // retrieve the objects x and y origins in window space
    window_x_origin = sim->properties->border.x + (sim->properties->border.w / 2.0f);
    window_y_origin = sim->properties->border.y + (sim->properties->border.h / 2.0f);

    nrows  = SIMULATION_NUM_OBJECTS;
    ncols  = SIMULATION_NUM_OBJECTS;
    border = sim->properties->border;

    // malloc the return array and set to 0
    collision_matrix_unwrapped = malloc(nrows * ncols);
    for (i = 0; i < nrows; i++) collision_matrix_unwrapped[i] = 0;

    // detect collisions with border
    for (i = 0; i < nrows; i++)
    {
        obj1 = *obj[i];

        // convert object's x-y coordinates to window coordinates
        rect1.x = window_x_origin + obj1.x_pos;
        rect1.y = window_y_origin - obj1.y_pos; 
        rect1.w = obj1.width;
        rect1.h = obj1.height;

        if (SDL_IntersectFRect(&rect1, &border, &sect))
        {
            // if the intersection of the object and the border is not equal to the object (the object is intersecting or outside the border)
            if (rect1.h != sect.h || rect1.w != sect.w || rect1.x != sect.x || rect1.y != sect.y)
            {
               collision_matrix[i][i] = 1;              // no this isn't a typo
            }
            else
            {
                collision_matrix[i][i] = 0;
            }
        }

    }

    // detect collisions with other objects
    for (i = 0; i < nrows; i++)
    {

        obj1 = *obj[i];

        // convert object's x-y coordinates to window coordinates
        rect1.x = window_x_origin + obj1.x_pos;
        rect1.y = window_y_origin - obj1.y_pos; 
        rect1.w = obj1.width;
        rect1.h = obj1.height;

        for (j = 0; j < ncols; j++)
        {

            obj2 = *obj[j];

            // convert object's x-y coordinates to window coordinates
            rect2.x = window_x_origin + obj2.x_pos;
            rect2.y = window_y_origin - obj2.y_pos; 
            rect2.w = obj2.width;
            rect2.h = obj2.height;

            // if the rectangles intersect, mark the collision in the matrix
            if (SDL_IntersectFRect(&rect1, &rect2, &sect))
            {
                collision_matrix[i][j] = 1;
            }

        }

    }

    // unwrap array into 1D (index = indexX * arrayWidth + indexY)
    for (i = 0; i < nrows; i++)
    {
        for (j = 0; j < ncols; j++)
        {
            collision_matrix_unwrapped[i * ncols * j] = collision_matrix[i][j];
        }
    }

    return collision_matrix_unwrapped;

}

// marks objects with what objects they are colliding with
static void simulation_handle_collisions(simobject_t *obj[SIMULATION_NUM_OBJECTS], uint8_t *collision_matrix_unwrapped, fieldproperties_t props)
{

    uint8_t i, j;
    uint8_t collision_matrix[SIMULATION_NUM_OBJECTS][SIMULATION_NUM_OBJECTS] = {0};
    uint8_t nrows, ncols;

    nrows = SIMULATION_NUM_OBJECTS;
    ncols = SIMULATION_NUM_OBJECTS;

    // unwrap collision matrix, free the previously-malloc'd memory
    for (i = 0; i < nrows; i++)
    {
        for (j = 0; j < ncols; j++)
        {
            collision_matrix[i][j] = collision_matrix_unwrapped[i * ncols * j];
        }
    }

    // every collision is just a simple harmonic oscillator... sshhh!
    printf("----------\n");
    for (i = 0; i < nrows; i++)
    {
        printf("|");
        for (j = 0; j < ncols; j++)
        {
            printf("%d|", collision_matrix[i][j]);
        }
        printf("\n");
    }
    printf("----------\n");

    free(collision_matrix_unwrapped);
    
}

//! /* ---------------------------------------------------------------------------------------- */  //!

// initializes the background for the simulation
static void simulation_init_background(simulation_t *sim)
{
    
    const SDL_FRect background = { 0, 0, sim->properties->windowLength, sim->properties->windowHeight };

    SDL_SetRenderDrawColor(sim->sdl->renderer, 0x00, 0x00, 0x00, 0xFF);
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

    uint8_t *cm_unwrapped;

    cm_unwrapped = simulation_check_collisions(sim, sim->objects);
    simulation_handle_collisions(sim->objects, cm_unwrapped, *sim->fieldproperties);

    for(uint32_t i = 0; i < SIMULATION_NUM_OBJECTS; i++)
    {
        simobject_update_state(sim->objects[i], *sim->fieldproperties);
    }

}

// renders each object as a rectangle and draws them to the screen
static void simulation_render_objects(simulation_t *sim)
{

    simobject_t obj;

    uint8_t objectColors[SIMULATION_NUM_OBJECTS][3] = { {0xFF, 0x00, 0x00}, {0x00, 0xFF, 0x00}, {0x00, 0x00, 0xFF} };

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

    }

    SDL_RenderPresent(sim->sdl->renderer);

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

    SDL_SetRenderDrawColor(sim->sdl->renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderDrawRectF(sim->sdl->renderer, &sim->properties->background);
    SDL_RenderFillRectF(sim->sdl->renderer, &sim->properties->background);

}

static void sdl_redraw_border(simulation_t *sim)
{

    SDL_SetRenderDrawColor(sim->sdl->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderDrawRectF(sim->sdl->renderer, &sim->properties->border);

}

static void sdl_report_error()
{

    char errmsg[SDL_ERRMSG_SIZE];

    SDL_GetErrorMsg(errmsg, SDL_ERRMSG_SIZE);
    printf("%s\n", errmsg);
    
}