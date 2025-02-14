/*
** SDLRaycaster - a from-scratch raycasting engine in C with SDL2
** Author: Clement Colin
*/

#include <SDLRaycaster.h>

void	manage_fps(t_game *game)
{
	static Uint32	last_time = 0;
	Uint32			start_time;
	Uint32			end_time;
	float			elapsed_time;
	float			frame_time_goal;
	float			wait_time;
	float			total_frame_time;

	start_time = SDL_GetTicks();
	if (last_time == 0)
		last_time = start_time;
	elapsed_time = (start_time - last_time) / 1000.0f;
	if (elapsed_time > 0.0001f)
		game->fps = 1.0f / elapsed_time;
	else
		game->fps = (float)FPS_CAP;
	frame_time_goal = 1.0f / (float)FPS_CAP;
	wait_time = frame_time_goal - elapsed_time;
	if (wait_time > 0)
		SDL_Delay((Uint32)(wait_time * 1000));
	end_time = SDL_GetTicks();
	total_frame_time = (end_time - last_time) / 1000.0f;
	game->fps = 1.0f / total_frame_time;
	last_time = end_time;
}
