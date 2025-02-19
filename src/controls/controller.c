/*
** SDLRaycaster - a from-scratch raycasting engine in C with SDL2
** Author: Clement Colin
*/

#include <SDLRaycaster.h>

void	left_stick(t_game *game)
{
	static int	last_time;
	int			new_time;
	int			ellapsed;

	new_time = SDL_GetTicks();
	ellapsed = new_time - last_time;
	if (fabs(JOYSTICK_Y) < 0.1 && fabs(JOYSTICK_X) < 0.1)
	{
		if (ellapsed > 100)
		{
			if (!KEYS[SHIFT])
				PLAYER_SPEED = DEFAULT_SPEED;
		}
	}
	else
		last_time = new_time;
}

void	manage_controller(t_game *game)
{
	left_stick(game);
}
