#include <SDL_image.h>
#include <stdbool.h>
#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <stdio.h>
#include <math.h>
#include <SDL.h>

#include "linmath.h"

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

static double degrees_to_radians(double angle)
{
	return angle * M_PI/180.0;
}

typedef void (*draw_wall_fn)(struct point2 *position, int x, int y);

static struct point2 wolf_cube_mesh[4][4] = {
	{
		{ 1.0, 0.0 },
		{ 1.0, 0.0 },
		{ 0.0, 0.0 },
		{ 0.0, 0.0 },
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
		{ 1.0, 1.0 },
		{ 1.0, 1.0 },
		{ 1.0, 0.0 },
		{ 1.0, 0.0 },
	},
};

static GLuint textures[2];

static void wolf_draw_floor(void)
{
	int x_step, y_step;

	x_step = 1;
	y_step = 1;

	glBindTexture(GL_TEXTURE_2D, textures[1]);

	glEnable(GL_TEXTURE_2D);

	for (int x = 0; x < MAP_WIDTH; x += x_step) {
		for (int y = 0; y < MAP_HEIGHT; y += y_step) {
			glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(x, 0.0f, y);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(x+x_step, 0.0f, y);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(x+x_step, 0.0f, y+y_step);

			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(x, 0.0f, y);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(x+x_step, 0.0f, y+y_step);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(x, 0.0f, y+y_step);
			glEnd();
		}
	}
}

mat4x4 model_view;

static void wolf_draw_wall(struct point2 *position, int x, int y)
{
	float wall_height = 1.5;

	glBindTexture(GL_TEXTURE_2D, textures[0]);

	glEnable(GL_TEXTURE_2D);

	mat4x4 mat;
	mat4x4_dup(mat, model_view);
	mat4x4_translate_in_place(mat, x, 0, y);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf((const GLfloat*) mat);

	for (int i = 0; i < 4; i++) {
		glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(wolf_cube_mesh[i][0].x, 0.0f,        wolf_cube_mesh[i][0].y);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(wolf_cube_mesh[i][1].x, wall_height, wolf_cube_mesh[i][1].y);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(wolf_cube_mesh[i][2].x, wall_height, wolf_cube_mesh[i][2].y);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(wolf_cube_mesh[i][0].x, 0.0f,        wolf_cube_mesh[i][0].y);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(wolf_cube_mesh[i][2].x, wall_height, wolf_cube_mesh[i][2].y);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(wolf_cube_mesh[i][3].x, 0.0f,        wolf_cube_mesh[i][3].y);
		glEnd();
	}
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

static void wolf_raycast(struct point2 *position, double angle, draw_wall_fn draw_wall)
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

		draw_wall(position, x, y);
	}
}

static void wolf_frame(struct point2 *position, struct vector2 *direction, float angle)
{
	glClearColor(0,0,0,0); 
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); 

	mat4x4 project;
	mat4x4_identity(project);
	mat4x4_perspective(project, degrees_to_radians(FOV), 640.0/480.0, 0.1f, 64.0f);

	mat4x4_identity(model_view);
	vec3 eye;
	eye[0] = position->x;
	eye[1] = 0.3;
	eye[2] = position->y;
	vec3 center;
	center[0] = position->x+direction->x;
	center[1] = 0.3;
	center[2] = position->y+direction->y;
	vec3 up;
	up[0] = 0.0;
	up[1] = 1.0;
	up[2] = 0.0;
	mat4x4_look_at(model_view, eye, center, up);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((const GLfloat*) project);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf((const GLfloat*) model_view);

	glEnable(GL_DEPTH_TEST);

	wolf_draw_floor();

	wolf_raycast(position, angle, wolf_draw_wall);
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

static void wolf_load_texture(unsigned int idx, const char *filename)
{
	SDL_Surface* surface;

	surface = IMG_Load(filename);
	if (!surface) {
		fprintf(stderr, "%s: error loading texture: %s\n",
			filename, SDL_GetError());
		return;
	}
	glGenTextures(1, &textures[idx]);
	glBindTexture(GL_TEXTURE_2D, textures[idx]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, surface->w, surface->h, 0, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);

	SDL_FreeSurface(surface);
}

int main(int argc, char* argv[])
{
	SDL_GLContext context;
	SDL_Window *window;

	SDL_Init(SDL_INIT_VIDEO);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

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

	context = SDL_GL_CreateContext(window);

	glewExperimental = GL_TRUE;
	glewInit();

	wolf_load_texture(0, "Assets/Textures/Wall.jpg");
	wolf_load_texture(1, "Assets/Textures/Floor.jpg");

	glViewport(0, 0, (GLsizei) 640, (GLsizei) 480);

	unsigned int time_delta = 0;

	while (running) {
		unsigned int frame_start, frame_end;

		frame_start = SDL_GetTicks();

		wolf_input();

		wolf_update(time_delta);

		wolf_frame(&position, &direction, angle);

		SDL_GL_SwapWindow(window);

		frame_end = SDL_GetTicks();

		time_delta = frame_end - frame_start;
	}

	SDL_GL_DeleteContext(context);

	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}
