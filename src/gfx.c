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

#define WIDTH 640
#define HEIGHT 480
#define SIZE 200
#define FPS 60

/* ---------------------------------------------------------------------------------------- */

static void sdl_reportAndDestroy(void);

/* ---------------------------------------------------------------------------------------- */

SDL_Window *window;
SDL_Renderer *renderer;

/* ---------------------------------------------------------------------------------------- */

void sdl_init()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        sdl_reportAndDestroy();
    }

    window = SDL_CreateWindow("Hello World!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    if (!window)
    {
        sdl_reportAndDestroy();
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer)
    {
        sdl_reportAndDestroy();
    }
}

// starts the main while loop
void sdl_start()
{
    // loop variables
    bool running = true;
    float x_pos = (WIDTH-SIZE)/2, y_pos = (HEIGHT-SIZE)/2;
    SDL_Rect rectangle = {(int) x_pos, (int) y_pos, SIZE, SIZE};
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
            }
        }

        // update the rendering object
        rectangle.h = (int)x_pos;
        rectangle.y = (int)y_pos;
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &rectangle);
        
        // draw to the window, wait for 1 frame
        SDL_RenderPresent(renderer);
        SDL_Delay(1000/FPS);

    }

    // murder this process
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