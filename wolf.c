#include <stdbool.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <stdio.h>
#include <math.h>
#include <SDL.h>

enum {
	MAP_WIDTH	= 24,
	MAP_HEIGHT	= 24,
	CELL_SIZE	= 4,
};

static char map[MAP_WIDTH][MAP_HEIGHT] =
{
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
	{1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
	{1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

struct point2 {
	double x;
	double y;
};

struct vector2 {
	double x;
	double y;
};

static double radians_to_degrees(double angle)
{
	return angle * 180.0/M_PI;
}

static double degrees_to_radians(double angle)
{
	return angle * M_PI/180.0;
}

static void wolf_set_color(int color)
{
	switch (color) {
        case 1: /* red */
		glColor3f(1.0, 0.0, 0.0);
		break;
        case 2: /* green */
		glColor3f(0.0, 1.0, 0.0);
		break;
        case 3: /* blue */
		glColor3f(0.0, 0.0, 1.0);
		break;
        case 4: /* white */
		glColor3f(1.0, 1.0, 1.0);
		break;
        default:
		glColor3f(1.0, 1.0, 0.0);
		break;
	}

}

typedef void (*draw_wall_fn)(SDL_Renderer *renderer, struct point2 *position, int x, int y);

static struct point2 wolf_cube_mesh[4][4] = {
	{
		{ 0.0, 0.0 },
		{ 0.0, 0.0 },
		{ 1.0, 0.0 },
		{ 1.0, 0.0 },
	},
	{
		{ 0.0, 1.0 },
		{ 0.0, 1.0 },
		{ 1.0, 1.0 },
		{ 1.0, 1.0 },
	},
	{
		{ 0.0, 0.0 },
		{ 0.0, 0.0 },
		{ 0.0, 1.0 },
		{ 0.0, 1.0 },
	},
	{
		{ 1.0, 0.0 },
		{ 1.0, 0.0 },
		{ 1.0, 1.0 },
		{ 1.0, 1.0 },
	},
};

static void wolf_draw_wall(SDL_Renderer *renderer, struct point2 *position, int x, int y)
{
	float wall_height = 1.5;

	wolf_set_color(map[x][y]);

	for (int i = 0; i < 4; i++) {
		glBegin(GL_QUADS);
		glVertex3f(x+wolf_cube_mesh[i][0].x, 0.0f,        y+wolf_cube_mesh[i][0].y);
		glVertex3f(x+wolf_cube_mesh[i][1].x, wall_height, y+wolf_cube_mesh[i][1].y);
		glVertex3f(x+wolf_cube_mesh[i][2].x, wall_height, y+wolf_cube_mesh[i][2].y);
		glVertex3f(x+wolf_cube_mesh[i][3].x, 0.0f,        y+wolf_cube_mesh[i][3].y);
		glEnd();
	}
}

static void wolf_minimap_draw_wall(SDL_Renderer *renderer, struct point2 *position, int map_x, int map_y)
{
	SDL_Rect rect;
	rect.x = map_x * CELL_SIZE;
	rect.y = map_y * CELL_SIZE;
	rect.w = CELL_SIZE;
	rect.h = CELL_SIZE;
	wolf_set_color(map[map_x][map_y]);
	SDL_RenderFillRect(renderer, &rect);

	glColor3f(0.5, 0.5, 0.5);
	SDL_RenderDrawLine(renderer, position->x*CELL_SIZE, position->y*CELL_SIZE, (int)map_x*CELL_SIZE, (int)map_y*CELL_SIZE);
}

#define FOV 90

static int sign(double x)
{
	if (x < 0) {
		return -1;
	} else if (x > 0) {
		return 1;
	} else {
		return 0;
	}
}

static void wolf_raycast(SDL_Renderer *renderer, struct point2 *position, double angle, draw_wall_fn draw_wall)
{
	double fov = degrees_to_radians(FOV) * 640.0/480.0;
	double ray_step = fov / (double) 640.0;

	for (int i = 0; i < 640; i++) {
		struct vector2 ray_direction;
		double next_x, next_y;
		double step_x, step_y;
		double max_x, max_y;
		double ray_angle;
		double dx, dy;
		int x, y;

		ray_angle = angle + -fov/2.0 + ray_step * i;
		ray_direction.x = cos(ray_angle);
		ray_direction.y = sin(ray_angle);

		x = position->x;
		y = position->y;

		step_x = sign(ray_direction.x);
		step_y = sign(ray_direction.y);

		next_x = x + (step_x > 0 ? 1 : 0);
		next_y = y + (step_y > 0 ? 1 : 0);

		max_x = (next_x - position->x) / ray_direction.x;
		max_y = (next_y - position->y) / ray_direction.y;

		if (isnan(max_x))
			max_x = INFINITY;
		if (isnan(max_y))
			max_y = INFINITY;

		dx = step_x / ray_direction.x;
		dy = step_y / ray_direction.y;

		if (isnan(dx))
			dx = INFINITY;
		if (isnan(dy))
			dy = INFINITY;

		for (;;) {
			if (map[x][y] != 0)
				break;

			if (max_x < max_y) {
				max_x += dx;
				x += step_x;
			} else {
				max_y += dy;
				y += step_y;
			}
		}

		draw_wall(renderer, position, x, y);
	}
}

static bool draw_minimap;

static void wolf_frame(SDL_Renderer *renderer, struct point2 *position, struct vector2 *direction, float angle)
{
	glClearColor(0,0,0,0); 
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); 

	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity();
	gluPerspective(FOV, 640.0/480.0, 0.1f, 64.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(90+radians_to_degrees(angle), 0, 1, 0);
	glTranslatef(-position->x, -0.3, -position->y);

	glEnable(GL_DEPTH_TEST);

	wolf_raycast(renderer, position, angle, wolf_draw_wall);

	glDisable(GL_DEPTH_TEST);

	if (draw_minimap) {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-640 + CELL_SIZE * MAP_WIDTH, CELL_SIZE * MAP_WIDTH, 480, 0, 0, 1);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glColor3f(0.5, 0.5, 0.5);

		for (int y = 0; y < MAP_HEIGHT; y++) {
			for (int x = 0; x < MAP_WIDTH; x++) {
				if (!map[x][y])
					continue;
				SDL_Rect rect;
				rect.x = x * CELL_SIZE;
				rect.y = y * CELL_SIZE;
				rect.w = CELL_SIZE;
				rect.h = CELL_SIZE;
				SDL_RenderFillRect(renderer, &rect);
			}
		}
		wolf_raycast(renderer, position, angle, wolf_minimap_draw_wall);
	}
}

static struct point2 position   = { .x = 1.5, .y = 2 };
static struct vector2 direction = { .x = 1, .y = 0 }; 
static bool running = true;
static float velocity;
static float angle_step;
static float angle = 0.0;

static bool wolf_try_to_move(struct point2 *pos)
{
	int x0, x1, y0, y1;
	double size = 0.2;

	x0 = pos->x - size;
	x1 = pos->x + size;
	y0 = pos->y - size;
	y1 = pos->y + size;

	for (int x = x0; x <= x1; x++) {
		for (int y = y0; y <= y1; y++) {
			if (map[x][y])
				return false;
		}
	}
	return true;
}

static struct point2 wolf_clip_move(struct point2 *old_pos, struct point2 *new_pos)
{
	struct point2 pos;

	pos.x = new_pos->x;
	pos.y = new_pos->y;

	if (wolf_try_to_move(&pos))
		return pos;

	pos.x = new_pos->x;
	pos.y = old_pos->y;

	if (wolf_try_to_move(&pos))
		return pos;

	pos.x = old_pos->x;
	pos.y = new_pos->y;

	if (wolf_try_to_move(&pos))
		return pos;

	return *old_pos;
}

static void wolf_update(unsigned int time_delta)
{
	struct vector2 new_direction;
	struct point2 new_position;

	new_position.x = position.x + direction.x*velocity * time_delta;
	new_position.y = position.y + direction.y*velocity * time_delta;

	position = wolf_clip_move(&position, &new_position);

	angle += angle_step*time_delta;

	new_direction.x = cos(angle);
	new_direction.y = sin(angle);

	direction.x = new_direction.x;
	direction.y = new_direction.y;
}

static void wolf_input(void)
{
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
	switch (event.type) {
		case SDL_KEYUP: {
			switch (event.key.keysym.sym) {
			case SDLK_UP: {
				velocity = 0.0;
				break;
			}
			case SDLK_DOWN: {
				velocity = 0.0;
				break;
			}
			case SDLK_RIGHT: {
				angle_step = 0.0;
				break;
			}
			case SDLK_LEFT: {
				angle_step = 0.0;
				break;
			}
			default:
				break;
			}
			break;
		}
		case SDL_KEYDOWN: {
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				running = false;
				break;
			case SDLK_UP: {
				velocity = 0.005;
				break;
			}
			case SDLK_DOWN: {
				velocity = -0.005;
				break;
			}
			case SDLK_RIGHT: {
				angle_step = 0.005;
				break;
			}
			case SDLK_LEFT: {
				angle_step = -0.005;
				break;
			}
			case 'm': {
				draw_minimap = !draw_minimap;
				break;
			}
			default:
				break;
			}
			break;
		}
		default:
			break;
		}
	}
}

int main(int argc, char* argv[])
{
	SDL_Window *window;

	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow(
		"Wolf",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		640,
		480,
		SDL_WINDOW_OPENGL
	);
	if (!window) {
		fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Renderer *renderer;

	renderer = SDL_CreateRenderer(window, -1,
				SDL_RENDERER_ACCELERATED |
				SDL_RENDERER_PRESENTVSYNC);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	glViewport(0, 0, (GLsizei) 640, (GLsizei) 480);

	unsigned int time_delta = 0;

	while (running) {
		unsigned int frame_start, frame_end;

		frame_start = SDL_GetTicks();

		wolf_input();

		wolf_update(time_delta);

		wolf_frame(renderer, &position, &direction, angle);

		SDL_RenderPresent(renderer);

		frame_end = SDL_GetTicks();

		time_delta = frame_end - frame_start;
	}

	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}
