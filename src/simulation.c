/*
 *  simulation.c
 *
 *  Created on: Jun 8, 2022
 *      Author: Dylan
 */

/* ---------------------------------------------------------------------------------------- */

#define TEXTURE_SCALING_FACTOR    (float)(0.80)

/* ---------------------------------------------------------------------------------------- */

#include <time.h>
#include <float.h>
#include <stdbool.h>

#include "../inc/SDL2/SDL.h"
#include "../inc/SDL2/SDL_render.h"
#include "../inc/SDL2/SDL_surface.h"
#include "../inc/SDL2/SDL_video.h"

#include "../inc/common.h"
#include "../inc/shapes.h"
#include "../inc/eventhandler.h"
#include "../inc/simobject.h"
#include "../inc/simulation.h"
#include "../inc/collisions.h"

/* ---------------------------------------------------------------------------------------- */

static void simulation_add_objects(simulation_t *sim);
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

    //^
    printf("window length: %d\nwindow height: %d\n", sim->properties->windowLength, sim->properties->windowHeight);
    printf("window pos_x: %d\nwindow pos_y: %d\n", sim->properties->windowPos_x, sim->properties->windowPos_y);
    //^

    sim->properties->fps = SIMULATION_FPS;
    sim->properties->running = true;

    // set user interaction default states
    sim->userinteractions->space_pressed = false;
    sim->userinteractions->escape_pressed = false;

    // set field properties
    sim->fieldproperties->timestep = 0.12;
    
    sim->fieldproperties->xvel_constant =  0.0f;
    sim->fieldproperties->yvel_constant =  0.0f;
    sim->fieldproperties->xacc_constant =  0.0f;
    sim->fieldproperties->yacc_constant =  0.5f;

    sim->fieldproperties->positive_x_boundary = (float)((sim->properties->windowPos_x) + (sim->properties->windowLength * 0.82f));
    sim->fieldproperties->negative_x_boundary = (float)((sim->properties->windowPos_x) + (sim->properties->windowLength * 0.18f));
    sim->fieldproperties->positive_y_boundary = (float)((sim->properties->windowPos_y) + (sim->properties->windowHeight * 0.18f));
    sim->fieldproperties->negative_y_boundary = (float)((sim->properties->windowPos_y) + (sim->properties->windowHeight * 0.82f));

    sim->fieldproperties->max_x_pos = 10000.0f;
    sim->fieldproperties->max_y_pos = 10000.0f;
    sim->fieldproperties->max_x_vel = 20.0f;
    sim->fieldproperties->max_y_vel = 20.0f;
    sim->fieldproperties->max_x_acc = 100.0f;
    sim->fieldproperties->max_y_acc = 100.0f;

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
    simulation_add_objects(sim);

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

    uint16_t i = 0;

    if (sim->sdl->window)   SDL_DestroyWindow(sim->sdl->window);
    if (sim->sdl->renderer) SDL_DestroyRenderer(sim->sdl->renderer);
    if (sim->sdl->texture)  SDL_DestroyTexture(sim->sdl->texture);

    free(sim->sdl);
    free(sim->properties);
    free(sim->userinteractions);
    free(sim->fieldproperties);
    
    for (i = 0; i < SIMULATION_NUM_OBJECTS; i++)
    {
        free(sim->objects[i]);
    }
    
    free(sim);

    SDL_Quit();

}

//! /* ---------------------------------------------------------------------------------------- */  //!

