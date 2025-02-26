/*
** SDLRaycaster - a from-scratch raycasting engine in C with SDL2
** Author: Clement Colin
*/

#include <SDLRaycaster.h>

static void	get_P_cores(t_game *game)
{
	int		pcores = 0;
	size_t	size = sizeof(pcores);

	if (sysctlbyname("hw.perflevel0.logicalcpu", &pcores, &size, NULL, 0) == 0)
		game->P_cores = pcores;
	else
		game->P_cores = 0;
}

static t_game	*game_struct_init(void)
{
	t_game	*game;

	game = malloc(sizeof(t_game));
	if (!game)
	{
		fprintf(stderr, "ERROR: memory allocation failed in init_game");
		exit(EXIT_FAILURE);
	}
	game->fps = 0;
	game->level = 0;
	game->moving = 0;
	game->wind_width = 0;
	game->wind_height = 0;
	game->P_cores = 0;
	game->event = (SDL_Event){0};
	game->input = (t_input){0};
	game->textures = (t_textures){0};
	game->sounds = (t_sounds){0};
	game->maps = NULL;
	game->player = NULL;
	game->enemy = NULL;
	game->window = NULL;
	game->renderer = NULL;
	game->z_buffer = NULL;
	game->screen = NULL;
	keyboard_key_states_init(game);
	get_P_cores(game);
	return (game);
}

t_game	*init_game(void)
{
	t_game	*game;

	game = game_struct_init();
	init_graphics(game);
	controller_init(game);
	mouse_init();
	sound_init(game);
	load_textures(game);
	maps_init(game);
	print_all_maps(game);
	entities_init(game);
	print_entities(game);
	pixel_buffers_init(game);
	return (game);
}
