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

static void wolf_draw_wall(SDL_Renderer *renderer, struct point2 *position, int x, int y)
{
	wolf_set_color(map[x][y]);
	glBegin(GL_QUADS);
	glVertex3f(x+0.0f, 0.0f, y); // The bottom left corner  
	glVertex3f(x+0.0f, 1.0f, y); // The top left corner  
	glVertex3f(x+1.0f, 1.0f, y); // The top right corner  
	glVertex3f(x+1.0f, 0.0f, y); // The bottom right corner  
	glEnd();
	glBegin(GL_QUADS);
	glVertex3f(x+0.0f, 0.0f, y+1.0f); // The bottom left corner  
	glVertex3f(x+0.0f, 1.0f, y+1.0f); // The top left corner  
	glVertex3f(x+1.0f, 1.0f, y+1.0f); // The top right corner  
	glVertex3f(x+1.0f, 0.0f, y+1.0f); // The bottom right corner  
	glEnd();
	glBegin(GL_QUADS);
	glVertex3f(x, 0.0f, y+0.0f); // The bottom left corner  
	glVertex3f(x, 1.0f, y+0.0f); // The top left corner  
	glVertex3f(x, 1.0f, y+1.0f); // The top right corner  
	glVertex3f(x, 0.0f, y+1.0f); // The bottom right corner  
	glEnd();
	glBegin(GL_QUADS);
	glVertex3f(x+1.0f, 0.0f, y+0.0f); // The bottom left corner  
	glVertex3f(x+1.0f, 1.0f, y+0.0f); // The top left corner  
	glVertex3f(x+1.0f, 1.0f, y+1.0f); // The top right corner  
	glVertex3f(x+1.0f, 0.0f, y+1.0f); // The bottom right corner  
	glEnd();
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

static void wolf_raycast(SDL_Renderer *renderer, struct point2 *position, struct vector2 *direction, draw_wall_fn draw_wall)
{
	double map_x, map_y;
	int nr_rays = 640;
	double fov = degrees_to_radians(FOV) * 640.0/480.0;
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

		draw_wall(renderer, position, map_x, map_y);
	}
}

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

	wolf_raycast(renderer, position, direction, wolf_draw_wall);

	glDisable(GL_DEPTH_TEST);

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
	wolf_raycast(renderer, position, direction, wolf_minimap_draw_wall);
}

static struct point2 position   = { .x = 1, .y = 1 };
static struct vector2 direction = { .x = 1, .y = 0 }; 
static bool running = true;
static float velocity;
static float angle_step;
static float angle = 0.0;

static void wolf_update(unsigned int time_delta)
{
	struct vector2 new_direction;
	struct point2 new_position;

	new_position.x = position.x + direction.x*velocity * time_delta;
	new_position.y = position.y + direction.y*velocity * time_delta;

	if (!map[(int)new_position.x][(int)new_position.y]) {
		position.x = new_position.x;
		position.y = new_position.y;
	}

	angle += angle_step*time_delta;

	new_direction.x = direction.x * cos(angle_step*time_delta) - direction.y * sin(angle_step*time_delta);
	new_direction.y = direction.x * sin(angle_step*time_delta) + direction.y * cos(angle_step*time_delta);

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
