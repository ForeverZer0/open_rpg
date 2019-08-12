# TODO

* Gaussian/Radial blurring?
* Drop shadow for text rendering
* Text outline (text is kinda ugly and doesn't stand out)
* Audio support (OpenAL or FMOD)
* Gamepad support
* SpriteSheet class (optimized class for a sheets of sprites, pre-calculated VAO for each cell)
* Tiled support for Tilemaps and Tilesets, define structs, load on Ruby side with XML and JSON?
* Particle emitter base class
* `bind` and `unbind` methods for image class for batch drawing
* Pathfinding

## Critical and Total Pain in the Ass

* Transitions don't account for screen resize (pillars/letterbox)
* Implement changing shaders, or remove all the glUseProgram calls and only change state when needed, and change back
