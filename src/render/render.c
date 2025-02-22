/*
** SDLRaycaster - a from-scratch raycasting engine in C with SDL2
** Author: Clement Colin
*/

#include <SDLRaycaster.h>

void clear_z_buffer(t_game *game)
{
	for (int i = 0; i < game->wind_width * game->wind_height; i++)
		game->z_buffer[i] = 100;
}

int check_z_buffer(t_game *game, int index, float z)
{
	if (index < 0 || index >= game->wind_width * game->wind_height)
		return (0);

	return (z < game->z_buffer[index]);
}

void set_z_buffer(t_game *game, float perp_wall_dist, int index)
{
	if (index >= 0 && index < WIND_WIDTH * WIND_HEIGHT)
		game->z_buffer[index] = perp_wall_dist;
}

static void half_down_block(t_game *game, t_raycaster *r, void *pixels)
{
	int			tex_x;
	int			start;
	int			block_height;
	double		wall_x;
	int			wind_height;
	int			tex_w;
	int			tex_h;
	double		inv_perp_wall_dist;
	Uint32		*pixel_data = (Uint32 *)pixels;
	Uint32		*texture;

	wind_height = WIND_HEIGHT;
	tex_w = game->textures.wall.width;
	tex_h = game->textures.wall.height;
	inv_perp_wall_dist = 1.0 / (r->perp_wall_dist / 2);
	int line_height = (int)(wind_height * inv_perp_wall_dist);
	start = ((wind_height - line_height) >> 1) + CAM_SHIFT;
	double normalized = (r->detected - 5) / 4.0;
	block_height = (int)(line_height * (0.1 + (normalized * 0.8)));
	wall_x = (r->side == 0)
		? (r->pos_y + r->perp_wall_dist * r->ray_dir_y)
		: (r->pos_x + r->perp_wall_dist * r->ray_dir_x);
	wall_x -= (int)wall_x;
	tex_x = (int)(wall_x * tex_w) & (tex_w - 1);
	if (r->side == 1)
		texture = game->textures.wall_dark.pixels;
	else
		texture = game->textures.wall.pixels;
	SDL_Rect dest;
	dest.x = r->x;
	dest.y = start;
	dest.w = 1;
	dest.h = block_height;

	int clip_amount = 0;
	if (dest.y < 0)
	{
		clip_amount = -dest.y;
		dest.y = 0;
	}
	int visible_height = block_height - clip_amount;
	if (visible_height <= 0) return;
	double tex_step = (double)tex_h / line_height;
	double tex_pos = clip_amount * tex_step;
	for (int y = 0; y < visible_height; y++)
	{
		int py = dest.y + y;
		if (py >= 0 && py < wind_height)
		{
			int pixel_index = py * WIND_WIDTH + dest.x;
			int tex_y = (int)tex_pos & (tex_h - 1);
			int tex_index = (tex_y * tex_w) + tex_x;
			if (tex_index >= 0 && tex_index < tex_w * tex_h)
			{
				set_z_buffer(game, r->perp_wall_dist, pixel_index);
				pixel_data[pixel_index] = texture[tex_index];
			}
			tex_pos += tex_step;
		}
	}
}

