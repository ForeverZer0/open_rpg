#include "./open_rpg.h"
#include "./internal.h"

VALUE rb_mOpenRPG;
VALUE rb_mGame;
VALUE rb_eRPGError;

GLuint transition_vbo;
GLuint transition_vao;

GLuint frame_count;
GLint frame_rate;
GLint vsync;
GLint rpgWIDTH;
GLint rpgHEIGHT;
GLfloat rpgRATIO_X;
GLfloat rpgRATIO_Y;

// extern
RPGwindow *rpgGAME_WINDOW;
RPGbatch *rpgRENDER_BATCH;
GLdouble rpgTICK;
RPGcolor rpgBACK_COLOR;
RPGmat4 rpgPROJECTION;
RPGrect rpgBOUNDS;

GLuint rpgPROGRAM;
GLint rpgUNIFORM_PROJECTION;
GLint rpgUNIFORM_MODEL;
GLint rpgUNIFORM_COLOR;
GLint rpgUNIFORM_TONE;
GLint rpgUNIFORM_ALPHA;
GLint rpgUNIFORM_FLASH;
GLint rpgUNIFORM_HUE;

static VALUE rpg_empty_method(VALUE obj) {
    return obj;
}

void rpg_buffer_resize(RPGwindow *window, int width, int height) {
    rpgRATIO_X = (GLfloat)width / rpgWIDTH;
    rpgRATIO_Y = (GLfloat)height / rpgHEIGHT;
    GLfloat ratio = rpgRATIO_X < rpgRATIO_Y ? rpgRATIO_X : rpgRATIO_Y;

    // Calculate letterbox/pillar rendering coordinates as required
    rpgBOUNDS.width = (GLint)roundf(rpgWIDTH * ratio);
    rpgBOUNDS.height = (GLint)roundf(rpgHEIGHT * ratio);
    rpgBOUNDS.x = (GLint)roundf((width - rpgWIDTH * ratio) / 2);
    rpgBOUNDS.y = (GLint)roundf((height - rpgHEIGHT * ratio) / 2);
    glViewport(rpgBOUNDS.x, rpgBOUNDS.y, rpgBOUNDS.width, rpgBOUNDS.height);

    // Ensure the clipping area is also cleared
    glDisable(GL_SCISSOR_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_SCISSOR_TEST);
    RPG_RESET_BACK_COLOR();
    glScissor(rpgBOUNDS.x, rpgBOUNDS.y, rpgBOUNDS.width, rpgBOUNDS.height);
}

void rpg_resolution(int width, int height) {
    check_dimensions(width, height);
    rpgWIDTH = width;
    rpgHEIGHT = height;
    if (rpgPROGRAM) {
        MAT4_ORTHO(rpgPROJECTION, 0.0f, rpgWIDTH, 0.0f, rpgHEIGHT, -1.0f, 1.0f);
        glUseProgram(rpgPROGRAM);
        glUniformMatrix4fv(rpgUNIFORM_PROJECTION, 1, GL_FALSE, (float *)&rpgPROJECTION);
    }
    int window_width, window_height;
    glfwGetFramebufferSize(rpgGAME_WINDOW, &window_width, &window_height);
    rpg_buffer_resize(rpgGAME_WINDOW, window_width, window_height);
}

