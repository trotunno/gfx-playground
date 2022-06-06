/*
 * gfx.c
 *
 *  Created on: Jun 4, 2022
 *      Author: Dylan
 */

#include <string.h>
#include "../inc/gfx.h"
#include "../inc/SDL2/SDL.h"

static void sdl_ReportErrorAndExit(char *errmsg, uint16_t size);

/* ---------------------------------------------------------------------------------------- */

void SDL_Create()
{

    // if initialization fails, print error string and exit
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        char errmsg[150];
        SDL_GetErrorMsg(errmsg, 150);
        sdl_ReportErrorAndExit(errmsg, 150);
    }

}

void SDL_Destroy()
{
    SDL_Quit();
}

/* ---------------------------------------------------------------------------------------- */

static void sdl_ReportErrorAndExit(char *errmsg, uint16_t size)
{
    printf("%s\n", errmsg);
    SDL_Destroy();
}