static void half_block_up(t_game *game, t_raycaster *r, void *pixels)
{
	int			tex_x, start, block_height, wind_height, tex_w, tex_h;
	double		wall_x, inv_perp_wall_dist;
	Uint32		*pixel_data = (Uint32 *)pixels;
	Uint32		*texture;

	SDL_SetTextureColorMod(game->textures.wall.texture, 255, 255, 255);
	wind_height = WIND_HEIGHT;
	tex_w = game->textures.wall.width;
	tex_h = game->textures.wall.height;
	inv_perp_wall_dist = 1.0 / (r->perp_wall_dist / 2);
	int 	line_height = ((int)(wind_height * inv_perp_wall_dist)+ 4);
	start = ((wind_height - line_height) >> 1) + CAM_SHIFT;
	double normalized = r->detected / 4.0;
	block_height = (int)(line_height * (0.1 + (normalized * 0.8))); // Min 10%, Max 90%
	wall_x = (r->side == 0)
		? (r->pos_y + r->perp_wall_dist * r->ray_dir_y)
		: (r->pos_x + r->perp_wall_dist * r->ray_dir_x);
	wall_x -= (int)wall_x;
	tex_x = (int)(wall_x * tex_w) & (tex_w - 1);
	if (r->side == 1)
		texture = game->textures.wall_dark.pixels;
	else
		texture = game->textures.wall.pixels;
	SDL_Rect dest;
	dest.x = r->x;
	dest.y = start + (line_height - block_height);
	dest.w = 1;
	dest.h = block_height;
	int clip_amount = 0;
	if (dest.y < 0)
	{
		clip_amount = -dest.y;
		dest.y = 0;
	}
	int visible_height = block_height - clip_amount;
	if (visible_height <= 0) return;
	double tex_step = (double)tex_h / line_height;
	double tex_pos = clip_amount * tex_step;
	for (int y = 0; y < visible_height; y++)
	{
		int py = dest.y + y;
		if (py >= 0 && py < wind_height)
		{
			int pixel_index = py * WIND_WIDTH + dest.x;
			int tex_y = (int)tex_pos & (tex_h - 1);
			int tex_index = (tex_y * tex_w) + tex_x;
			if (tex_index >= 0 && tex_index < tex_w * tex_h)
			{
				set_z_buffer(game, r->perp_wall_dist, pixel_index);
				pixel_data[pixel_index] = texture[tex_index];
			}
			tex_pos += tex_step;
		}
	}
	SDL_SetTextureColorMod(game->textures.wall.texture, 255, 255, 255);
}

static void draw_wall(t_game *game, t_raycaster *r, void *pixels)
{
	int			tex_x, start, line_height, wind_height, tex_w, tex_h;
	double		wall_x, inv_perp_wall_dist;
	Uint32		*pixel_data = (Uint32 *)pixels;
	Uint32		*texture;

	wind_height = WIND_HEIGHT;
	tex_w = game->textures.wall.width;
	tex_h = game->textures.wall.height;
	inv_perp_wall_dist = 1.0 / (r->perp_wall_dist / 2);
	line_height = ((int)(wind_height * inv_perp_wall_dist)+ 4);
	start = ((wind_height - line_height) >> 1) + CAM_SHIFT;
	wall_x = (r->side == 0)
		? (r->pos_y + r->perp_wall_dist * r->ray_dir_y)
		: (r->pos_x + r->perp_wall_dist * r->ray_dir_x);
	wall_x -= (int)wall_x;
	tex_x = (int)(wall_x * tex_w) & (tex_w - 1);
	if (r->side == 1)
		texture = game->textures.wall_dark.pixels;
	else
		texture = game->textures.wall.pixels;
	SDL_Rect dest;
	dest.x = r->x;
	dest.y = start;
	dest.w = 1;
	dest.h = line_height;
	int clip_amount = 0;
	if (dest.y < 0)
	{
		clip_amount = -dest.y;
		dest.y = 0;
	}
	int visible_height = line_height - clip_amount;
	if (visible_height <= 0) return;
	double tex_step = (double)tex_h / line_height;
	double tex_pos = clip_amount * tex_step;
	for (int y = 0; y < visible_height; y++)
	{
		int py = dest.y + y;
		if (py >= 0 && py < wind_height)
		{
			int pixel_index = py * WIND_WIDTH + dest.x;
			int tex_y = (int)tex_pos & (tex_h - 1);
			int tex_index = (tex_y * tex_w) + tex_x;
			if (tex_index >= 0 && tex_index < tex_w * tex_h)
			{
				set_z_buffer(game, r->perp_wall_dist, pixel_index);
				pixel_data[pixel_index] = texture[tex_index];
			}
			tex_pos += tex_step;
		}
	}
}

