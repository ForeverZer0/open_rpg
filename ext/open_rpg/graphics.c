#include "graphics.h"

GLuint transition_vbo;
GLuint transition_vao;

GLboolean frozen;
GLuint frame_count;
GLint frame_rate;
GLint vsync;
GLint game_width;
GLint game_height;
GLfloat game_ratio_x;
GLfloat game_ratio_y;
GLint screen_width;
GLint screen_height;

// extern
VALUE rb_mGraphics;
GLFWwindow *game_window;
RPGbatch *game_batch;
GLdouble game_tick;
RPGcolor bg_color;
GLuint quad_vao;
GLuint quad_vbo;
RPGmatrix4x4 projection;
RPGrect bounds;

GLuint _program;
GLint _projection;
GLint _model;
GLint _color;
GLint _tone;
GLint _alpha;
GLint _flash;
GLint _hue;

void rpg_graphics_init(VALUE parent) {
    rb_mGraphics = rb_define_module_under(parent, "Graphics");
    rb_define_singleton_method(rb_mGame, "main", rpg_game_main, -1);

    rb_define_singleton_method(rb_mGraphics, "create", rpg_graphics_create, -1);
    rb_define_singleton_method(rb_mGraphics, "width", rpg_graphics_width, 0);
    rb_define_singleton_method(rb_mGraphics, "height", rpg_graphics_height, 0);
    rb_define_singleton_method(rb_mGraphics, "size", rpg_graphics_get_size, 0);
    rb_define_singleton_method(rb_mGraphics, "size=", rpg_graphics_set_size, 1);
    rb_define_singleton_method(rb_mGraphics, "frame_count", rpg_graphics_get_frame_count, 0);
    rb_define_singleton_method(rb_mGraphics, "frame_count=", rpg_graphics_set_frame_count, 1);
    rb_define_singleton_method(rb_mGraphics, "frame_rate", rpg_graphics_get_frame_rate, 0);
    rb_define_singleton_method(rb_mGraphics, "frame_rate=", rpg_graphics_set_frame_rate, 1);
    rb_define_singleton_method(rb_mGraphics, "freeze", rpg_graphics_freeze, 0);
    rb_define_singleton_method(rb_mGraphics, "frozen?", rpg_graphics_frozen_p, 0);
    rb_define_singleton_method(rb_mGraphics, "transition", rpg_graphics_transition, -1);
    rb_define_singleton_method(rb_mGraphics, "background", rpg_graphics_get_bg_color, 0);
    rb_define_singleton_method(rb_mGraphics, "background=", rpg_graphics_set_bg_color, 1);
    rb_define_singleton_method(rb_mGraphics, "vsync", rpg_graphics_get_vsync, 0);
    rb_define_singleton_method(rb_mGraphics, "vsync=", rpg_graphics_set_vsync, 1);
    rb_define_singleton_method(rb_mGraphics, "destroy", rpg_graphics_destroy, 0);
    rb_define_singleton_method(rb_mGraphics, "capture", rpg_graphics_capture, 0);
    rb_define_singleton_method(rb_mGraphics, "transition", rpg_graphics_transition, -1);

    transition_vao = 0;

    game_window = NULL;
    frame_rate = DEFAULT_FRAME_RATE;
    frame_count = 0;
    frozen = GL_FALSE;
    vsync = -1;
    memset(&bg_color, 0, sizeof(RPGcolor));
    memset(&projection, 0, sizeof(RPGmatrix4x4));
    game_batch = ALLOC(RPGbatch);
    rpg_batch_init(game_batch);
}

static inline void rpg_graphics_render(void) {
    if (frozen) {
        return;
    }
    glClear(GL_COLOR_BUFFER_BIT);
    if (game_batch) {

        if (game_batch->updated) {
            rpg_batch_sort(game_batch, 0, game_batch->total - 1);
        }
        RPGrenderable *obj;
        int count = rpg_batch_total(game_batch);
        for (int i = 0; i < count; i++) {
            obj = game_batch->items[i];
            obj->render(obj);
        }
    }
}

void rpg_graphics_error(int code, const char *message) { rb_raise(rb_eRPGError, message); }

