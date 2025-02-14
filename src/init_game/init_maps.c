/*
** SDLRaycaster - a from-scratch raycasting engine in C with SDL2
** Author: Clement Colin
*/

#include <SDLRaycaster.h>

void	init_maps(t_game *game)
{
	char	*path;
	int		i = 1;

	MAPS = malloc(sizeof(char **) * (NUMBER_OF_MAPS + 1));
	if (!MAPS)
	{
		fprintf(stderr, "ERROR: memory allocation failed in init_maps");
		exit(EXIT_FAILURE);
	}
	while (i <= NUMBER_OF_MAPS)
	{
		path = get_path(i);
		MAPS[i - 1] = get_map(path);
		free(path);
		if (!MAPS[i - 1])
		{
			free(MAPS);
			MAPS = NULL;
			return ;
		}
		i++;
	}
	MAPS[NUMBER_OF_MAPS] = NULL;
}

void	init_vector_grid(t_game *game)
{
	int	i;
	int	y;
	int	x;
	int	width;

	i = 0;
	game->vector_grid = malloc(sizeof(t_float_xy **) * (NUMBER_OF_MAPS + 1));
	if (!game->vector_grid)
	{
		fprintf(stderr, "ERROR: memory allocation failed in init_vector_grid");
		exit(EXIT_FAILURE);
	}
	while (game->maps[i])
	{
		y = 0;
		while (game->maps[i][y])
			y++;
		game->vector_grid[i] = malloc(sizeof(t_float_xy *) * (y + 1));
		if (!game->vector_grid[i])
		{
			fprintf(stderr, "ERROR: memory allocation failed in init_vector_grid");
			exit(EXIT_FAILURE);
		}
		y = 0;
		while (game->maps[i][y])
		{
			width = 0;
			while (game->maps[i][y][width])
				width++;
			game->vector_grid[i][y] = malloc(sizeof(t_float_xy) * width);
			if (!game->vector_grid[i][y])
			{
				fprintf(stderr, "ERROR: memory allocation failed in init_vector_grid");
				exit(EXIT_FAILURE);
			}
			x = 0;
			while (x < width)
			{
				game->vector_grid[i][y][x].x = 0;
				game->vector_grid[i][y][x].y = 0;
				x++;
			}
			y++;
		}
		game->vector_grid[i][y] = NULL;
		i++;
	}
	game->vector_grid[i] = NULL;
}
