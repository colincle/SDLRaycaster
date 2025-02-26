/*
** SDLRaycaster - a from-scratch raycasting engine in C with SDL2
** Author: Clement Colin
*/

#include <SDLRaycaster.h>

void	move_player(t_game *game, int key)
{
	float	x;
	float	y;

	if (key == W)
	{
		x = PLAYER_X + (PLAYER_DIR_X * (PLAYER_SPEED) * (1.0 / FPS));
		y = PLAYER_Y + (PLAYER_DIR_Y * (PLAYER_SPEED) * (1.0 / FPS));
	}
	if (key == S)
	{
		x = PLAYER_X - (PLAYER_DIR_X * (PLAYER_SPEED) * (1.0 / FPS));
		y = PLAYER_Y - (PLAYER_DIR_Y * (PLAYER_SPEED) * (1.0 / FPS));
	}
	if (key == A)
	{
		x = PLAYER_X + (PLAYER_DIR_Y * (PLAYER_SPEED * 0.6) * (1.0 / FPS));
		y = PLAYER_Y - (PLAYER_DIR_X * (PLAYER_SPEED * 0.6) * (1.0 / FPS));
	}
	if (key == D)
	{
		x = PLAYER_X - (PLAYER_DIR_Y * (PLAYER_SPEED * 0.6) * (1.0 / FPS));
		y = PLAYER_Y + (PLAYER_DIR_X * (PLAYER_SPEED * 0.6) * (1.0 / FPS));
	}
	collisions(game, x, y);
	if (PLAYER_SPEED == DEFAULT_SPEED)
		MOVING = WALKING;
	else
		MOVING = RUNNING;
}

void	move_player_joystick(t_game *game, float x, float y)
{
	float	move_x = PLAYER_X;
	float	move_y = PLAYER_Y;
	float	frame_time = 1.0 / FPS;

	if (fabs(x) > 0.1)
	{
		move_x -= PLAYER_DIR_Y * (PLAYER_SPEED * 0.6) * (x * frame_time);
		move_y += PLAYER_DIR_X * (PLAYER_SPEED * 0.6) * (x * frame_time);
	}
	if (fabs(y) > 0.1)
	{
		move_x += PLAYER_DIR_X * PLAYER_SPEED * (y * frame_time);
		move_y += PLAYER_DIR_Y * PLAYER_SPEED * (y * frame_time);
	}
	collisions(game, move_x, move_y);
	if (PLAYER_SPEED == DEFAULT_SPEED)
		MOVING = WALKING;
	else
		MOVING = RUNNING;
}
