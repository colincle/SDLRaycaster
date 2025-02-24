/*
** SDLRaycaster - a from-scratch raycasting engine in C with SDL2
** Author: Clement Colin
*/

#include <SDLRaycaster.h>

Uint32 start_timer(Uint32 milliseconds)
{
	Uint32	timer_end;
	
	timer_end = SDL_GetTicks() + milliseconds;
	return (timer_end);
}

int traversable(int height, char c)
{
    if (height >= 0 && height < 500)
        return (!strchr("#1234789.D", c));
    else if (height >= 500)
        return (!strchr("#234789.D", c));
    return 0;
}


#define COLLISION_RADIUS 0.3f

// Returns true if the circle centered at (x,y) with radius COLLISION_RADIUS collides with a wall.
int check_circle_collision(t_game *game, float x, float y)
{
    // 0: no collision, 1: collision (touching), 2: center inside wall

    // First, check if the player's center is inside a non‑traversable cell.
    int center_cell_x = (int)floorf(x);
    int center_cell_y = (int)floorf(y);
    if (!traversable(PLAYER_HEIGHT, MAPS[LEVEL][center_cell_y][center_cell_x]))
        return 2;  // Center is inside a wall.

    // Otherwise, check for a collision along the circle’s boundary.
    int cell_min_x = (int)floorf(x - COLLISION_RADIUS);
    int cell_max_x = (int)ceilf(x + COLLISION_RADIUS);
    int cell_min_y = (int)floorf(y - COLLISION_RADIUS);
    int cell_max_y = (int)ceilf(y + COLLISION_RADIUS);

    for (int j = cell_min_y; j <= cell_max_y; j++) {
        for (int i = cell_min_x; i <= cell_max_x; i++) {
            if (!traversable(PLAYER_HEIGHT, MAPS[LEVEL][j][i])) {
                // Compute the nearest point on cell (i, j) to (x, y)
                float nearest_x = fmaxf(i, fminf(x, i + 1));
                float nearest_y = fmaxf(j, fminf(y, j + 1));
                float dx = x - nearest_x;
                float dy = y - nearest_y;
                if ((dx * dx + dy * dy) < (COLLISION_RADIUS * COLLISION_RADIUS))
                    return 1;  // The circle touches the wall.
            }
        }
    }
    return 0;  // No collision detected.
}


void collisions(t_game *game, float new_x, float new_y)
{
    // First, use our new check_circle_collision to determine the overall state.
    int state = check_circle_collision(game, new_x, new_y);
    
    if (state == 0 || state == 1) {
        // Normal sliding collision system.
        float old_x = PLAYER_X;
        float old_y = PLAYER_Y;
        float dx = new_x - old_x;
        float dy = new_y - old_y;
    
        // Try moving along X axis first.
        float temp_x = old_x + dx;
        if (check_circle_collision(game, temp_x, old_y) != 2)
            PLAYER_X = temp_x;
        // Then try moving along Y axis.
        float temp_y = old_y + dy;
        if (check_circle_collision(game, PLAYER_X, temp_y) != 2)
            PLAYER_Y = temp_y;
    }
    else if (state == 2) {
        // Fallback: simple collision system based solely on the player center.
        // Determine which cell the center would be in.
        int cell_x = (int)floorf(new_x);
        int cell_y = (int)floorf(new_y);
        // If that cell is traversable, allow the move; otherwise, do nothing.
        if (traversable(PLAYER_HEIGHT, MAPS[LEVEL][cell_y][cell_x])) {
            PLAYER_X = new_x;
            PLAYER_Y = new_y;
        }
        // (You might later want to add a correction here to push the center out of the wall.)
    }
}




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
	collisions(game, x, y);
	if (PLAYER_SPEED == DEFAULT_SPEED)
		MOVING = WALKING;
	else
		MOVING = RUNNING;
}

static void	move_player_joystick(t_game *game, float x, float y)
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

