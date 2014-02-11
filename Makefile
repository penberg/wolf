CFLAGS  = $(shell pkg-config sdl2 --cflags)

CFLAGS += -Wall -O3 -Werror -std=c11 -lm -lGL -lGLU -D_GNU_SOURCE

LDFLAGS = $(shell pkg-config sdl2 --libs)

PROGRAM = wolf

$(PROGRAM):

clean:
	rm -f $(PROGRAM)
