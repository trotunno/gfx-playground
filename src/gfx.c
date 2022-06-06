/*
 * gfx.c
 *
 *  Created on: Jun 4, 2022
 *      Author: Dylan
 */

#include <SDL2/SDL.h>
#include <string.h>
#include "common.h"
#include "gfx.h"

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