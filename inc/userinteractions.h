/*
 * userinteractions.h
 *
 *  Created on: Jun 8, 2022
 *      Author: Dylan
 */

#ifndef _INC_USERINTERACTIONS_H
#define _INC_USERINTERACTIONS_H

/* ---------------------------------------------------------------------------------------- */

#include <SDL2/SDL.h>
#include "common.h"

// manages user interactions
typedef struct userinteractions_t
{
    
    bool space_pressed;
    bool escape_pressed;

} userinteractions_t;

#endif