#include "./rpg.h"

VALUE cb_file_drop;
VALUE cb_window_move;
VALUE cb_focus;
VALUE cb_minimize;
VALUE cb_maximize;
VALUE cb_size_changed;
VALUE cb_close;

static void rpg_app_files_dropped(RPGwindow *window, int count, const char **filepaths) {
    if (cb_file_drop == Qnil) {
        return;
    }
    VALUE ary = rb_ary_new_capa(count);
    for (int i = 0; i < count; i++) {
        rb_ary_store(ary, i, rb_str_new_cstr(filepaths[i]));
    }
    rb_proc_call(cb_file_drop, rb_ary_new4(1, &ary));
}

static void rpg_app_moved(RPGwindow *window, int x, int y) {
    if (cb_window_move == Qnil) {
        return;
    }
    VALUE ary = rb_ary_new_capa(2);
    rb_ary_store(ary, 0, INT2NUM(x));
    rb_ary_store(ary, 1, INT2NUM(y));
    rb_proc_call(cb_window_move, ary);
}

static void rpg_app_focused(RPGwindow *window, int focused) {
    if (cb_focus == Qnil) {
        return;
    }
    rb_proc_call(cb_focus, RB_BOOL(focused));
}

static void rpg_app_size_change(RPGwindow *window, int width, int height) {
    if (cb_size_changed == Qnil) {
        return;
    }
    VALUE ary = rb_ary_new_capa(2);
    rb_ary_store(ary, 0, INT2NUM(width));
    rb_ary_store(ary, 0, INT2NUM(height));
    rb_proc_call(cb_size_changed, ary);
}

static void rpg_app_closing(RPGwindow *window) {
    if (cb_close == Qnil) {
        return;
    }
    rb_proc_call(cb_close, Qnil);
}

static void rpg_app_minimized(RPGwindow *window, int minimized) {
    if (cb_minimize == Qnil) {
        return;
    }
    rb_proc_call(cb_minimize, RB_BOOL(minimized));
}

static void rpg_app_maximized(RPGwindow *window, int maximized) {
    if (cb_maximize == Qnil) {
        return;
    }
    rb_proc_call(cb_maximize, RB_BOOL(maximized));
}

static VALUE rpg_app_frame_size(VALUE module) {
    int l, r, t, b;
    glfwGetWindowFrameSize(rpgGAME_WINDOW, &l, &t, &r, &b);
    VALUE hash = rb_hash_new();
    rb_hash_aset(hash, NUM2INT(l), STR2SYM("left"));
    rb_hash_aset(hash, NUM2INT(t), STR2SYM("top"));
    rb_hash_aset(hash, NUM2INT(r), STR2SYM("right"));
    rb_hash_aset(hash, NUM2INT(b), STR2SYM("bottom"));
    return hash;
}

static VALUE rpg_app_on_move(VALUE module) {
    if (rb_block_given_p()) {
        cb_window_move - rb_block_proc();
        glfwSetWindowPosCallback(rpgGAME_WINDOW, rpg_app_moved);
    } else {
        cb_window_move = Qnil;
        glfwSetWindowPosCallback(rpgGAME_WINDOW, NULL);
    }
    return Qnil;
}

static VALUE rpg_app_move(int argc, VALUE *argv, VALUE module) {
    VALUE x, y;
    rb_scan_args(argc, argv, "11", &x, &y);
    if (argc == 1) {
        RPGpoint *pnt = DATA_PTR(x);
        glfwSetWindowPos(rpgGAME_WINDOW, pnt->x, pnt->y);
    } else {
        glfwSetWindowPos(rpgGAME_WINDOW, NUM2INT(x), NUM2INT(y));
    }
    return Qnil;
}

static VALUE rpg_app_get_opacity(VALUE module) { return DBL2NUM(glfwGetWindowOpacity(rpgGAME_WINDOW)); }

static VALUE rpg_app_set_opacity(VALUE module, VALUE value) {
    float opacity = clampf(NUM2FLT(value), 0.0f, 1.0f);
    glfwSetWindowOpacity(rpgGAME_WINDOW, opacity);
    return value;
}

static VALUE rpg_app_get_clipboard(VALUE module) {
    const char *str = glfwGetClipboardString(rpgGAME_WINDOW);
    return str == NULL ? Qnil : rb_str_new_cstr(str);
}

static VALUE rpg_app_set_clipboard(VALUE module, VALUE value) {
    glfwSetClipboardString(rpgGAME_WINDOW, RTEST(value) ? StringValueCStr(value) : NULL);
    return value;
}

