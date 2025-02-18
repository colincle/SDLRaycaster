/*
** SDLRaycaster - a from-scratch raycasting engine in C with SDL2
** Author: Clement Colin
*/

#include <SDLRaycaster.h>

void	quit_game(t_game *game)
{
	SDL_DestroyWindow(WINDOW);
	SDL_DestroyRenderer(RENDERER);
	SDL_Quit();
	exit(EXIT_SUCCESS);
}

static void	game_loop(t_game *game)
{
	int		running;
	void	(*chapter[])(t_game *game, int *running) = {chapter_1, chapter_2, chapter_3, chapter_4, chapter_5};

	running = TRUE;
	LEVEL = 0;
	while (running)
	{
		chapter[LEVEL](game, &running);
		handle_events(game, &running);
		update_entities(game);
		render_next_frame(game);
		debug_statements(game);
		manage_fps(game);
	}
}

int	main(void)
{
	t_game	*game;

	game = game_init();
	game_loop(game);
	cleanup(game);
	return (0);
}
