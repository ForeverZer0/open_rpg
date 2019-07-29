#include "game.h"

VALUE rb_cGame;

GLint game_width;
GLint game_height;

GLuint _program;
GLint _projection;
GLint _model;
GLint _color;
GLint _tone;
GLint _alpha;
GLint _flash;
GLint _depth;
GLint _screen;
GLint _screen_z;

void rpg_game_init(VALUE parent) {
    glfwSetErrorCallback(rpg_game_glfw_error);
    if (!glfwInit()) {
        rb_raise(rb_eRPGError, "failed to initialize GLFW");
    }

    rb_cGame = rb_define_class_under(parent, "Game", rb_cObject);
    rb_define_alloc_func(rb_cGame, rpg_game_allocate);

    rb_define_method(rb_cGame, "initialize", rpg_game_initialize, -1);
    rb_define_method(rb_cGame, "main", rpg_game_main, -1);
    rb_define_method(rb_cGame, "update", rpg_game_update, 0);

    rb_define_method(rb_cGame, "frame_rate", rpg_game_get_frame_rate, 0);
    rb_define_method(rb_cGame, "frame_rate=", rpg_game_set_frame_rate, 1);
    rb_define_method(rb_cGame, "frame_count", rpg_game_get_frame_count, 0);
    rb_define_method(rb_cGame, "frame_count=", rpg_game_set_frame_count, 1);
    rb_define_method(rb_cGame, "bg_color", rpg_game_get_bg_color, 0);
    rb_define_method(rb_cGame, "bg_color=", rpg_game_set_bg_color, 1);

    rb_define_method(rb_cGame, "dispose", rpg_game_dispose, 0);
    rb_define_method(rb_cGame, "disposed?", rpg_game_disposed_p, 0);
    rb_define_method(rb_cGame, "close", rpg_game_close, -1);
    rb_define_method(rb_cGame, "closing?", rpg_game_closing_p, 0);

    rb_define_method(rb_cGame, "window_size", rpg_game_window_size, 0);
    rb_define_method(rb_cGame, "window_width", rpg_game_window_width, 0);
    rb_define_method(rb_cGame, "window_height", rpg_game_window_height, 0);
    rb_define_method(rb_cGame, "resolution", rpg_game_resolution, 0);
    rb_define_method(rb_cGame, "width", rpg_game_width, 0);
    rb_define_method(rb_cGame, "height", rpg_game_height, 0);
    rb_define_method(rb_cGame, "viewport", rpg_game_viewport, 0);

    rb_define_singleton_method(rb_cGame, "default_options", rpg_game_default_options, 0);
}

ALLOC_FUNC(rpg_game_allocate, RPGgame)

void rpg_game_glfw_error(int code, const char *message) { rb_raise(rb_eRPGError, message); }

static VALUE rpg_game_window_size(VALUE self) {
    RPGgame *game = DATA_PTR(self);
    RPGsize *size = ALLOC(RPGsize);
    glfwGetFramebufferSize(game->window, &size->width, &size->height);
    return Data_Wrap_Struct(rb_cSize, NULL, RUBY_DEFAULT_FREE, size);
}

static VALUE rpg_game_window_width(VALUE self) {
    RPGgame *game = DATA_PTR(self);
    int width;
    glfwGetFramebufferSize(game->window, &width, NULL);
    return INT2NUM(width);
}

static VALUE rpg_game_window_height(VALUE self) {
    RPGgame *game = DATA_PTR(self);
    int height;
    glfwGetFramebufferSize(game->window, NULL, &height);
    return INT2NUM(height);
}

static VALUE rpg_game_resolution(VALUE self) {
    RPGgame *game = DATA_PTR(self);
    RPGsize *size = ALLOC(RPGsize);
    size->width = game_width;
    size->height = game_height;
    return Data_Wrap_Struct(rb_cSize, NULL, RUBY_DEFAULT_FREE, size);
}

static inline void rpg_game_set_resolution_inline(RPGgame *game, int width, int height) {
    check_dimensions(width, height);
    game_width = width;
    game_height = height;
    if (_program) {
        RPGmatrix4x4 ortho;
        rpg_mat4_create_ortho(&ortho, 0, game_width, game_height, 0, -1.0f, 1.0f);
        glUseProgram(_program);
        glUniformMatrix4fv(_projection, 1, GL_FALSE, (float *)&ortho);
    }
    int window_width, window_height;
    glfwGetFramebufferSize(game->window, &window_width, &window_height);
    rpg_game_window_resize(game->window, window_width, window_height);
}

