/*
** SDLRaycaster - a from-scratch raycasting engine in C with SDL2
** Author: Clement Colin
*/

#include <SDLRaycaster.h>

static void	move_player(t_game *game, int key)
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
	if (MAPS[LEVEL][(int)PLAYER_Y][(int)x] != WALL)
		PLAYER_X = x;
	if (MAPS[LEVEL][(int)y][(int)PLAYER_X] != WALL)
		PLAYER_Y = y;
}

static void move_player_joystick(t_game *game, float x, float y)
{
	float move_x = PLAYER_X;
	float move_y = PLAYER_Y;
	float frame_time = 1.0 / FPS;

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

	if (MAPS[LEVEL][(int)PLAYER_Y][(int)move_x] != WALL)
		PLAYER_X = move_x;
	if (MAPS[LEVEL][(int)move_y][(int)PLAYER_X] != WALL)
		PLAYER_Y = move_y;
}


void	rotate_player_mouse(t_game *game, int x)
{
	double	old_dir_x;
	double	frame_time = 1.0 / game->fps;
	double	angle = abs(x) * MOUSE_SENSIT * frame_time;
	double	sin_rot = sin(angle);
	double	cos_rot = cos(angle);

	old_dir_x = PLAYER_DIR_X;
	if (x > 1)
	{
		PLAYER_DIR_X = (PLAYER_DIR_X * cos_rot - PLAYER_DIR_Y * sin_rot);
		PLAYER_DIR_Y = (old_dir_x * sin_rot + PLAYER_DIR_Y * cos_rot);
	}
	if (x < -1)
	{
		PLAYER_DIR_X = (PLAYER_DIR_X * cos_rot + PLAYER_DIR_Y * sin_rot);
		PLAYER_DIR_Y = (-old_dir_x * sin_rot + PLAYER_DIR_Y * cos_rot);
	}
	set_player_cam(game, LEVEL);
	MOUSE_X = 0;
}

void	rotate_player_joystick(t_game *game, float x)
{
	double	old_dir_x;
	double	frame_time = 1.0 / game->fps;
	double	angle = fabs(x) * JOY_SENSIT * frame_time;
	double	sin_rot = sin(angle);
	double	cos_rot = cos(angle);

	old_dir_x = PLAYER_DIR_X;
	if (x < -0.1)
	{
		PLAYER_DIR_X = (PLAYER_DIR_X * cos_rot - PLAYER_DIR_Y * sin_rot);
		PLAYER_DIR_Y = (old_dir_x * sin_rot + PLAYER_DIR_Y * cos_rot);
	}
	if (x > 0.1)
	{
		PLAYER_DIR_X = (PLAYER_DIR_X * cos_rot + PLAYER_DIR_Y * sin_rot);
		PLAYER_DIR_Y = (-old_dir_x * sin_rot + PLAYER_DIR_Y * cos_rot);
	}
	set_player_cam(game, LEVEL);
}

static void	update_player(t_game *game)
{
	if (KEYS[W])
		move_player(game, W);
	if (KEYS[S])
		move_player(game, S);
	if (KEYS[A])
		move_player(game, A);
	if (KEYS[D])
		move_player(game, D);
	if (JOYSTICK_Y || JOYSTICK_X)
		move_player_joystick(game, JOYSTICK_X, JOYSTICK_Y);
	if (MOUSE_X)
		rotate_player_mouse(game, MOUSE_X);
	if (JOYSTICK_ROT)
		rotate_player_joystick(game, JOYSTICK_ROT);
}

void	update_entities(t_game *game)
{
	update_vector_grid(game);
	update_player(game);
}
