/*
** SDLRaycaster - a from-scratch raycasting engine in C with SDL2
** Author: Clement Colin
*/

#include <SDLRaycaster.h>

static void	draw_vectors(t_game *game)
{
	int			x;
	int			y;
	t_float_xy	vec;
	SDL_Rect	center;

	for (y = 0; game->maps[LEVEL][y] != NULL; y++)
	{
		for (x = 0; game->maps[LEVEL][y][x] != '\0'; x++)
		{
			if (game->maps[LEVEL][y][x] != EMPTY)
				continue ;
			vec = game->vector_grid[LEVEL][y][x];
			center.x = x * MINIMAP_BLOCK + MINIMAP_BLOCK / 2;
			center.y = y * MINIMAP_BLOCK + MINIMAP_BLOCK / 2;
			center.w = 2;
			center.h = 2;
			SDL_SetRenderDrawColor(RENDERER, 0, 0, 0, 255);
			SDL_RenderFillRect(RENDERER, &center);
			if (vec.x == 0 && vec.y == 0)
			{
				SDL_RenderDrawLine(RENDERER, center.x - 3, center.y, center.x + 3, center.y);
				SDL_RenderDrawLine(RENDERER, center.x, center.y - 3, center.x, center.y + 3);
			}
			else
			{
				SDL_RenderDrawLine(RENDERER, center.x, center.y,
					center.x + vec.x * MINIMAP_BLOCK / 2,
					center.y + vec.y * MINIMAP_BLOCK / 2);
			}
		}
	}
}

void	draw_minimap(t_game *game)
{
	int	x;
	int	y;

	if (!SHOW_MINIMAP)
		return ;
	for (y = 0; MAPS[LEVEL][y] != NULL; y++)
	{
		for (x = 0; MAPS[LEVEL][y][x] != '\0'; x++)
		{
			SDL_Rect	cell = {x * MINIMAP_BLOCK, y * MINIMAP_BLOCK, MINIMAP_BLOCK, MINIMAP_BLOCK};
			if (MAPS[LEVEL][y][x] == EMPTY)
				SDL_SetRenderDrawColor(RENDERER, 255, 255, 0, 255);
			else if (MAPS[LEVEL][y][x] == WALL)
				SDL_SetRenderDrawColor(RENDERER, 128, 128, 128, 255);
			else if (MAPS[LEVEL][y][x] == DOOR_CLOSED)
				SDL_SetRenderDrawColor(RENDERER, 139, 69, 19, 255);
			SDL_RenderFillRect(RENDERER, &cell);
		}
	}
	SDL_SetRenderDrawColor(RENDERER, 255, 0, 0, 255);
	draw_cercle(RENDERER, PLAYER_X * MINIMAP_BLOCK, PLAYER_Y * MINIMAP_BLOCK, MINIMAP_BLOCK / 4);
	SDL_RenderDrawLine(RENDERER, PLAYER_X * MINIMAP_BLOCK, PLAYER_Y * MINIMAP_BLOCK,
		PLAYER_X * MINIMAP_BLOCK + (PLAYER_DIR_X * MINIMAP_BLOCK),
		PLAYER_Y * MINIMAP_BLOCK + (PLAYER_DIR_Y * MINIMAP_BLOCK));
	if (SHOW_PATH_FIND)
		draw_vectors(game);
}
