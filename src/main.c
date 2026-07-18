#include <SDLRaycaster.h>

void	quit_game(t_game *game)
{
	SDL_DestroyWindow(WINDOW);
	SDL_DestroyRenderer(RENDERER);
	SDL_Quit();
}

static void	level_trigger(t_game *game)
{
	int	next;

	if (MAPS[LEVEL][(int)PLAYER_Y][(int)PLAYER_X] != TRIGGER)
		return ;
	// Levels loop: the last map's trigger sends the player back to map 2
	// (index 1) rather than advancing past the end. Every other trigger
	// advances to the next map. This also keeps the index inside the
	// NUMBER_OF_MAPS-sized player array (player[LEVEL + 1] on the last map
	// would be out of bounds).
	if (LEVEL == NUMBER_OF_MAPS - 1)
		next = 1;
	else
		next = LEVEL + 1;
	game->player[next]->x = PLAYER_X;
	game->player[next]->y = PLAYER_Y;
	game->player[next]->dir.x = PLAYER_DIR_X;
	game->player[next]->dir.y = PLAYER_DIR_Y;
	game->player[next]->cam.x = PLAYER_CAM_X;
	game->player[next]->cam.y = PLAYER_CAM_Y;
	game->player[next]->camera_shift = CAM_SHIFT;
	game->player[next]->feet_touch = FEET_TOUCH;
	game->player[next]->jumping = JUMP;
	game->player[next]->crouching = CROUCH;
	game->player[next]->speed = PLAYER_SPEED;
	game->player[next]->crouch_lock = CROUCH_LOCK;
	game->player[next]->stand_lock = STAND_LOCK;
	game->player[next]->falling = FALLING;
	game->player[next]->jump_lock = JUMP_LOCK;
	LEVEL = next;
}

static void	game_frame(void *arg)
{
	static void	(*chapter[])(t_game *game, int *running) = {chapter_1, chapter_2, chapter_3, chapter_4, chapter_5};
	t_main_loop	*loop = arg;
	t_game		*game = loop->game;

	sounds(game);
	chapter[LEVEL](game, &loop->running);
	level_trigger(game);
	handle_events(game, &loop->running);
	update_entities(game);
	render_next_frame(game);
	debug_statements(game);
	manage_fps(game);
#ifdef __EMSCRIPTEN__
	if (!loop->running)
		emscripten_cancel_main_loop();
#endif
}

static void	game_loop(t_game *game)
{
	// static: under Emscripten this function returns while frames keep firing
	static t_main_loop	loop;

	loop.game = game;
	loop.running = TRUE;
	LEVEL = START_LEVEL;
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop_arg(game_frame, &loop, 0, TRUE);
#else
	while (loop.running)
		game_frame(&loop);
#endif
}

int	main(void)
{
	t_game	*game;

	game = init_game();
	game_loop(game);
	cleanup(game);
	return (0);
}