static VALUE rpg_app_set_title(VALUE module, VALUE value) {
    char *title = NIL_P(value) ? NULL : StringValueCStr(value);
    glfwSetWindowTitle(rpgGAME_WINDOW, title);
    return value;
}

static VALUE rpg_app_request_attention(VALUE module) {
    glfwRequestWindowAttention(rpgGAME_WINDOW);
    return Qnil;
}

static VALUE rpg_app_show(VALUE module) {
    glfwShowWindow(rpgGAME_WINDOW);
    return Qnil;
}

static VALUE rpg_app_hide(VALUE module) {
    glfwHideWindow(rpgGAME_WINDOW);
    return Qnil;
}

static VALUE rpg_app_restore(VALUE module) {
    glfwRestoreWindow(rpgGAME_WINDOW);
    return Qnil;
}

static VALUE rpg_app_minmize(VALUE module) {
    glfwIconifyWindow(rpgGAME_WINDOW);
    return Qnil;
}

static VALUE rpg_app_minmized_p(VALUE module) { return RB_BOOL(glfwGetWindowAttrib(rpgGAME_WINDOW, GLFW_ICONIFIED)); }

static VALUE rpg_app_maximize(VALUE module) {
    glfwMaximizeWindow(rpgGAME_WINDOW);
    return Qnil;
}

static VALUE rpg_app_maximized_p(VALUE module) { return RB_BOOL(glfwGetWindowAttrib(rpgGAME_WINDOW, GLFW_MAXIMIZED)); }

static VALUE rpg_app_focus(VALUE module) {
    glfwFocusWindow(rpgGAME_WINDOW);
    return Qnil;
}

static VALUE rpg_app_focused_p(VALUE module) { return RB_BOOL(glfwGetWindowAttrib(rpgGAME_WINDOW, GLFW_FOCUSED)); }

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
                RPG_THROW_UNLESS_FILE(fname);
                int width, height;
                imgs[i].pixels = rpg_image_load(fname, &imgs[i].width, &imgs[i].height);
            }
        }
        glfwSetWindowIcon(rpgGAME_WINDOW, argc, imgs);
        for (int i = 0; i < argc; i++) {
            RPG_FREE(imgs[i].pixels);
        }
    }
    return Qnil;
}

static VALUE rpg_app_window_size(VALUE module) {
    RPGsize *size = ALLOC(RPGsize);
    glfwGetFramebufferSize(rpgGAME_WINDOW, &size->width, &size->height);
    return Data_Wrap_Struct(rb_cSize, NULL, RUBY_DEFAULT_FREE, size);
}

static VALUE rpg_app_window_set_size(VALUE module, VALUE value) {
    RPGsize *size = DATA_PTR(value);
    check_dimensions(size->width, size->height);
    glfwSetWindowSize(rpgGAME_WINDOW, size->width, size->height);
    return value;
}

static VALUE rpg_app_window_width(VALUE module) {
    int width;
    glfwGetFramebufferSize(rpgGAME_WINDOW, &width, NULL);
    return INT2NUM(width);
}

static VALUE rpg_app_window_height(VALUE module) {
    int height;
    glfwGetFramebufferSize(rpgGAME_WINDOW, NULL, &height);
    return INT2NUM(height);
}

static VALUE rpg_app_close(int argc, VALUE *argv, VALUE module) {
    VALUE close;
    rb_scan_args(argc, argv, "01", &close);
    glfwSetWindowShouldClose(rpgGAME_WINDOW, argc > 0 ? RTEST(close) : GLFW_TRUE);
    return Qnil;
}

static VALUE rpg_app_closing_p(VALUE module) { return RB_BOOL(glfwWindowShouldClose(rpgGAME_WINDOW)); }

static VALUE rpg_app_on_file_drop(VALUE module) {
    cb_file_drop = rb_block_given_p() ? rb_block_proc() : Qnil;
    glfwSetDropCallback(rpgGAME_WINDOW, cb_file_drop == Qnil ? NULL : rpg_app_files_dropped);
    return cb_file_drop;
}

static VALUE rpg_app_on_size_changed(VALUE module) {
    if (rb_block_given_p()) {
        cb_size_changed = rb_block_proc();
        glfwSetWindowSizeCallback(rpgGAME_WINDOW, rpg_app_size_change);
    } else {
        cb_size_changed = Qnil;
        glfwSetWindowSizeCallback(rpgGAME_WINDOW, NULL);
    }
    return Qnil;
}

