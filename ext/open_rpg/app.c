#include "./app.h"

VALUE file_drops;

const char *caption;

void rpg_app_init(VALUE parent) {
    VALUE app = rb_define_module_under(parent, "App");

    rb_define_singleton_method(app, "set_icon", rpg_app_set_icon, -1);
    rb_define_singleton_method(app, "client_size", rpg_app_window_size, 0);
    rb_define_singleton_method(app, "client_width", rpg_app_window_width, 0);
    rb_define_singleton_method(app, "client_height", rpg_app_window_height, 0);
    rb_define_singleton_method(app, "close", rpg_app_close, 0);
    rb_define_singleton_method(app, "closing?", rpg_app_closing_p, 0);

    rb_define_singleton_method(app, "on_file_drop", rpg_app_on_file_drop, 0);

    file_drops = Qnil;
    caption = NULL;
}

static VALUE rpg_app_get_caption(VALUE module) {
    // TODO: Mem
    return caption ? rb_str_new2(caption) : Qnil;
}

static VALUE rpg_app_set_caption(VALUE module, VALUE value) {
    // TODO: Mem
    caption = NIL_P(value) ? NULL : StringValueCStr(value);
    rpg_app_caption(caption);
    return value;
}

static VALUE rpg_app_set_icon(int argc, VALUE *argv, VALUE module) {
    if (argc > 0) {
        struct GLFWimage imgs[argc];
        for (int i = 0; i < argc; i++) {
            if (RB_TYPE_P(argv[i], T_DATA)) {
                RPGimage *image = DATA_PTR(argv[i]);
                int size;
                imgs[i].width = image->width;
                imgs[i].height = image->height;
                imgs[i].pixels = rpg_image_pixels(image, &size);
            } else {
                const char *fname = StringValueCStr(argv[i]);
                int width, height;
                imgs[i].pixels = rpg_image_load(fname, &imgs[i].width, &imgs[i].height);
            }
        }
        glfwSetWindowIcon(game_window, argc, imgs);
        for (int i = 0; i < argc; i++) {
            xfree(imgs[i].pixels);
        }
    }
    return Qnil;
}

static VALUE rpg_app_window_size(VALUE module) {
    RPGsize *size = ALLOC(RPGsize);
    glfwGetFramebufferSize(game_window, &size->width, &size->height);
    return Data_Wrap_Struct(rb_cSize, NULL, RUBY_DEFAULT_FREE, size);
}

static VALUE rpg_app_window_width(VALUE module) {
    int width;
    glfwGetFramebufferSize(game_window, &width, NULL);
    return INT2NUM(width);
}

static VALUE rpg_app_window_height(VALUE module) {
    int height;
    glfwGetFramebufferSize(game_window, NULL, &height);
    return INT2NUM(height);
}

static VALUE rpg_app_close(int argc, VALUE *argv, VALUE module) {
    VALUE close;
    rb_scan_args(argc, argv, "01", &close);
    glfwSetWindowShouldClose(game_window, argc > 0 ? RTEST(close) : GLFW_TRUE);
    return Qnil;
}

static VALUE rpg_app_closing_p(VALUE module) { return RB_BOOL(glfwWindowShouldClose(game_window)); }

void rpg_app_caption(const char *str) {
    glfwSetWindowTitle(game_window, str);
    caption = str;
}

static VALUE rpg_app_on_file_drop(VALUE module) {
    file_drops = rb_block_given_p() ? rb_block_proc() : Qnil;
    glfwSetDropCallback(game_window, file_drops == Qnil ? NULL : rpg_app_files_dropped);
    return file_drops;
}

static void rpg_app_files_dropped(GLFWwindow *window, int count, const char **filepaths) {
    if (file_drops == Qnil) {
        return;
    }
    VALUE ary = rb_ary_new_capa(count);
    for (int i = 0; i < count; i++) {
        rb_ary_store(ary, i, rb_str_new_cstr(filepaths[i]));
    }
    rb_proc_call(file_drops, ary);
}