void	draw_floor_tile(t_game *game, t_floor_ceiling *f)
{
	int		px, py, cell_x, cell_y;
	int		floor_tx, floor_ty;
	Uint32	floor_color;
	float	floor_x, floor_y;
	float	step_x, step_y, row_distance, pos_z;
	int		y, x, p;

	pos_z = 0.402 * WIND_HEIGHT;
	f->floor_pixels = game->textures.floor_light.pixels;
	for (y = f->horizon; y < WIND_HEIGHT; y++)
	{
		p = y - (WIND_HEIGHT / 2) - CAM_SHIFT;
		row_distance = (pos_z / p) * 2;
		step_x = row_distance * (f->ray_dir_x_1 - f->ray_dir_x_0) / WIND_WIDTH;
		step_y = row_distance * (f->ray_dir_y_1 - f->ray_dir_y_0) / WIND_WIDTH;
		floor_x = PLAYER_X + row_distance * f->ray_dir_x_0;
		floor_y = PLAYER_Y + row_distance * f->ray_dir_y_0;

		int row_start = y * f->pitch; // 🔥 Precompute row memory index
		for (x = 0; x < WIND_WIDTH; x++)
		{
			cell_x = (int)floor_x;
			cell_y = (int)floor_y;

			// 🔥 Skip non-floor tiles BEFORE processing pixels
			if (cell_x >= 0 && cell_y >= 0 && cell_x < MAP_WIDTH && cell_y < MAP_HEIGHT && MAPS[LEVEL][cell_y][cell_x] == '0')
			{
				floor_tx = ((int)(game->textures.floor_light.width * (floor_x - cell_x))) & (game->textures.floor_light.width - 1);
				floor_ty = ((int)(game->textures.floor_light.height * (floor_y - cell_y))) & (game->textures.floor_light.height - 1);
				floor_color = f->floor_pixels[game->textures.floor_light.width * floor_ty + floor_tx];

				// 🔥 Lightening calculation moved OUT of pixel loop
				floor_color = floor_color | 0x010101;

				// 🔥 No need for dx/dy loops
				px = x;
				py = y;

				int pixel_index = row_start + px;
				if (check_z_buffer(game, pixel_index, row_distance))
					f->pixels[pixel_index] = floor_color;
			}

			// 🔥 Advance only AFTER pixel check
			floor_x += step_x;
			floor_y += step_y;
		}
	}
}
void	draw_floor_tile_2(t_game *game, t_floor_ceiling *f)
{
	int		px, py, cell_x, cell_y;
	int		floor_tx, floor_ty;
	Uint32	floor_color;
	float	floor_x, floor_y;
	float	step_x, step_y, row_distance, pos_z;
	int		y, x, p;

	pos_z = 0.2 * WIND_HEIGHT;
	f->floor_pixels = game->textures.floor_lighter.pixels;
	for (y = f->horizon; y < WIND_HEIGHT; y++)
	{
		p = y - (WIND_HEIGHT / 2) - CAM_SHIFT;
		row_distance = (pos_z / p) * 2;
		step_x = row_distance * (f->ray_dir_x_1 - f->ray_dir_x_0) / WIND_WIDTH;
		step_y = row_distance * (f->ray_dir_y_1 - f->ray_dir_y_0) / WIND_WIDTH;
		floor_x = PLAYER_X + row_distance * f->ray_dir_x_0;
		floor_y = PLAYER_Y + row_distance * f->ray_dir_y_0;

		int row_start = y * f->pitch; // 🔥 Precompute row memory index
		for (x = 0; x < WIND_WIDTH; x++)
		{
			cell_x = (int)floor_x;
			cell_y = (int)floor_y;

			// 🔥 Skip non-floor tiles BEFORE processing pixels
			if (cell_x >= 0 && cell_y >= 0 && cell_x < MAP_WIDTH && cell_y < MAP_HEIGHT && MAPS[LEVEL][cell_y][cell_x] == '1')
			{
				floor_tx = ((int)(game->textures.floor_lighter.width * (floor_x - cell_x))) & (game->textures.floor_lighter.width - 1);
				floor_ty = ((int)(game->textures.floor_lighter.height * (floor_y - cell_y))) & (game->textures.floor_lighter.height - 1);
				floor_color = f->floor_pixels[game->textures.floor_lighter.width * floor_ty + floor_tx];

				// 🔥 Lightening calculation moved OUT of pixel loop
				floor_color = floor_color | 0x030303;

				// 🔥 No need for dx/dy loops
				px = x;
				py = y;

				int pixel_index = row_start + px;
				if (check_z_buffer(game, pixel_index, row_distance))
					f->pixels[pixel_index] = floor_color;
			}

			// 🔥 Advance only AFTER pixel check
			floor_x += step_x;
			floor_y += step_y;
		}
	}
}