static VALUE rpg_graphics_destroy(VALUE module) {
    frozen = GL_TRUE;
    if (game_batch) {
        if (game_batch->items) {
            xfree(game_batch->items);
        }
        xfree(game_batch);
        game_batch = NULL;
    }
    if (quad_vao) {
        glDeleteVertexArrays(1, &quad_vao);
    }
    if (quad_vbo) {
        glDeleteBuffers(1, &quad_vbo);
    }
    if (_program) {
        glDeleteProgram(_program);
    }
    glfwTerminate();
}

static VALUE rpg_graphics_get_vsync(VALUE module) { return INT2NUM(vsync); }

static VALUE rpg_graphics_set_vsync(VALUE module, VALUE value) {
    vsync = imax(NUM2INT(value), -1);
    if (vsync >= 0) {
        glfwSwapInterval(vsync);
    }
    return value;
}

static VALUE rpg_graphics_width(VALUE module) { return INT2NUM(game_width); }

static VALUE rpg_graphics_height(VALUE module) { return INT2NUM(game_height); }

static VALUE rpg_graphics_get_size(VALUE module) {
    RPGsize *size = ALLOC(RPGsize);
    size->width = game_width;
    size->height = game_height;
    return Data_Wrap_Struct(rb_cSize, NULL, RUBY_DEFAULT_FREE, size);
}

static VALUE rpg_graphics_set_size(VALUE module, VALUE value) {
    RPGsize *size = DATA_PTR(value);
    rpg_graphics_resolution(size->width, size->height);
    return value;
}

static VALUE rpg_graphics_get_frame_count(VALUE module) { return UINT2NUM(frame_count); }

static VALUE rpg_graphics_set_frame_count(VALUE module, VALUE value) {
    frame_count = NUM2UINT(value);
    return value;
}

static VALUE rpg_graphics_get_frame_rate(VALUE module) { return INT2NUM(frame_rate); }

static VALUE rpg_graphics_set_frame_rate(VALUE module, VALUE value) {
    frame_rate = clampi(NUM2INT(value), MIN_FRAME_RATE, MAX_FRAME_RATE);
    const double d = 1.0 / 60.0;
    game_tick = d * (60.0 / frame_rate);
    return value;
}

static VALUE rpg_graphics_freeze(VALUE module) {
    frozen = GL_TRUE;
    return Qnil;
}

static VALUE rpg_graphics_frozen_p(VALUE module) { return RB_BOOL(frozen); }

static VALUE rpg_graphics_transition(int argc, VALUE *argv, VALUE module) {
    // Parse arguments
    rb_need_block();
    VALUE shader, frames;
    rb_scan_args(argc, argv, "02", &shader, &frames);

    // Return instantly if no shader is given number of frames is 0
    int f = NUM2INT(frames);
    if (NIL_P(shader) || f < 1) {
        rb_yield(Qnil);
        return Qnil;
    }

    // Create a VAO to use for transitions
    if (transition_vao == 0) {
        glGenVertexArrays(1, &transition_vao);
        glGenBuffers(1, &transition_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, transition_vbo);
        float vertices[VERTICES_COUNT] = {-1.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f,
                                          -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 0.0f};
        glBufferData(GL_ARRAY_BUFFER, VERTICES_SIZE, vertices, GL_STATIC_DRAW);
        glBindVertexArray(transition_vao);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, VERTICES_STRIDE, NULL);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    // Take copy of current screen
    RPGimage *from = rpg_graphics_snap();
    // Yield control back to Ruby to change scene, etc, then take another copy of screen
    rb_yield(Qnil);
    glClear(GL_COLOR_BUFFER_BIT);
    rpg_graphics_render();
    RPGimage *to = rpg_graphics_snap();

    // Bind the shader and set the locations to recieve the from/to textures
    RPGshader *s = DATA_PTR(shader);
    glUseProgram(s->program);
    GLint progress = glGetUniformLocation(s->program, "progress");
    glUniform1i(glGetUniformLocation(s->program, "from"), 0);
    glUniform1i(glGetUniformLocation(s->program, "to"), 1);

    //////////////
    // FIXME: Set in Ruby
    glUniform1f(glGetUniformLocation(s->program, "ratio"), (GLfloat)game_width / game_height);
    glUniform2f(glGetUniformLocation(s->program, "center"), 0.5f, 0.5f);

    ////////////

    // Bind the "to" and "from" textures to the shader
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, from->texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, to->texture);

    // Clear the current framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(bg_color.r, bg_color.g, bg_color.b, bg_color.a);
    glClear(GL_COLOR_BUFFER_BIT);

    // Get time, and calculate length of transition
    double done = f * game_tick;
    GLdouble time = glfwGetTime();
    GLdouble max = time + (f * game_tick);
    glBindVertexArray(transition_vao);

    // Loop through the defined amount of time, updating the "progress" uniform each draw
    while (time < max && !glfwWindowShouldClose(game_window)) {
        glUniform1f(progress, (GLfloat)(1.0 - ((max - time) / done)));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glfwPollEvents();
        glfwSwapBuffers(game_window);
        time = glfwGetTime();
    }

    // Unbind the textures
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    frame_count += (GLuint)f;

    return Qnil;
}

