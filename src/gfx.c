/*
 * gfx.c
 *
 *  Created on: Jun 4, 2022
 *      Author: Dylan
 */

#include <string.h>
#include "../inc/gfx.h"
#include "../inc/SDL2/SDL.h"

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

void sdl_init()
{

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
void sdl_start()
{
    
    bool running = true;
    bool jump_pressed = false;
    bool can_jump = true;
    bool left_pressed = false;
    bool right_pressed = false;
    
    float x_pos = (WINDOW_WIDTH - SIZE) / 2;
    float y_pos = (WINDOW_HEIGHT - SIZE) / 2;
    float x_vel = 0;
    float y_vel = 0;
    
    SDL_Rect rectangle = { (int)x_pos, (int)y_pos, SIZE, SIZE };
    
    // ---- Main Loop ---- //

    SDL_Event event;
    while(running)
    {
        
        // check for events
        while (SDL_PollEvent(&event))
        {
            switch(event.type)
            {
              
                case SDL_QUIT:
                    running = false;
                    break;
                
                case SDL_KEYDOWN:
                    switch (event.key.keysym.scancode)
                    {
                        case SDL_SCANCODE_UP:
                        case SDL_SCANCODE_SPACE:
                            jump_pressed = true;
                            break;

                        case SDL_SCANCODE_A:
                        case SDL_SCANCODE_LEFT:
                            left_pressed = true;
                            break;

                        case SDL_SCANCODE_D:
                        case SDL_SCANCODE_RIGHT:
                            right_pressed = true;
                            break;

                        default:
                            break;
                    }
                    break;

                case SDL_KEYUP:
                    switch (event.key.keysym.scancode)
                    {
                        case SDL_SCANCODE_UP:
                        case SDL_SCANCODE_SPACE:
                            jump_pressed = false;
                            break;

                        case SDL_SCANCODE_A:
                        case SDL_SCANCODE_LEFT:
                            left_pressed = false;
                            break;
                        
                        case SDL_SCANCODE_D:
                        case SDL_SCANCODE_RIGHT:
                            right_pressed = false;
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
        x_vel = (right_pressed - left_pressed)*SPEED;
        y_vel += GRAVITY;

        if (jump_pressed && can_jump)
        {
            can_jump = false;
            y_vel = JUMP;
        }

        x_pos += x_vel / 60;
        y_pos += y_vel / 60;
    
        if (x_pos <= 0)
            x_pos = 0;

        if (x_pos >= WINDOW_WIDTH - rectangle.w)
            x_pos = WINDOW_WIDTH - rectangle.w;

        if (y_pos <= 0)
            y_pos = 0;

        if (y_pos >= WINDOW_HEIGHT - rectangle.h)
        {
            y_vel = 0;
            y_pos = WINDOW_HEIGHT - rectangle.h;

            if (!jump_pressed)
                can_jump = true;
        }

        // update the rendering object(s)
        rectangle.x = (int) x_pos;
        rectangle.y = (int) y_pos;

        // draw the rendering object(s)
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &rectangle);

        // update the window, wait for 1 frame before looping again
        SDL_RenderPresent(renderer);
        SDL_Delay(1000/FPS);

    }
    
    // kill the SDL process
    sdl_kill();

}

// destroys all SDL objects and quits
void sdl_kill()
{
    if (window)   SDL_DestroyWindow(window);
    if (renderer) SDL_DestroyRenderer(renderer);

    SDL_Quit();
}

/* ---------------------------------------------------------------------------------------- */

static void sdl_reportAndDestroy()
{
    char errmsg[ERRMSG_SIZE];

    SDL_GetErrorMsg(errmsg, ERRMSG_SIZE);
    printf("%s\n", errmsg);

    sdl_kill();
}