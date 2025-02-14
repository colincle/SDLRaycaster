/*
** SDLRaycaster - a from-scratch raycasting engine in C with SDL2
** Author: Clement Colin
*/

#include <SDLRaycaster.h>

static void	proto_3d_render(t_game *game, t_raycaster *r)
{
	int	line_height;
	int	start;
	int	end;

	r->perp_wall_dist /= 2;
	line_height = game->wind_height / r->perp_wall_dist;
	start = (game->wind_height - line_height) / 2;
	end = start + line_height;
	draw_column(RENDERER, r->x, 0, start, int_to_color(0x808080FF));
	draw_column(RENDERER, r->x, start++, end++, int_to_color(r->wall_dir));
	draw_column(RENDERER, r->x, end, game->wind_height, int_to_color(0x808080FF));
}

static void	draw_scene(t_game *game)
{
	t_raycaster	*r;

	r = malloc(sizeof(t_raycaster));
	if (!r)
	{
		fprintf(stderr, "ERROR: memory allocation failed in draw_scene");
		exit(EXIT_FAILURE);
	}
	r->x = 0;
	while (r->x < WIND_WIDTH)
	{
		init_raycaster(r, game);
		init_raycaster_steps(r);
		perform_raycaster_steps(r, game);
		proto_3d_render(game, r);
		r->x++;
	}
	free(r);
}

void	render_next_frame(t_game *game)
{
	SDL_RenderClear(RENDERER);
	draw_scene(game);
	draw_minimap(game);
	SDL_RenderPresent(RENDERER);
}
