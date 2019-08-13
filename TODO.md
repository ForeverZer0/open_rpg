# TODO

* Gaussian/Radial blurring?
* Variable width for text-outline
* Audio support (OpenAL or FMOD)
* Gamepad support?
* SpriteSheet class (optimized class for a sheets of sprites, pre-calculated VAO for each cell)
* Tiled support for Tilemaps and Tilesets, define structs, load on Ruby side with XML and JSON?
* Particle emitter base class
* Pathfinding
* Consolidate headers?
* Documentation
  * Depenedencies for each platform
  * Memory management
  * Transition videos

## Critical and Total Pain in the Ass

* Specialized viewport class with unsorted batch for weather, etc (or just custom particle emitter?)
* Implement changing shaders, or remove all the glUseProgram calls and only change state when needed, and change back