static void simulation_add_objects(simulation_t *sim)
{

    float default_mass = 50;

    float min_x_spawn = sim->fieldproperties->negative_x_boundary + 50;
    float max_x_spawn = sim->fieldproperties->positive_x_boundary - 50;
    float min_y_spawn = sim->fieldproperties->negative_y_boundary + 50;
    float max_y_spawn = sim->fieldproperties->positive_y_boundary - 50;

    float x = min_x_spawn;
    float y = min_y_spawn;

/*
    for (uint8_t i = 0; i < SIMULATION_NUM_OBJECTS; i++)
    {
        sim->objects[i] = createObject
        (
            default_mass,                           // mass
            x,                                      // x pos initial
            y,                                      // y pos initial
            0,                                      // x vel initial
            0,                                      // y vel initial
            0,                                      // x acc initial
            0,                                      // y acc initial
            (rand() % 2) - 1,                       // x vel intrinsic
            (rand() % 2) - 1,                       // y vel intrinsic
            0,                                      // x acc intrinsic
            0                                       // y acc intrinsic
        );

        // make sure they don't overlap (not robust at all)  
        if (x + ( (default_mass/2) + 2 ) >= max_x_spawn)
        {
            x = min_x_spawn;
            y = min_y_spawn + ( (default_mass/2) + 2 );
        }
        else
        {
            x += ( (default_mass/2) + 2 );
            y += ( (default_mass/2) + 2 );
        }
        
    }
*/

    // * there is an issue with the "intrinsic velocity" that is causing simulation border problems

    sim->objects[0] = createObject(30, 300, -100, 0, 0, 0, 0, 0, 0, 0, 0);
    
    /*
    sim->objects[1] = createObject(24, -100, -33, 0, 0, 0, 0, 0, 0, 0, 0);
    sim->objects[2] = createObject(30, -150, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    sim->objects[3] = createObject(40, 200, 120, 0, 0, 0, 0, 0, 0, 0, 0);
    sim->objects[4] = createObject(32, -210, -120, 0, 0, 0, 0, 0, 0, 0, 0);
    
    sim->objects[5] = createObject(35, -84, 30, 0, 0, 0, 0, 0, 0, 0, 0);
    sim->objects[6] = createObject(38, 200, -183, 0, 0, 0, 0, 0, 0, 0, 0);
    sim->objects[7] = createObject(39, -215, -33, 0, 0, 0, 0, 0, 0, 0, 0);
    sim->objects[8] = createObject(29, -120, 121, 0, 0, 0, 0, 0, 0, 0, 0);
    sim->objects[9] = createObject(28, 200, 84, 235, 0, 0, 0, 0, 0, 0, 0);
    
    sim->objects[10] = createObject(30, -rand() % 200, -rand() % 200, 0, 0, 0, 0, 0, 0, 0, 0);
    sim->objects[11] = createObject(24, -rand() % 200, -rand() % 200, 0, 0, 0, 0, 0, 0, 0, 0);
    sim->objects[12] = createObject(30, -rand() % 200,  rand() % 200, 0, 0, 0, 0, 0, 0, 0, 0);
    sim->objects[13] = createObject(40,  rand() % 200, -rand() % 200, 0, 0, 0, 0, 0, 0, 0, 0);
    sim->objects[14] = createObject(32,  rand() % 200, -rand() % 200, 0, 0, 0, 0, 0, 0, 0, 0);

    sim->objects[15] = createObject(35,  rand() % 200,  rand() % 200, 0, 0, 0, 0, 0, 0, 0, 0);
    sim->objects[16] = createObject(38, -rand() % 200, -rand() % 200, 0, 0, 0, 0, 0, 0, 0, 0);
    sim->objects[17] = createObject(39, -rand() % 200,  rand() % 200, 0, 0, 0, 0, 0, 0, 0, 0);
    sim->objects[18] = createObject(29,  rand() % 200, -rand() % 200, 0, 0, 0, 0, 0, 0, 0, 0);
    sim->objects[19] = createObject(28, -rand() % 200,  rand() % 200, 0, 0, 0, 0, 0, 0, 0, 0);
    */

}

// detects which objects have interecting locations (O(N^2)). Treats objects as rectangles
static uint8_t* simulation_check_collisions(simulation_t *sim, simobject_t* obj[SIMULATION_NUM_OBJECTS])
{

    uint8_t collision_matrix[SIMULATION_NUM_OBJECTS][SIMULATION_NUM_OBJECTS] = {0};
    uint8_t *collision_matrix_unwrapped;
    uint16_t row_index, col_index;
    uint16_t i, j, k;
    uint16_t nrows, ncols;
    simobject_t obj1, obj2;
    SDL_FRect rect1, rect2, border;
    float window_x_origin, window_y_origin;

    nrows  = SIMULATION_NUM_OBJECTS;
    ncols  = SIMULATION_NUM_OBJECTS;
    border = sim->properties->border;

    // retrieve the objects x and y origins in window space
    window_x_origin = sim->properties->border.x + (sim->properties->border.w / 2.0f);
    window_y_origin = sim->properties->border.y + (sim->properties->border.h / 2.0f);

    // malloc the return array and set to 0
    collision_matrix_unwrapped = malloc(nrows * ncols);
    for (i = 0; i < nrows; i++) collision_matrix_unwrapped[i] = 0;

    // detect collisions with border
    for (i = 0; i < nrows; i++)
    {

        obj1 = *obj[i];

        // convert object's x-y coordinates to window coordinates
        rect1.x = window_x_origin + obj1.x_pos;
        rect1.y = window_y_origin + obj1.y_pos; 
        rect1.w = obj1.width;
        rect1.h = obj1.height;

        //^
        //printf("rect1: (%f) (%f)\n", rect1.x, rect1.y);
        //^

        collision_matrix[i][i] = detect_border_collision(rect1, border);

    }

    // detect collisions with other objects
    for (i = 0; i < nrows; i++)
    {

        obj1 = *obj[i];

        // convert object's x-y coordinates to window coordinates
        rect1.x = window_x_origin + obj1.x_pos;
        rect1.y = window_y_origin + obj1.y_pos; 
        rect1.w = obj1.width;
        rect1.h = obj1.height;

        for (j = 0; j < ncols; j++)
        {

            if (i == j) continue;       // skip the border collision case

            obj2 = *obj[j];

            // convert object's x-y coordinates to window coordinates
            rect2.x = window_x_origin + obj2.x_pos;
            rect2.y = window_y_origin + obj2.y_pos; 
            rect2.w = obj2.width;
            rect2.h = obj2.height;

            collision_matrix[i][j] = detect_object_collision(rect1, rect2);

        }

    }

    // unwrap array into 1D
    for (i = 0, col_index = 0; col_index < ncols; col_index++)
    {
        for (row_index = 0; row_index < nrows; row_index++, i++)
        {
            collision_matrix_unwrapped[i] = collision_matrix[row_index][col_index];
        }
    }

    return collision_matrix_unwrapped;

}

