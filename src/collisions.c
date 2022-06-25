#include <float.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "../inc/SDL2/SDL.h"

#include "../inc/collisions.h"

// checks if rect1 is colliding with rect2
bool detect_object_collision(SDL_FRect rect1, SDL_FRect rect2)
{

    uint16_t i, j, k;
    SDL_FPoint point;
    bool collision_detected = false;

    SDL_FRect intersect = {0};

    if (SDL_HasIntersectionF(&rect1, &rect2))
    {
        collision_detected = true;
    }

    return collision_detected;

}

bool detect_border_collision(SDL_FRect rect1, SDL_FRect border)
{

    uint16_t i, j, k;
    SDL_FPoint point = {0};
    SDL_FRect _union = {0};
    bool collision_detected = false;

    // get center point of rectangle
    point.x = rect1.x + (rect1.w / 2.0);
    point.y = rect1.y + (rect1.h / 2.0);

    // if the point isn't even in within the border... it's a collision.
    if (!SDL_PointInFRect(&point, &border))
    {
        collision_detected = true;
    }
    else
    {
        
        // if any part of the rectangle is outside of the border rectangle
        if ( rect1.x < border.x || ((rect1.x + rect1.w) > (border.x + border.w)) || rect1.y < border.y  || ((rect1.y + rect1.h) > (border.y + border.h)) )
        {
            collision_detected = true;
        }

    }

    return collision_detected;

}