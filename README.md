# Wolf

Wolf is a raycasting engine for 2.5D games, like Wolfenstein 3D and others from
the 1990s!

The engine uses OpenGL for rendering with GPUs but operates on 2D maps under
the hood.  The player is is thus not able to move up or down and walls always
have 90-degree angles between them.

The design of the engine is similar to the [iPhone port of Wolfenstein 3D](http://fabiensanglard.net/wolf3d/index.php).

![alt text](https://github.com/penberg/wolf/raw/master/Wolf.png "Wolf")

## Features

- 2D maps
- Raycasting for visibility determination
- OpenGL rendering pipeline
- Texture mapping
- Collision detection

## Usage

First install prerequisites:

```
$ sudo yum install SDL2-devel SDL2_image-devel
```

To build Wolf:

```
$ make
```

Then, to start up the demo:

```
$ ./wolf
```

## License

Copyright © 2014 Pekka Enberg

Wolf is distributed under the 2-clause BSD license.

The texture assets are taken from [Crystal Space](http://www.crystalspace3d.org/CreativeCommonsTextures/).
