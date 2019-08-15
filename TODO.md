# TODO

- [ ] User-defined quality when saving JPEG images, other options for formats, etc.
- [ ] Gaussian/Radial blurring?
- [ ] Variable width for text-outline?
- [ ] Audio support (OpenAL/libsndfile, FMOD, separate backends/gems?
- [ ] Gamepad support?
- [ ] SpriteSheet class (optimized class for a sheets of sprites, pre-calculated VAO for each cell)
- [ ] Particle emitter base class
- [ ] Pathfinding
- [ ] Documentation
  - [ ] Depenedencies for each platform
  - [ ] Memory management
  - [ ] Transition videos
- [ ] Redo vector/matrix classes, implement in Ruby
- [ ] Change vector/matrix classes to the GLSL types
- [ ] `RPGimage` uses `void*` for `font` field, can change to `RPGfont` now

## Critical and Total Pain in the Ass

- [ ] Map renderer
- [ ] Reorganize, things are messy after the structure changes
- [ ] Specialized viewport class with unsorted batch for weather, etc (or just custom particle emitter?)
- [ ] Implement changing shaders, or remove all the glUseProgram calls and only change state when needed, and change back