// marks objects with what objects they are colliding with
static void simulation_handle_collisions(simobject_t *obj[SIMULATION_NUM_OBJECTS], uint8_t *collision_matrix_unwrapped, fieldproperties_t props)
{

    uint8_t i, j, k;
    uint8_t collision_matrix[SIMULATION_NUM_OBJECTS][SIMULATION_NUM_OBJECTS] = {0};
    static uint8_t last_collision_matrix[SIMULATION_NUM_OBJECTS][SIMULATION_NUM_OBJECTS];
    uint8_t nrows, ncols;
    uint16_t row_index, col_index, obj_index;
    uint16_t counter = 0;
    uint8_t num_ignore_frames = 2;

    nrows = SIMULATION_NUM_OBJECTS;
    ncols = SIMULATION_NUM_OBJECTS;

    // unwrap collision matrix to 2D
    for (i = 0, col_index = 0; col_index < ncols; col_index++)
    {
        for (row_index = 0; row_index < nrows; row_index++, i++)
        {
            collision_matrix[row_index][col_index] = collision_matrix_unwrapped[i];
        }
    }

    //^ print collision matrix
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
    printf("=----------\n");
    //^

    // every collision is just a simple harmonic oscillator... sshhh!
    for (i = 0, obj_index = 0; i < nrows; i++, obj_index++)
    {
        for (j = 0; j < ncols; j++)
        {

            // if there's a collision between obj[i] and obj[j]
            if (collision_matrix[i][j])
            {

                // if these objects very recently collided, ignore it
                if (last_collision_matrix[i][j] != 0)
                {
                    last_collision_matrix[i][j]--;
                    continue;
                }

                // if it's a border collision
                if (i == j)
                {
                    //^
                    printf("object x,y velocity b4 collision = %f,%f\n", obj[i]->x_vel, obj[i]->y_vel);
                    //^

                    simobject_t border = { .mass = 10000, .x_vel = obj[i]->x_vel, .y_vel = obj[i]->y_vel };
                    simobject_collision(obj[i], &border, collision_matrix[i][j], props);

                    //^
                    printf("object x,y velocity after collision = %f,%f\n", obj[i]->x_vel, obj[i]->y_vel);
                    //^

                    last_collision_matrix[i][j] = num_ignore_frames;

                }

                // if it's an object collision
                else
                {
                    simobject_collision(obj[i], obj[j], collision_matrix[i][j], props);
                    last_collision_matrix[i][j] = num_ignore_frames;
                }

                // don't re-calculate the collision
                collision_matrix[j][i] = 0;

            }
        }
    }

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

    // applies any momenta transferrance between objects
    cm_unwrapped = simulation_check_collisions(sim, sim->objects);
    simulation_handle_collisions(sim->objects, cm_unwrapped, *sim->fieldproperties);

    // update objects according to field properties
    for(uint32_t i = 0; i < SIMULATION_NUM_OBJECTS; i++)
    {
        simobject_update_state(sim->objects[i], *sim->fieldproperties);
    }

}

// renders each object as a rectangle and draws them to the screen
static void simulation_render_objects(simulation_t *sim)
{

    shapes_render_circle(sim);

    simobject_t obj;

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
        float window_y_pos = window_y_origin + obj.y_pos;

        // create rectangle from the object's current state
        SDL_FRect rect = { window_x_pos, window_y_pos, obj.height, obj.width };

        // set object color
        if (SDL_SetRenderDrawColor(sim->sdl->renderer, sim->objects[i]->color_r, sim->objects[i]->color_g, sim->objects[i]->color_b, 0xFF))         
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
    //SDL_RenderFillRectF(sim->sdl->renderer, &sim->properties->border);

}

static void sdl_report_error()
{

    char errmsg[SDL_ERRMSG_SIZE];

    SDL_GetErrorMsg(errmsg, SDL_ERRMSG_SIZE);
    printf("%s\n", errmsg);
    
}