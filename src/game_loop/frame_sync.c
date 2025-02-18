/*
** SDLRaycaster - a from-scratch raycasting engine in C with SDL2
** Author: Clement Colin
*/

#include <SDLRaycaster.h>

void manage_fps(t_game *game)
{
    static Uint64 last_time = 0;
    Uint64 start_time, end_time;
    double elapsed_time, frame_time_goal, wait_time, total_frame_time;
    double frequency = (double)SDL_GetPerformanceFrequency();

    start_time = SDL_GetPerformanceCounter();
    if (last_time == 0)
        last_time = start_time;

    elapsed_time = (double)(start_time - last_time) / frequency;
    game->fps = (elapsed_time > 0.0001) ? (1.0 / elapsed_time) : (double)FPS_CAP;

    frame_time_goal = 1.0 / (double)FPS_CAP;
    wait_time = frame_time_goal - elapsed_time;

    while (((double)(SDL_GetPerformanceCounter() - start_time) / frequency) < wait_time); // Active waiting to ensure more precise timing

    end_time = SDL_GetPerformanceCounter();
    total_frame_time = (double)(end_time - last_time) / frequency;
    game->fps = 1.0 / total_frame_time;
    last_time = end_time;
}

