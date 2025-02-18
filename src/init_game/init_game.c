/*
** SDLRaycaster - a from-scratch raycasting engine in C with SDL2
** Author: Clement Colin
*/

#include <SDLRaycaster.h>

static void	game_struct_init(t_game *game)
{
	init_maps(game);
	print_all_maps(game);
	init_vector_grid(game);
	init_entities(game);
	print_entities(game);
	KEYS = malloc(sizeof(int) * HOW_MANY_KEYS);
	if (!KEYS)
	{
		fprintf(stderr, "ERROR: memory allocation failed in game_struct_init");
		exit(EXIT_FAILURE);
	}
}

static void	graphics_init(t_game *game)
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		printf("SDL_Init Error: %s\n", SDL_GetError());
		cleanup(game);
	}
	WINDOW = SDL_CreateWindow("SDLRaycaster", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_FULLSCREEN_DESKTOP);
	if (!WINDOW)
	{
		printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
		cleanup(game);
	}
	RENDERER = SDL_CreateRenderer(WINDOW, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	// RENDERER = SDL_CreateRenderer(WINDOW, -1, SDL_RENDERER_ACCELERATED);
	if (!RENDERER)
	{
		printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
		cleanup(game);
	}
	SDL_GetWindowSize(WINDOW, &WIND_WIDTH, &WIND_HEIGHT);
}

void	controller_init(t_game *game)
{
	if (SDL_Init(SDL_INIT_GAMECONTROLLER) < 0) 
	{
		printf("SDL could not initialize! Error: %s\n", SDL_GetError());
		cleanup(game);
	}
	SDL_GameController *controller = NULL;
	if (SDL_NumJoysticks() > 0 && SDL_IsGameController(0)) 
	{
	    controller = SDL_GameControllerOpen(0);
	    if (!controller) 
		{
	        printf("Failed to open controller: %s\n", SDL_GetError());
	    }
		else 
		{
	        printf("Controller connected: %s\n", SDL_GameControllerName(controller));
	    }
	}
	else 
	{
	    printf("No compatible controller found.\n");
	}
}

void load_textures(t_game *game)
{
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        printf("IMG_Init Error: %s\n", IMG_GetError());
        cleanup(game);
    }
	SDL_Surface *surface = IMG_Load("./assets/textures/png/east.png");
    if (!surface)
    {
        printf("IMG_Load Error: %s\n", IMG_GetError());
        cleanup(game);
    }
    game->textures.wall.texture = SDL_CreateTextureFromSurface(RENDERER, surface);
    SDL_FreeSurface(surface);
    if (!game->textures.wall.texture)
    {
        printf("SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
		cleanup(game);
    }
	SDL_QueryTexture(game->textures.wall.texture, NULL, NULL, &game->textures.wall.width, &game->textures.wall.height);
}

t_game	*game_init(void)
{
	t_game	*game;

	game = malloc(sizeof(t_game));
	if (!game)
	{
		fprintf(stderr, "ERROR: memory allocation failed in game_init");
		exit(EXIT_FAILURE);
	}
	controller_init(game);
	graphics_init(game);
	load_textures(game);
	game_struct_init(game);
	SDL_SetHint(SDL_HINT_MOUSE_RELATIVE_MODE_WARP, "0");
	SDL_SetRelativeMouseMode(SDL_TRUE);	
	return (game);
}
