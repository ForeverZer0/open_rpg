#include "graphics.h"

GLboolean frozen;
GLuint frame_count;
GLint frame_rate;
GLdouble tick;
GLint vsync;
GLint game_width;
GLint game_height;

// extern
VALUE rb_mGraphics;
GLFWwindow *game_window;
RPGbatch *game_batch;
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
    rb_define_singleton_method(rb_mGraphics, "update", rpg_graphics_update, 0);
    rb_define_singleton_method(rb_mGraphics, "frame_reset", rpg_graphics_frame_reset, 0);
    rb_define_singleton_method(rb_mGraphics, "transition", rpg_graphics_transition, -1);
    rb_define_singleton_method(rb_mGraphics, "bg_color", rpg_graphics_get_bg_color, 0);
    rb_define_singleton_method(rb_mGraphics, "bg_color=", rpg_graphics_set_bg_color, 1);
    rb_define_singleton_method(rb_mGraphics, "vsync", rpg_graphics_get_vsync, 0);
    rb_define_singleton_method(rb_mGraphics, "vsync=", rpg_graphics_set_vsync, 1);
    rb_define_singleton_method(rb_mGraphics, "destroy", rpg_graphics_destroy, 0);

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

void rpg_graphics_error(int code, const char *message) { 
    rb_raise(rb_eRPGError, message); 
}

static VALUE rpg_graphics_destroy(VALUE module) {
    frozen = GL_TRUE;
    if (game_batch) {
        if (game_batch->items) {
            xfree(game_batch->items);
        }
        xfree(game_batch);
        game_batch = NULL;
    }
    // TODO: Cleanup
    // glfwTerminate();
}

static VALUE rpg_graphics_get_vsync(VALUE module) {
    return INT2NUM(vsync);
}

static VALUE rpg_graphics_set_vsync(VALUE module, VALUE value) {
    vsync = imax(NUM2INT(value), -1);
    if (vsync >= 0) {
        glfwSwapInterval(vsync);
    }
    return value;
}

static VALUE rpg_graphics_width(VALUE module) {
    return INT2NUM(game_width);
}

static VALUE rpg_graphics_height(VALUE module) {
    return INT2NUM(game_height);
}

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

static VALUE rpg_graphics_get_frame_count(VALUE module) {
    return UINT2NUM(frame_count);
}

static VALUE rpg_graphics_set_frame_count(VALUE module, VALUE value) {
    frame_count = NUM2UINT(value);
    return value;
}

static VALUE rpg_graphics_get_frame_rate(VALUE module) {
    return INT2NUM(frame_rate);
}

static VALUE rpg_graphics_set_frame_rate(VALUE module, VALUE value) {
    frame_rate = clampi(NUM2INT(value), MIN_FRAME_RATE, MAX_FRAME_RATE);
    const double d = 1.0 / 60.0;
    tick = d * (60.0 / frame_rate);
    return value;
}

static VALUE rpg_graphics_freeze(VALUE module) {
    frozen = GL_TRUE;
    return Qnil;
}

static VALUE rpg_graphics_frozen_p(VALUE module) {
    return RB_BOOL(frozen);
}

static VALUE rpg_graphics_update(VALUE module) {
    // TODO: Placeholder
    return Qnil;
}

static VALUE rpg_graphics_frame_reset(VALUE module) {
    // TODO: Don't really need this...
    return Qnil;
}

static VALUE rpg_graphics_transition(int argc, VALUE *argv, VALUE module) {
    // TODO: Implement
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
    GLfloat ratio_x = (GLfloat) width / game_width;
    GLfloat ratio_y = (GLfloat) height / game_height;
    GLfloat ratio = ratio_x < ratio_y ? ratio_x : ratio_y;

    // Calculate letterbox/pillar rendering coordinates as required
    bounds.width = (GLint) roundf(game_width * ratio);
    bounds.height = (GLint) roundf(game_height * ratio);
    bounds.x = (GLint) roundf((width - game_width * ratio) / 2);
    bounds.y = (GLint) roundf((height - game_height * ratio) / 2);
    glViewport(bounds.x, bounds.y, bounds.width, bounds.height);

    // Ensure the clipping area is also cleared
    glDisable(GL_SCISSOR_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_SCISSOR_TEST);
    glClearColor(bg_color.r, bg_color.g, bg_color.b, bg_color.a);
    glScissor(bounds.x, bounds.y, bounds.width, bounds.height);
}

static inline void rpg_graphics_render(void) {
    if (frozen) {
        return;
    }
    glfwSwapBuffers(game_window);
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
            delta += tick;
        }
        rpg_graphics_render();
        glfwPollEvents();
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
        glUniformMatrix4fv(_projection, 1, GL_FALSE, (float*) &projection);
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
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    if (vsync >= 0) {
        glfwSwapInterval(vsync);
    }

    // Enable required OpenGL capabilities
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_BLEND);

    // Create shader program and cache uniform locations
    _program = rpg_create_shader_program(STOCK_VERTEX_SHADER, STOCK_FRAGMENT_SHADER, NULL);
    _projection = glGetUniformLocation(_program, "projection");
    _model = glGetUniformLocation(_program, "model");
    _color = glGetUniformLocation(_program, "color");
    _tone = glGetUniformLocation(_program, "tone");
    _alpha = glGetUniformLocation(_program, "alpha");
    _flash = glGetUniformLocation(_program, "flash");
    _hue = glGetUniformLocation(_program, "hue");

    // Create a shared vertex array for drawing a quad texture with two triangles
    glGenVertexArrays(1, &quad_vao);
    glGenBuffers(1, &quad_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
    float vertices[VERTICES_COUNT] = 
    {
        0.0f, 1.0f, 0.0f, 1.0f, 
        1.0f, 0.0f, 1.0f, 0.0f, 
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 1.0f, 
        1.0f, 1.0f, 1.0f, 1.0f, 
        1.0f, 0.0f, 1.0f, 0.0f
    };
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

    rpg_graphics_resolution(game_width, game_height);

    return Qnil;
}