void	cast_floor_and_ceiling(t_game *game, t_floor_ceiling *f)
{
	int		px, py, cell_x, cell_y;
	int		floor_tx, floor_ty, ceiling_tx, ceiling_ty;
	float	floor_x, floor_y, ceiling_x, ceiling_y;
	float	step_x, step_y, row_distance, pos_z;
	int		y, x, p;
	Uint32	ceiling_color; 
	Uint32	floor_color; 

	f->horizon = (WIND_HEIGHT / 2) + CAM_SHIFT;
	f->ceiling_pixels = game->textures.ceiling.pixels;
	f->floor_pixels = game->textures.floor.pixels;
	f->ray_dir_x_0 = PLAYER_DIR_X - PLAYER_CAM_X;
	f->ray_dir_y_0 = PLAYER_DIR_Y - PLAYER_CAM_Y;
	f->ray_dir_x_1 = PLAYER_DIR_X + PLAYER_CAM_X;
	f->ray_dir_y_1 = PLAYER_DIR_Y + PLAYER_CAM_Y;
	pos_z = 0.5 * WIND_HEIGHT;
	for (y = 0; y < f->horizon; y += 1)
	{
		p = (WIND_HEIGHT / 2) - y + CAM_SHIFT;
		row_distance = (pos_z / p) * 2;
		step_x = row_distance * (f->ray_dir_x_1 - f->ray_dir_x_0) / WIND_WIDTH;
		step_y = row_distance * (f->ray_dir_y_1 - f->ray_dir_y_0) / WIND_WIDTH;
		ceiling_x = PLAYER_X + row_distance * f->ray_dir_x_0;
		ceiling_y = PLAYER_Y + row_distance * f->ray_dir_y_0;
		for (x = 0; x < WIND_WIDTH; x += 1)
		{
			cell_x = (int)ceiling_x;
			cell_y = (int)ceiling_y;
			if (cell_x >= 0 && cell_y >= 0 && cell_x < MAP_WIDTH
				&& cell_y < MAP_HEIGHT
				&& ((MAPS[LEVEL][cell_y][cell_x] == EMPTY || MAPS[LEVEL][cell_y][cell_x] == TRIGGER)
				|| IS_HALF_BLOCK_UP(MAPS[LEVEL][cell_y][cell_x])))
			{
				if (f->ceiling_pixels)
				{
					ceiling_tx = ((int)(game->textures.ceiling.width * (ceiling_x - cell_x)))
						& (game->textures.ceiling.width - 1);
					ceiling_ty = ((int)(game->textures.ceiling.height * (ceiling_y - cell_y)))
						& (game->textures.ceiling.height - 1);
						ceiling_color = f->ceiling_pixels[game->textures.ceiling.width * ceiling_ty + ceiling_tx];
						for (int dy = 0; dy < 1; dy++)
					{
						for (int dx = 0; dx < 1; dx++)
						{
							px = x + dx;
							py = y + dy;
							if (px < WIND_WIDTH && py >= 0 && py < WIND_HEIGHT)
							{
								if (check_z_buffer(game, py * f->pitch + px, row_distance))
								f->pixels[py * f->pitch + px] = ceiling_color;
							}
						}
					}
				}
			}
			ceiling_x += step_x;
			ceiling_y += step_y;
		}
	}
	for (y = f->horizon; y < WIND_HEIGHT; y++)
	{
		p = y - (WIND_HEIGHT / 2) - CAM_SHIFT;
		row_distance = (pos_z / p) * 2;
		step_x = row_distance * (f->ray_dir_x_1 - f->ray_dir_x_0) / WIND_WIDTH;
		step_y = row_distance * (f->ray_dir_y_1 - f->ray_dir_y_0) / WIND_WIDTH;
		floor_x = PLAYER_X + row_distance * f->ray_dir_x_0;
		floor_y = PLAYER_Y + row_distance * f->ray_dir_y_0;
		for (x = 0; x < WIND_WIDTH; x++)
		{
			cell_x = (int)floor_x;
			cell_y = (int)floor_y;
			if (cell_x >= 0 && cell_y >= 0 && cell_x < MAP_WIDTH
				&& cell_y < MAP_HEIGHT
				&& ((MAPS[LEVEL][cell_y][cell_x] == EMPTY || MAPS[LEVEL][cell_y][cell_x] == TRIGGER)
				|| IS_HALF_BLOCK_DOWN(MAPS[LEVEL][cell_y][cell_x])))
			{
				if (f->floor_pixels)
				{
					floor_tx = ((int)(game->textures.floor.width * (floor_x - cell_x)))
						& (game->textures.floor.width - 1);
					floor_ty = ((int)(game->textures.floor.height * (floor_y - cell_y)))
						& (game->textures.floor.height - 1);
					floor_color = f->floor_pixels[game->textures.floor.width * floor_ty + floor_tx];
					for (int dy = 0; dy < 1; dy++)
					{
						for (int dx = 0; dx < 1; dx++)
						{
							px = x + dx;
							py = y + dy;
							if (px < WIND_WIDTH && py < WIND_HEIGHT)
							{
								if (check_z_buffer(game, py * f->pitch + px, row_distance))
								{
									set_z_buffer(game, row_distance, py * f->pitch + px);
									f->pixels[py * f->pitch + px] = floor_color;
								}
							}
						}
					}
				}
			}
			floor_x += step_x;
			floor_y += step_y;
		}
	}
}