void	look_up_and_down_joystick(t_game *game, float x)
{
	double	frame_time = 1.0 / game->fps;
	double	offset = x * JOY_SENSIT * frame_time * 500;
	double	max_shift = WIND_HEIGHT * 0.15;

	CAM_SHIFT += offset;
	if (CAM_SHIFT > max_shift)
		CAM_SHIFT = max_shift;
	if (CAM_SHIFT < -max_shift)
		CAM_SHIFT = -max_shift;
}

void	look_up_and_down_mouse(t_game *game, int y)
{
	double	frame_time = 1.0 / game->fps;
	double	offset = abs(y) * MOUSE_SENSIT * frame_time * 500;
	double	max_shift = WIND_HEIGHT * 0.15;

	if (y > 1)
		CAM_SHIFT -= offset;
	if (y < -1)
		CAM_SHIFT += offset;
	if (CAM_SHIFT > max_shift)
		CAM_SHIFT = max_shift;
	if (CAM_SHIFT < -max_shift)
		CAM_SHIFT = -max_shift;
	set_player_cam(game, LEVEL);
	MOUSE_Y = 0;
}

void jump(t_game *game)
{
    float dt = 1.0f / game->fps;   // seconds per frame
    const float jump_rate = 3.0f;    // speed at which jump_progress changes
    const float jump_range = 500.0f; // fixed jump height offset from takeoff
    const float head_cap = 800.0f;   // ceiling for player's head
    static float jump_progress = 0.0f; // 0 = on ground, 1 = apex
    static float jump_origin = 0.0f;   // BASE_HEIGHT at jump takeoff

    if (JUMP == JUMP_UP)
    {
        // Record the takeoff BASE_HEIGHT on the very first frame.
        if (jump_progress == 0.0f)
            jump_origin = BASE_HEIGHT;

        jump_progress += dt * jump_rate;
        if (jump_progress >= 1.0f)
        {
            jump_progress = 1.0f;
            JUMP = JUMP_DOWN;
        }
    }
    else if (JUMP == JUMP_DOWN)
    {
        jump_progress -= dt * jump_rate;
        if (jump_progress <= 0.0f)
        {
            jump_progress = 0.0f;
            JUMP = NO_JUMP;
        }
    }

    // Easing for a natural jump curve.
    float eased = 2 * jump_progress - jump_progress * jump_progress;
    // Fixed apex relative to takeoff.
    float apex = jump_origin + jump_range;
    // Interpolate: at eased==0, landing height = current BASE_HEIGHT;
    // at eased==1, height = apex.
    float new_height = (1 - eased) * BASE_HEIGHT + eased * apex;

    // If, while ascending, the computed height exceeds the head cap,
    // clamp it and force an immediate descent.
    if (JUMP == JUMP_UP && new_height >= head_cap)
    {
        new_height = head_cap;
        JUMP = JUMP_DOWN;
        jump_progress = 1.0f; // force the apex so descent begins right away
    }

    PLAYER_HEIGHT = new_height;
}