static VALUE rpg_graphics_get_bg_color(VALUE module) {
    RPGcolor *color = ALLOC(RPGcolor);
    memcpy(color, &bg_color, sizeof(RPGcolor));
    return Data_Wrap_Struct(rb_cColor, NULL, RUBY_DEFAULT_FREE, color);
}

static VALUE rpg_graphics_set_bg_color(VALUE module, VALUE value) {
    if (!RB_IS_A(value, rb_cColor)) {
        rb_raise(rb_eTypeError, "%s is not a color", rb_class2name(CLASS_OF(value)));
    }
    RPGcolor *color = DATA_PTR(value);
    memcpy(&bg_color, color, sizeof(RPGcolor));
    glClearColor(color->r, color->g, color->b, color->a);
    return value;
}

void rpg_graphics_buffer_resize(GLFWwindow *window, int width, int height) {
    screen_width = width;
    screen_height = height;
    game_ratio_x = (GLfloat)width / game_width;
    game_ratio_y = (GLfloat)height / game_height;
    GLfloat ratio = game_ratio_x < game_ratio_y ? game_ratio_x : game_ratio_y;

    // Calculate letterbox/pillar rendering coordinates as required
    bounds.width = (GLint)roundf(game_width * ratio);
    bounds.height = (GLint)roundf(game_height * ratio);
    bounds.x = (GLint)roundf((width - game_width * ratio) / 2);
    bounds.y = (GLint)roundf((height - game_height * ratio) / 2);
    glViewport(bounds.x, bounds.y, bounds.width, bounds.height);

    // Ensure the clipping area is also cleared
    glDisable(GL_SCISSOR_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_SCISSOR_TEST);
    glClearColor(bg_color.r, bg_color.g, bg_color.b, bg_color.a);
    glScissor(bounds.x, bounds.y, bounds.width, bounds.height);
}

static VALUE rpg_game_main(int argc, VALUE *argv, VALUE self) {
    VALUE rate;
    rb_scan_args(argc, argv, "01", &rate);
    rpg_graphics_set_frame_rate(self, argc == 0 ? INT2NUM(DEFAULT_FRAME_RATE) : rate);

    double delta = glfwGetTime();
    ID update = rb_intern("update");

    while (!glfwWindowShouldClose(game_window)) {
        while (delta < glfwGetTime()) {
            frame_count++;
            rb_funcall(rb_mGame, update, 0);
            rpg_input_update(rb_mInput);
            delta += game_tick;
        }
        rpg_graphics_render();
        glfwPollEvents();
        glfwSwapBuffers(game_window);
    }
    return Qnil;
}

void rpg_graphics_resolution(int width, int height) {
    check_dimensions(width, height);
    game_width = width;
    game_height = height;
    if (_program) {
        rpg_mat4_create_ortho(&projection, 0, game_width, game_height, 0, -1.0f, 1.0f);
        glUseProgram(_program);
        glUniformMatrix4fv(_projection, 1, GL_FALSE, (float *)&projection);
    }
    int window_width, window_height;
    glfwGetFramebufferSize(game_window, &window_width, &window_height);
    rpg_graphics_buffer_resize(game_window, window_width, window_height);
}