RPGimage *rpg_snapshot(void) {
    RPGimage *img = ALLOC(RPGimage);
    img->width = rpgWIDTH;
    img->height = rpgHEIGHT;
    
    glDisable(GL_SCISSOR_TEST);

    // Create texture the same size as the internal resolution
    glGenTextures(1, &img->texture);
    glBindTexture(GL_TEXTURE_2D, img->texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rpgWIDTH, rpgHEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Create FBO and bind as the draw buffer
    glGenFramebuffers(1, &img->fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, img->fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, img->texture, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    // Bind the primary buffer as the read buffer, and blit
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBlitFramebuffer(rpgBOUNDS.x, rpgBOUNDS.y, rpgBOUNDS.width + rpgBOUNDS.x, rpgBOUNDS.height + rpgBOUNDS.y, 0, 0, rpgWIDTH, rpgHEIGHT,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // Rebind primary FBO and return the created image
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glEnable(GL_SCISSOR_TEST);
    return img;
}



void rpg_render(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    if (rpgRENDER_BATCH) {

        if (rpgRENDER_BATCH->updated) {
            rpg_batch_sort(rpgRENDER_BATCH, 0, rpgRENDER_BATCH->total - 1);
        }
        RPGrenderable *obj;
        int count = rpg_batch_total(rpgRENDER_BATCH);
        for (int i = 0; i < count; i++) {
            obj = rpgRENDER_BATCH->items[i];
            obj->render(obj);
        }
    }
}

void rpg_error_cb(int code, const char *message) { rb_raise(rb_eRPGError, message); }

static VALUE rpg_destroy(VALUE module) {
    if (rpgRENDER_BATCH) {
        if (rpgRENDER_BATCH->items) {
            RPG_FREE(rpgRENDER_BATCH->items);
        }
        RPG_FREE(rpgRENDER_BATCH);
        rpgRENDER_BATCH = NULL;
    }
    if (rpgPROGRAM) {
        glDeleteProgram(rpgPROGRAM);
    }
    glfwTerminate();
}

static VALUE rpg_get_vsync(VALUE module) { return INT2NUM(vsync); }

static VALUE rpg_set_vsync(VALUE module, VALUE value) {
    vsync = imax(NUM2INT(value), -1);
    if (vsync >= 0) {
        glfwSwapInterval(vsync);
    }
    return value;
}

static VALUE rpg_game_width(VALUE module) { return INT2NUM(rpgWIDTH); }

static VALUE rpg_game_height(VALUE module) { return INT2NUM(rpgHEIGHT); }

static VALUE rpg_game_get_size(VALUE module) {
    RPGsize *size = ALLOC(RPGsize);
    size->width = rpgWIDTH;
    size->height = rpgHEIGHT;
    return Data_Wrap_Struct(rb_cSize, NULL, RUBY_DEFAULT_FREE, size);
}

static VALUE rpg_game_set_size(VALUE module, VALUE value) {
    RPGsize *size = DATA_PTR(value);
    rpg_resolution(size->width, size->height);
    return value;
}

static VALUE rpg_get_frame_count(VALUE module) { return UINT2NUM(frame_count); }

static VALUE rpg_set_frame_count(VALUE module, VALUE value) {
    frame_count = NUM2UINT(value);
    return value;
}

static VALUE rpg_get_frame_rate(VALUE module) { return INT2NUM(frame_rate); }

static VALUE rpg_set_frame_rate(VALUE module, VALUE value) {
    frame_rate = clampi(NUM2INT(value), MIN_FRAME_RATE, MAX_FRAME_RATE);
    // const double d = 1.0 / 60.0;
    // rpgTICK = d * (60.0 / frame_rate);
    rpgTICK = 1.0 / frame_rate;
    return value;
}

static VALUE rpg_transition(int argc, VALUE *argv, VALUE module) {
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
        float vertices[VERTICES_COUNT] = {
            -1.0f,  1.0f,   0.0f, 1.0f, 
             1.0f, -1.0f,   1.0f, 0.0f, 
            -1.0f, -1.0f,   0.0f, 0.0f,
            -1.0f,  1.0f,   0.0f, 1.0f, 
             1.0f,  1.0f,   1.0f, 1.0f, 
             1.0f, -1.0f,   1.0f, 0.0f
        };

        glBufferData(GL_ARRAY_BUFFER, VERTICES_SIZE, vertices, GL_STATIC_DRAW);
        glBindVertexArray(transition_vao);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, VERTICES_STRIDE, NULL);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    RPGshader *s = DATA_PTR(shader);

    // Take copy of current screen
    // glClear(GL_COLOR_BUFFER_BIT);
    // rpg_render();
    RPGimage *from = rpg_snapshot();
    
    // Enable transition shader and yield control back to Ruby to change scene, set uniforms, etc,
    glUseProgram(s->program);
    rb_yield(shader);

    // Take copy of the target screen to transition to
    glClear(GL_COLOR_BUFFER_BIT);
    rpg_render();
    RPGimage *to = rpg_snapshot();

    // Copy front buffer (current frame) to the back buffer (currently has target frame drawn on it).
    // If not done, the first buffer swap will show the final frame for a single render, causing a flicker.
    int w, h;
    glfwGetFramebufferSize(rpgGAME_WINDOW, &w, &h);
    glReadBuffer(GL_FRONT);
    glDrawBuffer(GL_BACK);
    glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glReadBuffer(GL_BACK);

    // Bind the shader and set the locations to recieve the from/to textures
    glUseProgram(s->program);
    GLint progress = glGetUniformLocation(s->program, "progress");
    glUniform1i(glGetUniformLocation(s->program, "from"), 0);
    glUniform1i(glGetUniformLocation(s->program, "to"), 1);

    // Bind the "to" and "from" textures to the shader
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, from->texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, to->texture);

    // Get time, and calculate length of transition
    glBindVertexArray(transition_vao);
    double done = f * rpgTICK;
    GLdouble time = glfwGetTime();
    GLdouble max = time + (f * rpgTICK);
    float percent;

    // Loop through the defined amount of time, updating the "progress" uniform each draw
    while (time < max && !glfwWindowShouldClose(rpgGAME_WINDOW)) {
        percent = clampf((GLfloat)(1.0 - ((max - time) / done)), 0.0f, 1.0f);
        glUniform1f(progress, percent);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glfwPollEvents();
        glfwSwapBuffers(rpgGAME_WINDOW);
        time = glfwGetTime();
    }
    
    // Unbind the textures
    RPG_RESET_BACK_COLOR();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    frame_count += (GLuint)f;

    // Cleanup
    rpg_image_free(from);
    rpg_image_free(to);

    return Qnil;
}

static VALUE rpg_get_bg_color(VALUE module) {
    RPGcolor *color = ALLOC(RPGcolor);
    memcpy(color, &rpgBACK_COLOR, sizeof(RPGcolor));
    return Data_Wrap_Struct(rb_cColor, NULL, RUBY_DEFAULT_FREE, color);
}

static VALUE rpg_set_bg_color(VALUE module, VALUE value) {
    if (!RB_IS_A(value, rb_cColor)) {
        rb_raise(rb_eTypeError, "%s is not a color", rb_class2name(CLASS_OF(value)));
    }
    RPGcolor *color = DATA_PTR(value);
    memcpy(&rpgBACK_COLOR, color, sizeof(RPGcolor));
    glClearColor(color->r, color->g, color->b, color->a);
    return value;
}

static VALUE rpg_game_main(int argc, VALUE *argv, VALUE self) {
    VALUE rate;
    rb_scan_args(argc, argv, "01", &rate);
    rpg_set_frame_rate(self, argc == 0 ? INT2NUM(DEFAULT_FRAME_RATE) : rate);

    double delta = glfwGetTime();
    ID update = rb_intern("update");

    while (!glfwWindowShouldClose(rpgGAME_WINDOW)) {
        while (delta < glfwGetTime()) {
            frame_count++;
            rb_funcall(rb_mGame, update, 0);
            rpg_input_update();
            delta += rpgTICK;
        }
        rpg_render();
        glfwPollEvents();
        glfwSwapBuffers(rpgGAME_WINDOW);
    }
    return Qnil;
}

static VALUE rpg_create(int argc, VALUE *argv, VALUE module) {
    VALUE w, h, caption, options;
    rb_scan_args(argc, argv, "21:", &w, &h, &caption, &options);
    if (rpgGAME_WINDOW) {
        rb_raise(rb_eRPGError, "graphics context already extists on calling thread");
    }
    if (!glfwInit()) {
        rb_raise(rb_eRPGError, "failed to initialize GLFW");
    }
    glfwSetErrorCallback(rpg_error_cb);
    GLFWmonitor *monitor;
    int lock_aspect = GL_FALSE;
    const char *title = RTEST(caption) ? StringValueCStr(caption) : NULL;
    rpgWIDTH = NUM2INT(w);
    rpgHEIGHT = NUM2INT(h);
    check_dimensions(rpgWIDTH, rpgHEIGHT);

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

        opt = rb_hash_aref(options, STR2SYM("locked"));
        lock_aspect = RTEST(opt);

    }

    rpgGAME_WINDOW = glfwCreateWindow(rpgWIDTH, rpgHEIGHT, title, monitor, NULL);
    if (!rpgGAME_WINDOW) {
        rb_raise(rb_eRPGError, "failed to create graphics context, ensure OpenGL 3.3 is supported on the system");
    }

    // Make OpenGL context current and import OpenGL function pointers
    glfwMakeContextCurrent(rpgGAME_WINDOW);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    if (vsync >= 0) {
        glfwSwapInterval(vsync);
    }
    if (lock_aspect) {
        glfwSetWindowAspectRatio(rpgGAME_WINDOW, rpgWIDTH, rpgHEIGHT);
    }
        
    // Enable required OpenGL capabilities
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_BLEND);

    char *vert_path = RPG_MALLOC(256);
    char *frag_path = RPG_MALLOC(256);
    sprintf(vert_path, "%s/assets/shaders/%s", RPG_BASE, STOCK_VERTEX_SHADER);
    sprintf(frag_path, "%s/assets/shaders/%s", RPG_BASE, STOCK_FRAGMENT_SHADER);

    // Create shader program and cache uniform locations
    rpgPROGRAM = rpg_create_shader_program(vert_path, frag_path, NULL);
    rpgUNIFORM_PROJECTION = glGetUniformLocation(rpgPROGRAM, "projection");
    rpgUNIFORM_MODEL = glGetUniformLocation(rpgPROGRAM, "model");
    rpgUNIFORM_COLOR = glGetUniformLocation(rpgPROGRAM, "color");
    rpgUNIFORM_TONE = glGetUniformLocation(rpgPROGRAM, "tone");
    rpgUNIFORM_ALPHA = glGetUniformLocation(rpgPROGRAM, "alpha");
    rpgUNIFORM_FLASH = glGetUniformLocation(rpgPROGRAM, "flash");
    rpgUNIFORM_HUE = glGetUniformLocation(rpgPROGRAM, "hue");

    RPG_FREE(vert_path);
    RPG_FREE(frag_path);

    // Bind callbacks
    glfwSetFramebufferSizeCallback(rpgGAME_WINDOW, rpg_buffer_resize);
    glfwSetKeyCallback(rpgGAME_WINDOW, rpg_input_key_cb);
    glfwSetMouseButtonCallback(rpgGAME_WINDOW, rpg_input_mouse_cb);
    glfwSetScrollCallback(rpgGAME_WINDOW, rpg_input_mouse_scroll_cb);

    rpg_resolution(rpgWIDTH, rpgHEIGHT);

    return Qnil;
}



