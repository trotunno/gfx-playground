/*
 *  collisions.c
 *
 *  Created on: Jun 24, 2022
 *      Author: Dylan
 */

/* ---------------------------------------------------------------------------------------- */

#include <float.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "../inc/SDL2/SDL.h"
#include "../inc/collisions.h"

/* ---------------------------------------------------------------------------------------- */

// checks if rect1 is colliding with rect2, returns side of collision for each object
uint8_t detect_object_collision(SDL_FRect rect1, SDL_FRect rect2)
{

    uint16_t i, j, k;
    SDL_FPoint point;
    uint8_t collision_detected = 0;

    SDL_FRect intersect = {0};

    if (SDL_HasIntersectionF(&rect1, &rect2))
    {

        // right side collision
        if ((rect1.x + rect1.w) > (rect2.x))
        {
            collision_detected |= 0x01;
        }

        // left side collision
        if (rect1.x < (rect2.x + rect2.w))
        {
            collision_detected |= 0x02;
        }

        // top collision
        if ((rect1.y + rect1.h) > rect2.y)
        {
            collision_detected |= 0x04;
        }

        // bottom collision
        if (rect1.y < (rect2.y + rect2.h))
        {
            collision_detected |= 0x08;
        }

    }

    return collision_detected;

}

// returns type of collision with border (top, bottom, left, right, outside)
uint8_t detect_border_collision(SDL_FRect rect1, SDL_FRect border)
{

    uint16_t i, j, k;
    SDL_FPoint point = {0};
    SDL_FRect _union = {0};
    uint8_t collision_detected = 0;

    // get center point of rectangle
    point.x = rect1.x + (rect1.w / 2.0);
    point.y = rect1.y + (rect1.h / 2.0);

    // if the point isn't even in within the border, special case
    if (!SDL_PointInFRect(&point, &border))
    {
        collision_detected = 0;
    }
    else
    {
        // left side collision
        if (rect1.x < border.x)
        {
            collision_detected |= 0x01;
        }

        // right side collision
        if ((rect1.x + rect1.w) > (border.x + border.w))
        {
            collision_detected |= 0x02;
        }

        // top collision
        if (rect1.y < border.y)
        {
            collision_detected |= 0x04;
        }

        // bottom collision
        if ((rect1.y + rect1.h) > (border.y + border.h))
        {
            collision_detected |= 0x08;
        }

    }

    return collision_detected;

}