void crouch(t_game *game)
{
    if (JUMP != NO_JUMP)
        return;

    // --- Input Processing (unchanged) ---
    static Uint32 press_start = 0;
    static int processed = 0; // 0 = not processed, 1 = processed

    if (KEYS[C]) {
        if (press_start == 0) {
            press_start = SDL_GetTicks();
            processed = 0;
        } else {
            Uint32 elapsed = SDL_GetTicks() - press_start;
            if (elapsed >= 150 && !processed) {
                // Held long enough: switch to crawling.
                CROUCH = CRAWLING;
                processed = 1;
            }
        }
    } else {
        if (press_start != 0) {
            Uint32 elapsed = SDL_GetTicks() - press_start;
            if (elapsed < 150 && !processed) {
                // Tap: toggle between standing and crouching.
                if (CROUCH == STANDING)
                    CROUCH = CROUCHING;
                else if (CROUCH == CROUCHING)
                    CROUCH = STANDING;
            } else {
                if (CROUCH == CRAWLING)
                    CROUCH = STANDING;
            }
        }
        press_start = 0;
        processed = 0;
    }

    // --- Interpolated Height Adjustment with "Jump-like" Acceleration ---
    // We want the vertical movement (for both rising and falling) to mimic the jump's acceleration.
    // When the crouch state changes, we record the current BASE_HEIGHT as the origin,
    // and we interpolate from BASE_HEIGHT (landing height, which may update) to a fixed target.
    float dt = 1.0f / game->fps;
    const float rate = 3.0f;  // interpolation speed

    // We'll use a static progress variable: 0.0 = transition just started, 1.0 = transition complete.
    static float progress = 1.0f;  // assume starting at complete state
    // Record the BASE_HEIGHT at the moment of state change.
    static float origin = 0.0f;
    // Track the previous crouch state.
    static int prev_state = STANDING;  // assume initial state is STANDING

    int current_state = CROUCH;  // current crouch state from input

    // If the crouch state changed, reset the interpolation.
    if (current_state != prev_state) {
        progress = 0.0f;
        origin = BASE_HEIGHT;  // record the takeoff height for this transition
        prev_state = current_state;
    }

    // Determine the fixed target based on state:
    // When standing, target is BASE_HEIGHT (landing height).
    // When crouching, target is (origin - 200).
    // When crawling, target is (origin - 620).
    float target;
    if (current_state == STANDING)
        target = BASE_HEIGHT;
    else if (current_state == CROUCHING)
        target = origin - 200.0f;
    else if (current_state == CRAWLING)
        target = origin - 620.0f;
    else
        target = BASE_HEIGHT;

    // Only update progress if not complete.
    if (progress < 1.0f)
        progress += dt * rate;
    if (progress > 1.0f)
        progress = 1.0f;

    // Use an ease-out quadratic function for a natural feel.
    float eased = 2 * progress - progress * progress;
    // Interpolate from the current (dynamic) BASE_HEIGHT to the fixed target.
    float new_height = (1 - eased) * BASE_HEIGHT + eased * target;
    PLAYER_HEIGHT = new_height;
}

void	update_player_height(t_game *game)
{
	int	speed;
	int	base_height_goal;

	speed = (int)FPS * 0.5;
	if (MAPS[LEVEL][(int)PLAYER_Y][(int)PLAYER_X] == EMPTY)
		base_height_goal = 0;
	if (MAPS[LEVEL][(int)PLAYER_Y][(int)PLAYER_X] == WALL_0)
		base_height_goal = 200;
	if (MAPS[LEVEL][(int)PLAYER_Y][(int)PLAYER_X] == WALL_1)
		base_height_goal = 650;
	
	if (base_height_goal < BASE_HEIGHT)
	{
		BASE_HEIGHT -= speed;
		if (BASE_HEIGHT < base_height_goal)
			BASE_HEIGHT = base_height_goal;
	}
	if (base_height_goal > BASE_HEIGHT)
	{
		BASE_HEIGHT += speed;
		if (BASE_HEIGHT > base_height_goal)
			BASE_HEIGHT = base_height_goal;
	}
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
	if (MOUSE_Y)
		look_up_and_down_mouse(game, MOUSE_Y);
	if (JOYSTICK_ROT_X)
		rotate_player_joystick(game, JOYSTICK_ROT_X);
	if (fabs(JOYSTICK_ROT_Y) > 0.1)
		look_up_and_down_joystick(game, JOYSTICK_ROT_Y);
	if (fabs(JOYSTICK_Y) < 0.1 && fabs(JOYSTICK_X) < 0.1 && !KEYS[W] && !KEYS[S] && !KEYS[A] && !KEYS[D])
		MOVING = STILL;
	if (JUMP)
		jump(game);
	crouch(game);
	update_player_height(game);
}

void	update_entities(t_game *game)
{
	update_vector_grid(game);
	update_player(game);
}