static VALUE rpg_capture(VALUE module) {
    RPGimage *img = rpg_snapshot();
    return Data_Wrap_Struct(rb_cImage, NULL, NULL, img);
}

/**
 * @brief Main entry point.
 * 
 * Main entry point, called by Ruby when the gem is required.
 * 
 */
void Init_open_rpg(void) {

    rb_mOpenRPG = rb_define_module("OpenRPG");
    rb_mGame = rb_define_module_under(rb_mOpenRPG, "Game");
    rb_eRPGError = rb_define_class_under(rb_mOpenRPG, "RPGError", rb_eStandardError);
    
    rpg_timespan_init(rb_mOpenRPG);
    rpg_input_init(rb_mOpenRPG);
    rpg_app_init(rb_mOpenRPG);
    rpg_image_init(rb_mOpenRPG);
    rpg_renderable_init(rb_mOpenRPG);
    rpg_shader_init(rb_mOpenRPG);
    rpg_color_init(rb_mOpenRPG);
    rpg_tone_init(rb_mOpenRPG);
    rpg_font_init(rb_mOpenRPG);
    rpg_table_init(rb_mOpenRPG);
    rpg_tmx_init(rb_mOpenRPG);
    rpg_geometry_init(rb_mOpenRPG); 
    rpg_numerics_init(rb_mOpenRPG);
    rpg_audio_init(rb_mOpenRPG);
    rpg_reverb_init(rb_mAudio);

    VALUE dir = rb_define_module_under(rb_mOpenRPG, "Direction");
    rb_define_const(dir, "NONE", INT2NUM(RPG_NONE));
    rb_define_const(dir, "LEFT", INT2NUM(RPG_LEFT));
    rb_define_const(dir, "TOP", INT2NUM(RPG_TOP));
    rb_define_const(dir, "RIGHT", INT2NUM(RPG_RIGHT));
    rb_define_const(dir, "BOTTOM", INT2NUM(RPG_BOTTOM));
    rb_define_const(dir, "TOP_LEFT", INT2NUM(RPG_TOP_LEFT));
    rb_define_const(dir, "TOP_RIGHT", INT2NUM(RPG_TOP_RIGHT));
    rb_define_const(dir, "BOTTOM_LEFT", INT2NUM(RPG_BOTTOM_LEFT));
    rb_define_const(dir, "BOTTOM_RIGHT", INT2NUM(RPG_BOTTOM_RIGHT));
    rb_define_const(dir, "WEST", INT2NUM(RPG_WEST));
    rb_define_const(dir, "NORTH", INT2NUM(RPG_NORTH));
    rb_define_const(dir, "EAST", INT2NUM(RPG_EAST));
    rb_define_const(dir, "SOUTH", INT2NUM(RPG_SOUTH));
    rb_define_const(dir, "NORTH_WEST", INT2NUM(RPG_NORTH_WEST));
    rb_define_const(dir, "NORTH_EAST", INT2NUM(RPG_NORTH_EAST));
    rb_define_const(dir, "SOUTH_WEST", INT2NUM(RPG_SOUTH_WEST));
    rb_define_const(dir, "SOUTH_EAST", INT2NUM(RPG_SOUTH_EAST));
    rb_define_const(dir, "ALL", INT2NUM(RPG_ALL_DIRECTIONS));

    VALUE base = rb_str_new_cstr(RPG_BASE);
    rb_str_freeze(base);
    rb_define_const(rb_mOpenRPG, "BASE_DIRECTORY", base);
    frame_rate = DEFAULT_FRAME_RATE;
    rpgTICK = 1.0 / frame_rate;
    vsync = -1;
    memset(&rpgBACK_COLOR, 0, sizeof(RPGcolor));
    memset(&rpgPROJECTION, 0, sizeof(RPGmat4));
    rpgRENDER_BATCH = ALLOC(RPGbatch);
    rpg_batch_init(rpgRENDER_BATCH);


    rb_define_singleton_method(rb_mOpenRPG, "create", rpg_create, -1);
    rb_define_singleton_method(rb_mOpenRPG, "vsync", rpg_get_vsync, 0);
    rb_define_singleton_method(rb_mOpenRPG, "vsync=", rpg_set_vsync, 1);
    rb_define_singleton_method(rb_mOpenRPG, "destroy", rpg_destroy, 0);
    rb_define_singleton_method(rb_mOpenRPG, "capture", rpg_capture, 0);

    rb_define_singleton_method(rb_mGame, "update", rpg_empty_method, 0);
    rb_define_singleton_method(rb_mGame, "main", rpg_game_main, -1);
    rb_define_singleton_method(rb_mGame, "width", rpg_game_width, 0);
    rb_define_singleton_method(rb_mGame, "height", rpg_game_height, 0);
    rb_define_singleton_method(rb_mGame, "size", rpg_game_get_size, 0);
    rb_define_singleton_method(rb_mGame, "size=", rpg_game_set_size, 1);
    rb_define_singleton_method(rb_mGame, "frame_count", rpg_get_frame_count, 0);
    rb_define_singleton_method(rb_mGame, "frame_count=", rpg_set_frame_count, 1);
    rb_define_singleton_method(rb_mGame, "frame_rate", rpg_get_frame_rate, 0);
    rb_define_singleton_method(rb_mGame, "frame_rate=", rpg_set_frame_rate, 1);
    rb_define_singleton_method(rb_mGame, "back_color", rpg_get_bg_color, 0);
    rb_define_singleton_method(rb_mGame, "back_color=", rpg_set_bg_color, 1);
    rb_define_singleton_method(rb_mGame, "transition", rpg_transition, -1);
}

