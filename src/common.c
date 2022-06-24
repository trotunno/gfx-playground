/*
 *  common.c
 *
 *  Created on: Jun 23, 2022
 *      Author: Dylan
 */

/* ---------------------------------------------------------------------------------------- */

#include <time.h>
#include <stdarg.h>

#include "../inc/common.h"

/* ---------------------------------------------------------------------------------------- */

void sdelay(int number_of_seconds)
{
    // Converting time into milliseconds
    int milli_seconds = 1000 * number_of_seconds;
  
    // Storing start time
    clock_t start_time = clock();
  
    // looping till required time is not achieved
    while (clock() < start_time + milli_seconds);
}