static VALUE rpg_game_set_resolution(VALUE self, VALUE value) {
    RPGgame *game = DATA_PTR(self);
    RPGsize *size = DATA_PTR(value);
    rpg_game_set_resolution_inline(game, size->width, size->height);
    return value;
}

static VALUE rpg_game_width(VALUE self) { return INT2NUM(game_width); }

static VALUE rpg_game_height(VALUE self) { return INT2NUM(game_height); }

static VALUE rpg_game_update(VALUE self) { return Qnil; }

static VALUE rpg_game_get_frame_count(VALUE self) {
    RPGgame *game = DATA_PTR(self);
    return ULL2NUM(game->frame_count);
}

static VALUE rpg_game_set_frame_count(VALUE self, VALUE count) {
    RPGgame *game = DATA_PTR(self);
    game->frame_count = NUM2ULL(count);
    return count;
}

static VALUE rpg_game_get_frame_rate(VALUE self) {
    RPGgame *game = DATA_PTR(self);
    return DBL2NUM(game->rate);
}

static VALUE rpg_game_set_frame_rate(VALUE self, VALUE rate) {
    RPGgame *game = DATA_PTR(self);
    game->rate = NUM2DBL(rate);
    if (game->rate < 1.0) {
        game->rate = 1.0;
    }
    const double d = 1.0 / 60.0;
    game->tick = d * (60.0 / game->rate);
    return rate;
}

static VALUE rpg_game_get_bg_color(VALUE self) {
    RPGgame *game = DATA_PTR(self);
    RPGcolor *color = ALLOC(RPGcolor);
    memcpy(color, &game->bg_color, sizeof(RPGcolor));
    return Data_Wrap_Struct(rb_cColor, NULL, RUBY_DEFAULT_FREE, color);
}

static VALUE rpg_game_set_bg_color(VALUE self, VALUE value) {
    RPGgame *game = DATA_PTR(self);
    RPGcolor *color = DATA_PTR(value);
    memcpy(&game->bg_color, color, sizeof(RPGcolor));
    glClearColor(color->r, color->g, color->b, color->a);
    return value;
}

static inline void rpg_game_render(RPGgame *game) {
    glfwSwapBuffers(game->window);
    glClear(GL_COLOR_BUFFER_BIT);

    VALUE TEST = rb_gv_get("$TEST");

    if (TEST != Qnil) {
        RPGsprite *sprite = DATA_PTR(TEST);
        rpg_sprite_render(sprite);
    }
}

static VALUE rpg_game_main(int argc, VALUE *argv, VALUE self) {
    VALUE rate;
    rb_scan_args(argc, argv, "01", &rate);
    rpg_game_set_frame_rate(self, NIL_P(rate) ? DBL2NUM(30.0) : rate);

    RPGgame *game = DATA_PTR(self);
    double delta = glfwGetTime();
    ID update = rb_intern("update");

    while (!glfwWindowShouldClose(game->window)) {
        while (delta < glfwGetTime()) {
            game->frame_count++;
            rb_funcall(self, update, 0);
            rpg_input_update(rb_mInput);
            delta += game->tick;
        }
        rpg_game_render(game);
        glfwPollEvents();
    }
    return Qnil;
}

static VALUE rpg_game_dispose(VALUE self) {
    RPGgame *game = DATA_PTR(self);
    if (game->window) {
        glfwDestroyWindow(game->window);
        game->window = NULL;
    }
    return Qnil;
}

static VALUE rpg_game_disposed_p(VALUE self) {
    RPGgame *game = DATA_PTR(self);
    return game->window ? Qfalse : Qtrue;
}

static VALUE rpg_game_viewport(VALUE self) {
    RPGgame *game = DATA_PTR(self);
    RPGrect *rect = ALLOC(RPGrect);
    memcpy(rect, &game->viewport, sizeof(RPGrect));
    return Data_Wrap_Struct(rb_cRect, NULL, RUBY_DEFAULT_FREE, rect);
}

static VALUE rpg_game_default_options(VALUE klass) {
    VALUE opts = rb_hash_new();
    rb_hash_aset(opts, STR2SYM("fullscreen"), Qfalse);
    rb_hash_aset(opts, STR2SYM("decorated"), Qtrue);
    rb_hash_aset(opts, STR2SYM("resizable"), Qfalse);
    rb_hash_aset(opts, STR2SYM("vsync"), INT2NUM(-1));

    // TODO: Borderless fullscreen, internal resolution?, aspect lock
    return opts;
}