char *rpg_read_file(const char *fname, size_t *length) {
    char *buffer = NULL;
    FILE *file = fopen(fname, "rb");
    if (file) {
        fseek(file, 0, SEEK_END);
        long len = ftell(file);
        fseek(file, 0, SEEK_SET);
        buffer = RPG_MALLOC(len);
        if (buffer) {
            *length = fread(buffer, 1, len, file);
        }
        fclose(file);
    }
    return buffer;
}

GLuint rpg_create_shader_src(const char *src, GLenum type) {
    if (src == NULL) {
        return 0;
    }
    GLuint shader = glCreateShader(type);
    GLint length = (GLint)strlen(src);
    glShaderSource(shader, 1, &src, &length);
    glCompileShader(shader);
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE) {
        char log[512];
        glGetShaderInfoLog(shader, 512, NULL, log);
        rb_raise(rb_eRPGError, "failed to compile shader -- %s", log);
    }
    return shader;
}

GLuint rpg_create_shader(const char *fname, GLenum type) {
    if (fname == NULL) {
        return 0;
    }
    GLuint shader = glCreateShader(type);
    size_t len;
    const char *src = rpg_read_file(fname, &len);
    GLint length = (GLint)len;
    glShaderSource(shader, 1, &src, &length);
    glCompileShader(shader);
    RPG_FREE((void *)src);
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE) {
        char log[512];
        glGetShaderInfoLog(shader, 512, NULL, log);
        rb_raise(rb_eRPGError, "failed to compile shader -- %s", log);
    }
    return shader;
}

GLuint rpg_create_shader_program(const char *vert_path, const char *frag_path, const char *geo_path) {

    GLuint program = glCreateProgram();
    GLuint vertex = rpg_create_shader(vert_path, GL_VERTEX_SHADER);
    GLuint fragment = rpg_create_shader(frag_path, GL_FRAGMENT_SHADER);
    GLuint geometry = 0;

    glAttachShader(program, vertex);
    glAttachShader(program, fragment);

    if (geo_path) {
        geometry = rpg_create_shader(geo_path, GL_GEOMETRY_SHADER);
        glAttachShader(program, geometry);
    }
    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success != GL_TRUE) {
        char log[1024];
        glGetProgramInfoLog(program, 1024, NULL, log);
        rb_raise(rb_eRPGError, "failed to link shader program -- %s", log);
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometry) {
        glDeleteShader(geometry);
    }
    return program;
}