static VALUE rpg_app_on_close(VALUE module) {
    if (rb_block_given_p()) {
        cb_close = rb_block_proc();
        glfwSetWindowCloseCallback(rpgGAME_WINDOW, rpg_app_closing);
    } else {
        cb_close = Qnil;
        glfwSetWindowCloseCallback(rpgGAME_WINDOW, NULL);
    }
    return Qnil;
}

static VALUE rpg_app_on_minimize_change(VALUE module) {
    if (rb_block_given_p()) {
        cb_minimize = rb_block_given_p();
        glfwSetWindowIconifyCallback(rpgGAME_WINDOW, rpg_app_minimized);
    } else {
        cb_minimize = Qnil;
        glfwSetWindowIconifyCallback(rpgGAME_WINDOW, NULL);
    }
    return Qnil;
}

static VALUE rpg_app_on_maximize_change(VALUE module) {
    if (rb_block_given_p()) {
        cb_maximize = rb_block_given_p();
        glfwSetWindowMaximizeCallback(rpgGAME_WINDOW, rpg_app_maximized);
    } else {
        cb_maximize = Qnil;
        glfwSetWindowMaximizeCallback(rpgGAME_WINDOW, NULL);
    }
    return Qnil;
}

static VALUE rpg_app_on_focus_change(VALUE module) {
    if (rb_block_given_p()) {
        cb_focus = rb_block_proc();
        glfwSetWindowFocusCallback(rpgGAME_WINDOW, rpg_app_focused);
    } else {
        cb_focus = Qnil;
        glfwSetWindowFocusCallback(rpgGAME_WINDOW, NULL);
    }
    return Qnil;
}

void rpg_app_init(VALUE parent) {
    VALUE app = rb_define_module_under(parent, "App");

    rb_define_singleton_method(app, "set_title", rpg_app_set_title, 1);
    rb_define_singleton_method(app, "set_icon", rpg_app_set_icon, -1);
    rb_define_singleton_method(app, "client_size", rpg_app_window_size, 0);
    rb_define_singleton_method(app, "client_size=", rpg_app_window_set_size, 1);
    rb_define_singleton_method(app, "client_width", rpg_app_window_width, 0);
    rb_define_singleton_method(app, "client_height", rpg_app_window_height, 0);
    rb_define_singleton_method(app, "close", rpg_app_close, -1);
    rb_define_singleton_method(app, "closing?", rpg_app_closing_p, 0);
    rb_define_singleton_method(app, "focus", rpg_app_focus, 0);
    rb_define_singleton_method(app, "focused?", rpg_app_focused_p, 0);
    rb_define_singleton_method(app, "minimize", rpg_app_minmize, 0);
    rb_define_singleton_method(app, "minimized?", rpg_app_minmized_p, 0);
    rb_define_singleton_method(app, "maximize", rpg_app_maximize, 0);
    rb_define_singleton_method(app, "maximized?", rpg_app_maximized_p, 0);
    rb_define_singleton_method(app, "restore", rpg_app_restore, 0);
    rb_define_singleton_method(app, "show", rpg_app_show, 0);
    rb_define_singleton_method(app, "hide", rpg_app_hide, 0);
    rb_define_singleton_method(app, "request_attention", rpg_app_request_attention, 0);
    rb_define_singleton_method(app, "move", rpg_app_move, -1);
    rb_define_singleton_method(app, "frame_size", rpg_app_frame_size, 0);

    rb_define_singleton_method(app, "on_focus_change", rpg_app_on_focus_change, 0);
    rb_define_singleton_method(app, "on_move", rpg_app_on_move, 0);
    rb_define_singleton_method(app, "on_file_drop", rpg_app_on_file_drop, 0);
    rb_define_singleton_method(app, "on_minimize_change", rpg_app_on_minimize_change, 0);
    rb_define_singleton_method(app, "on_maximize_change", rpg_app_on_maximize_change, 0);
    rb_define_singleton_method(app, "on_close", rpg_app_on_close, 0);
    rb_define_singleton_method(app, "on_size_change", rpg_app_on_size_changed, 0);

    rb_define_singleton_method(app, "clipboard", rpg_app_get_clipboard, 0);
    rb_define_singleton_method(app, "clipboard=", rpg_app_set_clipboard, 1);
    rb_define_singleton_method(app, "opacity", rpg_app_get_opacity, 0);
    rb_define_singleton_method(app, "opacity=", rpg_app_set_opacity, 1);

    cb_focus = Qnil;
    cb_file_drop = Qnil;
    cb_window_move = Qnil;
    cb_minimize = Qnil;
    cb_maximize = Qnil;
    cb_size_changed = Qnil;
    cb_close = Qnil;
}