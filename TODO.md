# TODO

- [ ] Variable width for text-outline?
- [ ] Audio support (OpenAL/libsndfile, FMOD, separate backends/gems?
- [ ] Gamepad support?
- [ ] Particle emitter base class
- [ ] Pathfinding
- [ ] Documentation
  - [ ] Depenedencies for each platform
  - [ ] Memory management
  - [ ] Transition videos
  - [ ] Examples everywhere....
- [ ] Redo vector/matrix classes, implement in Ruby
- [ ] Change vector/matrix classes to the GLSL types
- [ ] `RPGimage` uses `void*` for `font` field, can change to `RPGfont` now
- [ ] Use stb_truetype instead of FreeType2?

## Critical and Total Pain in the Ass

- [ ] Map renderer
- [ ] Reorganize, things are messy after the structure changes
- [ ] Specialized viewport class with unsorted batch for weather, etc (or just custom particle emitter?)
- [ ] Implement changing shaders, or remove all the glUseProgram calls and only change state when needed, and change back
