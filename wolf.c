#include <stdbool.h>
#include <stdio.h>
#include <SDL.h>

enum {
	MAP_WIDTH	= 24,
	MAP_HEIGHT	= 24,
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

static void wolf_set_color(SDL_Renderer *renderer, int color)
{
	switch (color) {
        case 1: /* red */
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		break;
        case 2: /* green */
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
		break;
        case 3: /* blue */
		SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
		break;
        case 4: /* white */
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		break;
        default:
		SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
		break;
	}

}

static void wolf_raycast(SDL_Renderer *renderer, struct point2 *position, struct vector2 *direction)
{
	double map_x, map_y;
	int nr_rays = 640;
	double fov = 1.57; /* 90 degrees */
	double fov_step = fov / (double)nr_rays;

	for (double i = -nr_rays/2; i < nr_rays/2; i++) {
		struct vector2 ray_direction;

		ray_direction.x = direction->x * cos(fov_step*i) - direction->y * sin(fov_step*i);
		ray_direction.y = direction->x * sin(fov_step*i) + direction->y * cos(fov_step*i);

		map_x = position->x;
		map_y = position->y;

		for (;;) {
			if (map[(int)map_x][(int)map_y] != 0)
				break;

			map_x += ray_direction.x;
			map_y += ray_direction.y;
		}
		SDL_Rect rect;
		rect.x = (int)map_x * 8;
		rect.y = (int)map_y * 8;
		rect.w = 8;
		rect.h = 8;
		wolf_set_color(renderer, map[(int)map_x][(int)map_y]);
		SDL_RenderFillRect(renderer, &rect);
		SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
		SDL_RenderDrawLine(renderer, (int)position->x*8, (int)position->y*8, (int)map_x*8, (int)map_y*8);
	}
}

static void wolf_frame(SDL_Renderer *renderer, struct point2 *position, struct vector2 *direction)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);

	for (int y = 0; y < MAP_HEIGHT; y++) {
		for (int x = 0; x < MAP_WIDTH; x++) {
			if (!map[x][y])
				continue;
			SDL_Rect rect;
			rect.x = x * 8;
			rect.y = y * 8;
			rect.w = 8;
			rect.h = 8;
			SDL_RenderFillRect(renderer, &rect);
		}
	}

	wolf_raycast(renderer, position, direction);

	SDL_RenderPresent(renderer);
}

static struct point2 position = { .x = 22, .y =  12 };
static struct vector2 direction = { .x = -1, .y =  0 }; 
static bool running = true;
static float velocity;
static float angle;

static void wolf_update(void)
{
	struct vector2 new_direction;

	position.x = position.x + direction.x*velocity;
	position.y = position.y + direction.y*velocity;

	new_direction.x = direction.x * cos(angle) - direction.y * sin(angle);
	new_direction.y = direction.x * sin(angle) + direction.y * cos(angle);

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
				angle = 0.0;
				break;
			}
			case SDLK_LEFT: {
				angle = 0.0;
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
				velocity = 0.1;
				break;
			}
			case SDLK_DOWN: {
				velocity = -0.1;
				break;
			}
			case SDLK_RIGHT: {
				angle = 0.05;
				break;
			}
			case SDLK_LEFT: {
				angle = -0.05;
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

	SDL_Renderer *renderer;

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (!window) {
		fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
		return 1;
	}

	while (running) {
		wolf_input();
		wolf_update();

		wolf_frame(renderer, &position, &direction);

		SDL_Delay(10);
	}

	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}
