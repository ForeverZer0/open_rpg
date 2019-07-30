#include "./application.h"

const char *caption;

void rpg_app_init(VALUE parent) {
    VALUE app = rb_define_module_under(parent, "Application");

    rb_define_singleton_method(app, "client_width", rpg_app_window_width, 0);
    rb_define_singleton_method(app, "client_height", rpg_app_window_height, 0);

    caption = NULL;
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

static VALUE rpg_app_closing_p(VALUE module) {
    return RB_BOOL(glfwWindowShouldClose(game_window));
}

void rpg_app_caption(const char* str) {
    glfwSetWindowTitle(game_window, str);
    caption = str;
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