static VALUE rpg_graphics_create(int argc, VALUE *argv, VALUE module) {
    VALUE w, h, caption, options;
    rb_scan_args(argc, argv, "21:", &w, &h, &caption, &options);
    if (game_window) {
        rb_raise(rb_eRPGError, "graphics context already extists on calling thread");
    }
    if (!glfwInit()) {
        rb_raise(rb_eRPGError, "failed to initialize GLFW");
    }
    glfwSetErrorCallback(rpg_graphics_error);
    GLFWmonitor *monitor;
    const char *title = RTEST(caption) ? StringValueCStr(caption) : NULL;
    game_width = NUM2INT(w);
    game_height = NUM2INT(h);
    check_dimensions(game_width, game_height);

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    if (NIL_P(options)) {
        glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        monitor = NULL;
    } else {

        VALUE opt = rb_hash_aref(options, STR2SYM("decorated"));
        glfwWindowHint(GLFW_DECORATED, NIL_P(opt) ? GL_TRUE : RTEST(opt));

        opt = rb_hash_aref(options, STR2SYM("resizable"));
        glfwWindowHint(GLFW_RESIZABLE, NIL_P(opt) ? GL_FALSE : RTEST(opt));

        opt = rb_hash_aref(options, STR2SYM("fullscreen"));
        monitor = RTEST(opt) ? glfwGetPrimaryMonitor() : NULL;

        opt = rb_hash_aref(options, STR2SYM("vsync"));
        vsync = NIL_P(opt) ? -1 : imax(NUM2INT(opt), -1);
    }

    game_window = glfwCreateWindow(game_width, game_height, title, monitor, NULL);
    if (!game_window) {
        rb_raise(rb_eRPGError, "failed to create graphics context, ensure OpenGL 3.3 is supported on the system");
    }

    // Make OpenGL context current and import OpenGL function pointers
    glfwMakeContextCurrent(game_window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    if (vsync >= 0) {
        glfwSwapInterval(vsync);
    }

    // Enable required OpenGL capabilities
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_BLEND);

    char *vert_path = xmalloc(256);
    char *frag_path = xmalloc(256);
    sprintf(vert_path, "%s/%s", RPG_SHADERS, STOCK_VERTEX_SHADER);
    sprintf(frag_path, "%s/%s", RPG_SHADERS, STOCK_FRAGMENT_SHADER);

    // Create shader program and cache uniform locations
    _program = rpg_create_shader_program(vert_path, frag_path, NULL);
    _projection = glGetUniformLocation(_program, "projection");
    _model = glGetUniformLocation(_program, "model");
    _color = glGetUniformLocation(_program, "color");
    _tone = glGetUniformLocation(_program, "tone");
    _alpha = glGetUniformLocation(_program, "alpha");
    _flash = glGetUniformLocation(_program, "flash");
    _hue = glGetUniformLocation(_program, "hue");

    xfree(vert_path);
    xfree(frag_path);

    // Create a shared vertex array for drawing a quad texture with two triangles
    glGenVertexArrays(1, &quad_vao);
    glGenBuffers(1, &quad_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
    float vertices[VERTICES_COUNT] = {0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                      0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f};
    glBufferData(GL_ARRAY_BUFFER, VERTICES_SIZE, vertices, GL_STATIC_DRAW);
    glBindVertexArray(quad_vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, VERTICES_STRIDE, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Bind callbacks
    glfwSetFramebufferSizeCallback(game_window, rpg_graphics_buffer_resize);
    glfwSetKeyCallback(game_window, rpg_input_key_cb);
    glfwSetMouseButtonCallback(game_window, rpg_input_mouse_cb);
    glfwSetScrollCallback(game_window, rpg_input_mouse_scroll_cb);

    rpg_graphics_resolution(game_width, game_height);

    return Qnil;
}

static RPGimage *rpg_graphics_snap(void) {
    RPGimage *img = ALLOC(RPGimage);

    // Create texture the same size as the internal resolution
    glGenTextures(1, &img->texture);
    glBindTexture(GL_TEXTURE_2D, img->texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, game_width, game_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Create FBO and bind as the draw buffer
    glGenFramebuffers(1, &img->fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, img->fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, img->texture, 0);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    // Bind the primary buffer as the read buffer, and blit
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBlitFramebuffer(bounds.x, bounds.y, bounds.width + bounds.x, bounds.height + bounds.y, 0, 0, game_width, game_height,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // Rebind primary FBO and return the created image
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    return img;
}

static VALUE rpg_graphics_capture(VALUE module) {
    RPGimage *img = rpg_graphics_snap();
    return Data_Wrap_Struct(rb_cImage, NULL, NULL, img);
}