static VALUE rpg_game_close(int argc, VALUE *argv, VALUE self) {
    RPGgame *game = DATA_PTR(self);
    VALUE close = Qtrue;
    if (argc > 0) {
        rb_scan_args(argc, argv, "01", &close);
    }
    glfwSetWindowShouldClose(game->window, RTEST(close));
    return self;
}

static VALUE rpg_game_closing_p(VALUE self) {
    RPGgame *game = DATA_PTR(self);
    return glfwWindowShouldClose(game->window) ? Qtrue : Qfalse;
}

static VALUE rpg_game_initialize(int argc, VALUE *argv, VALUE self) {
    VALUE w, h, caption, options;
    rb_scan_args(argc, argv, "21:", &w, &h, &caption, &options);
    RPGgame *game = DATA_PTR(self);
    GLFWmonitor *monitor;

    // Width
    game_width = NUM2INT(w);
    game_height = NUM2INT(h);
    check_dimensions(game_width, game_height);

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Options
    const char *title = NIL_P(caption) ? NULL : StringValueCStr(caption);
    int swap = -1;
    if (NIL_P(options)) {
        glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        monitor = NULL;
    } else {
        VALUE v = rb_hash_aref(options, STR2SYM("decorated"));
        glfwWindowHint(GLFW_DECORATED, NIL_P(v) ? GL_TRUE : RTEST(v));

        v = rb_hash_aref(options, STR2SYM("resizable"));
        glfwWindowHint(GLFW_RESIZABLE, NIL_P(v) ? GL_FALSE : RTEST(v));

        monitor = RTEST(rb_hash_aref(options, STR2SYM("fullscreen"))) ? glfwGetPrimaryMonitor() : NULL;
        VALUE vsync = rb_hash_aref(options, STR2SYM("vsync"));
        if (FIXNUM_P(vsync)) {
            swap = NUM2INT(vsync);
        }
    }

    // Create context and set function loader
    game->window = glfwCreateWindow(game_width, game_height, title, monitor, NULL);
    if (!game->window) {
        rb_raise(rb_eRPGError, "failed to create graphics context, ensure OpenGL 3.3 is supported on the system");
    }

    glfwMakeContextCurrent(game->window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // Swap Interval (vsync)
    if (swap >= 0) {
        glfwSwapInterval(swap);
    }
    rpg_input_bind(game);
    glfwSetFramebufferSizeCallback(game->window, rpg_game_window_resize);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_BLEND);
    glfwSetWindowUserPointer(game->window, game);
    if (!_program) {
        _program = rpg_create_shader_program(STOCK_VERTEX_SHADER, STOCK_FRAGMENT_SHADER, NULL);
        _projection = glGetUniformLocation(_program, "projection");
        _model = glGetUniformLocation(_program, "model");
        _color = glGetUniformLocation(_program, "color");
        _tone = glGetUniformLocation(_program, "tone");
        _alpha = glGetUniformLocation(_program, "alpha");
        _flash = glGetUniformLocation(_program, "flash");
        _depth = glGetUniformLocation(_program, "z");
        _screen = glGetUniformLocation(_program, "screen");
        _screen_z = glGetUniformLocation(_program, "screen_z");
    }
    rpg_game_set_resolution_inline(game, game_width, game_height);

    if (rb_block_given_p()) {
        rb_yield(self);
        rpg_game_dispose(self);
    }

    return Qnil;
}

void rpg_game_window_resize(GLFWwindow *window, int window_width, int window_height) {
    RPGgame *game = glfwGetWindowUserPointer(window);

    game->ratio.x = (GLfloat)window_width / game_width;
    game->ratio.y = (GLfloat)window_height / game_height;
    float ratio = game->ratio.x < game->ratio.y ? game->ratio.x : game->ratio.y;

    // Calculate letterbox/pillar rendering coordinates as required
    game->viewport.width = (GLint)roundf(game_width * ratio);
    game->viewport.height = (GLint)roundf(game_height * ratio);
    game->viewport.x = (GLint)roundf((window_width - game_width * ratio) / 2);
    game->viewport.y = (GLint)roundf((window_height - game_height * ratio) / 2);

    glViewport(game->viewport.x, game->viewport.y, game->viewport.width, game->viewport.height);

    // Ensure the clipping area is also cleared
    glDisable(GL_SCISSOR_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_SCISSOR_TEST);
    glClearColor(game->bg_color.r, game->bg_color.g, game->bg_color.b, game->bg_color.a);
    glScissor(game->viewport.x, game->viewport.y, game->viewport.width, game->viewport.height);
}