void	render_mini_rays(t_game *game, t_mini_ray_node **head, void *pixels)
{
	t_mini_ray_node *current = *head;
	t_mini_ray_node *temp;

	while (current)
	{
		
		if (current->ray.detected < 5)
		{
			half_block_up(game, &current->ray, pixels);
		}
		else
			half_down_block(game, &current->ray, pixels);
		temp = current;
		current = current->next;
		free(temp);
	}

	*head = NULL;
}

static void	draw_scene(t_game *game)
{
	t_raycaster			r;
	t_floor_ceiling		f;

	if (!game->textures.screen_texture)
			cleanup(game);
	if (SDL_LockTexture(game->textures.screen_texture, NULL, (void **)&f.pixels, &f.pitch) != 0)
		cleanup(game);

	f.pitch /= sizeof(Uint32);
	r.x = 0;
	while (r.x < WIND_WIDTH)
	{
		init_raycaster(&r, game);
		init_raycaster_steps(&r);
		perform_raycaster_steps(&r, game);
		draw_wall(game, &r, f.pixels);
		render_mini_rays(game, &r.mini_ray, f.pixels);
		r.x++;
	}
	cast_floor_and_ceiling(game, &f);
	draw_floor_tile(game, &f);
	draw_floor_tile_2(game, &f);
	SDL_UnlockTexture(game->textures.screen_texture);
	SDL_RenderCopy(game->renderer, game->textures.screen_texture, NULL, NULL);
	clear_z_buffer(game);
}


void	render_next_frame(t_game *game)
{
	SDL_RenderClear(RENDERER);
	draw_scene(game);
	draw_minimap(game);
	SDL_RenderPresent(RENDERER);
}

	