#include "./input.h"

VALUE rb_mInput;

// TODO: Ensure these references do no require marking
VALUE cb_key;
VALUE cb_mouse_move;
VALUE cb_mouse_enter;
VALUE cb_mouse_leave;
VALUE cb_mouse_button;
VALUE cb_text;

double scroll_x;
double scroll_y;

GLFWcursor *mouse_cursor;
RPGkeybinding *bindings;

char key_state[GLFW_KEY_LAST + 1];
char mouse_state[GLFW_MOUSE_BUTTON_LAST + 1];

void rpg_input_init(VALUE parent) {

    rb_mInput = rb_define_module_under(parent, "Input");

    VALUE kb = rb_define_module_under(rb_mInput, "Keyboard");
    VALUE ms = rb_define_module_under(rb_mInput, "Mouse");
    VALUE keys = rb_define_module_under(rb_mInput, "Key");
    VALUE mod_keys = rb_define_module_under(rb_mInput, "ModKey");

    memset(&key_state, 0, sizeof(key_state));
    memset(&mouse_state, 0, sizeof(mouse_state));
    mouse_cursor = NULL;
    bindings = NULL;

    rb_define_singleton_method(rb_mInput, "bind", rpg_input_bind, 3);
    rb_define_singleton_method(rb_mInput, "unbind", rpg_input_unbind, 1);
    rb_define_singleton_method(rb_mInput, "each_binding", rpg_input_each_binding, 0);
    rb_define_singleton_method(rb_mInput, "trigger?", rpg_input_trigger_p, 1);
    rb_define_singleton_method(rb_mInput, "repeat?", rpg_input_repeat_p, 1);
    rb_define_singleton_method(rb_mInput, "release?", rpg_input_release_p, 1);
    rb_define_singleton_method(rb_mInput, "press?", rpg_input_press_p, 1);
    rb_define_singleton_method(rb_mInput, "on_text", rpg_input_on_text, 0);

    rb_define_singleton_method(kb, "trigger?", rpg_kb_trigger_p, 1);
    rb_define_singleton_method(kb, "repeat?", rpg_kb_repeat_p, 1);
    rb_define_singleton_method(kb, "release?", rpg_kb_release_p, 1);
    rb_define_singleton_method(kb, "press?", rpg_kb_press_p, 1);
    rb_define_singleton_method(kb, "key_name", rpg_kb_key_name, 1);
    rb_define_singleton_method(kb, "scancode_name", rpg_kb_scancode_name, 1);
    rb_define_singleton_method(kb, "scancode", rpg_kb_key_scancode, 1);
    rb_define_singleton_method(kb, "on_key", rpg_input_on_key, 0);

    rb_define_singleton_method(ms, "trigger?", rpg_mouse_trigger_p, 1);
    rb_define_singleton_method(ms, "repeat?", rpg_mouse_repeat_p, 1);
    rb_define_singleton_method(ms, "release?", rpg_mouse_release_p, 1);
    rb_define_singleton_method(ms, "press?", rpg_mouse_press_p, 1);
    rb_define_singleton_method(ms, "change_cursor", rpg_mouse_change_cursor, -1);
    rb_define_singleton_method(ms, "location", rpg_mouse_location, 0);
    rb_define_singleton_method(ms, "x", rpg_mouse_x, 0);
    rb_define_singleton_method(ms, "y", rpg_mouse_y, 0);
    rb_define_singleton_method(ms, "scroll", rpg_mouse_scroll, 0);
    rb_define_singleton_method(ms, "scroll_x", rpg_mouse_scroll_x, 0);
    rb_define_singleton_method(ms, "scroll_y", rpg_mouse_scroll_y, 0);
    rb_define_singleton_method(ms, "cursor_mode", rpg_mouse_cursor_mode, -1);
    rb_define_singleton_method(ms, "on_move", rpg_input_on_mouse_move, 0);
    rb_define_singleton_method(ms, "on_button", rpg_input_on_mouse_button, 0);
    rb_define_singleton_method(ms, "on_enter", rpg_input_on_mouse_enter, 0);
    rb_define_singleton_method(ms, "on_leave", rpg_input_on_mouse_leave, 0);

    cb_key = Qnil;
    cb_mouse_move = Qnil;
    cb_mouse_button = Qnil;
    cb_mouse_enter = Qnil;
    cb_mouse_leave = Qnil;
    cb_text = Qnil;

#if RPG_GAMEPAD_SUPPORT
    VALUE gp = rb_define_module_under(rb_mInput, "Gamepad");

    // Gamepad Constants
    rb_define_const(gp, "A", INT2NUM(GLFW_GAMEPAD_BUTTON_A));
    rb_define_const(gp, "B", INT2NUM(GLFW_GAMEPAD_BUTTON_B));
    rb_define_const(gp, "X", INT2NUM(GLFW_GAMEPAD_BUTTON_X));
    rb_define_const(gp, "Y", INT2NUM(GLFW_GAMEPAD_BUTTON_Y));
    rb_define_const(gp, "LEFT_BUMPER", INT2NUM(GLFW_GAMEPAD_BUTTON_LEFT_BUMPER));
    rb_define_const(gp, "RIGHT_BUMPER", INT2NUM(GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER));
    rb_define_const(gp, "BACK", INT2NUM(GLFW_GAMEPAD_BUTTON_BACK));
    rb_define_const(gp, "START", INT2NUM(GLFW_GAMEPAD_BUTTON_START));
    rb_define_const(gp, "GUIDE", INT2NUM(GLFW_GAMEPAD_BUTTON_GUIDE));
    rb_define_const(gp, "LEFT_THUMB", INT2NUM(GLFW_GAMEPAD_BUTTON_LEFT_THUMB));
    rb_define_const(gp, "RIGHT_THUMB", INT2NUM(GLFW_GAMEPAD_BUTTON_RIGHT_THUMB));
    rb_define_const(gp, "DPAD_UP", INT2NUM(GLFW_GAMEPAD_BUTTON_DPAD_UP));
    rb_define_const(gp, "DPAD_RIGHT", INT2NUM(GLFW_GAMEPAD_BUTTON_DPAD_RIGHT));
    rb_define_const(gp, "DPAD_DOWN", INT2NUM(GLFW_GAMEPAD_BUTTON_DPAD_DOWN));
    rb_define_const(gp, "DPAD_LEFT", INT2NUM(GLFW_GAMEPAD_BUTTON_DPAD_LEFT));
    rb_define_const(gp, "DPAD_CROSS", INT2NUM(GLFW_GAMEPAD_BUTTON_CROSS));
    rb_define_const(gp, "DPAD_CIRCLE", INT2NUM(GLFW_GAMEPAD_BUTTON_CIRCLE));
    rb_define_const(gp, "DPAD_SQUARE", INT2NUM(GLFW_GAMEPAD_BUTTON_SQUARE));
    rb_define_const(gp, "DPAD_TRIANGLE", INT2NUM(GLFW_GAMEPAD_BUTTON_TRIANGLE));
#endif

    // Mouse Constants
    rb_define_const(ms, "BUTTON_1", INT2NUM(GLFW_MOUSE_BUTTON_1));
    rb_define_const(ms, "BUTTON_2", INT2NUM(GLFW_MOUSE_BUTTON_2));
    rb_define_const(ms, "BUTTON_3", INT2NUM(GLFW_MOUSE_BUTTON_3));
    rb_define_const(ms, "BUTTON_4", INT2NUM(GLFW_MOUSE_BUTTON_4));
    rb_define_const(ms, "BUTTON_5", INT2NUM(GLFW_MOUSE_BUTTON_5));
    rb_define_const(ms, "BUTTON_6", INT2NUM(GLFW_MOUSE_BUTTON_6));
    rb_define_const(ms, "BUTTON_7", INT2NUM(GLFW_MOUSE_BUTTON_7));
    rb_define_const(ms, "BUTTON_8", INT2NUM(GLFW_MOUSE_BUTTON_8));
    rb_define_const(ms, "LEFT", INT2NUM(GLFW_MOUSE_BUTTON_LEFT));
    rb_define_const(ms, "RIGHT", INT2NUM(GLFW_MOUSE_BUTTON_RIGHT));
    rb_define_const(ms, "MIDDLE", INT2NUM(GLFW_MOUSE_BUTTON_MIDDLE));

    rb_define_const(ms, "ARROW_CURSOR", INT2NUM(GLFW_ARROW_CURSOR));
    rb_define_const(ms, "IBEAM_CURSOR", INT2NUM(GLFW_IBEAM_CURSOR));
    rb_define_const(ms, "CROSSHAIR_CURSOR", INT2NUM(GLFW_CROSSHAIR_CURSOR));
    rb_define_const(ms, "HAND_CURSOR", INT2NUM(GLFW_HAND_CURSOR));
    rb_define_const(ms, "HRESIZE_CURSOR", INT2NUM(GLFW_HRESIZE_CURSOR));
    rb_define_const(ms, "VRESIZE_CURSOR", INT2NUM(GLFW_VRESIZE_CURSOR));

    // Key Constants
    rb_define_const(keys, "SPACE", INT2NUM(GLFW_KEY_SPACE));
    rb_define_const(keys, "APOSTROPHE", INT2NUM(GLFW_KEY_APOSTROPHE));
    rb_define_const(keys, "COMMA", INT2NUM(GLFW_KEY_COMMA));
    rb_define_const(keys, "MINUS", INT2NUM(GLFW_KEY_MINUS));
    rb_define_const(keys, "PERIOD", INT2NUM(GLFW_KEY_PERIOD));
    rb_define_const(keys, "SLASH", INT2NUM(GLFW_KEY_SLASH));
    rb_define_const(keys, "NUM_0", INT2NUM(GLFW_KEY_0));
    rb_define_const(keys, "NUM_1", INT2NUM(GLFW_KEY_1));
    rb_define_const(keys, "NUM_2", INT2NUM(GLFW_KEY_2));
    rb_define_const(keys, "NUM_3", INT2NUM(GLFW_KEY_3));
    rb_define_const(keys, "NUM_4", INT2NUM(GLFW_KEY_4));
    rb_define_const(keys, "NUM_5", INT2NUM(GLFW_KEY_5));
    rb_define_const(keys, "NUM_6", INT2NUM(GLFW_KEY_6));
    rb_define_const(keys, "NUM_7", INT2NUM(GLFW_KEY_7));
    rb_define_const(keys, "NUM_8", INT2NUM(GLFW_KEY_8));
    rb_define_const(keys, "NUM_9", INT2NUM(GLFW_KEY_9));
    rb_define_const(keys, "SEMICOLON", INT2NUM(GLFW_KEY_SEMICOLON));
    rb_define_const(keys, "EQUAL", INT2NUM(GLFW_KEY_EQUAL));
    rb_define_const(keys, "A", INT2NUM(GLFW_KEY_A));
    rb_define_const(keys, "B", INT2NUM(GLFW_KEY_B));
    rb_define_const(keys, "C", INT2NUM(GLFW_KEY_C));
    rb_define_const(keys, "D", INT2NUM(GLFW_KEY_D));
    rb_define_const(keys, "E", INT2NUM(GLFW_KEY_E));
    rb_define_const(keys, "F", INT2NUM(GLFW_KEY_F));
    rb_define_const(keys, "G", INT2NUM(GLFW_KEY_G));
    rb_define_const(keys, "H", INT2NUM(GLFW_KEY_H));
    rb_define_const(keys, "I", INT2NUM(GLFW_KEY_I));
    rb_define_const(keys, "J", INT2NUM(GLFW_KEY_J));
    rb_define_const(keys, "K", INT2NUM(GLFW_KEY_K));
    rb_define_const(keys, "L", INT2NUM(GLFW_KEY_L));
    rb_define_const(keys, "M", INT2NUM(GLFW_KEY_M));
    rb_define_const(keys, "N", INT2NUM(GLFW_KEY_N));
    rb_define_const(keys, "O", INT2NUM(GLFW_KEY_O));
    rb_define_const(keys, "P", INT2NUM(GLFW_KEY_P));
    rb_define_const(keys, "Q", INT2NUM(GLFW_KEY_Q));
    rb_define_const(keys, "R", INT2NUM(GLFW_KEY_R));
    rb_define_const(keys, "S", INT2NUM(GLFW_KEY_S));
    rb_define_const(keys, "T", INT2NUM(GLFW_KEY_T));
    rb_define_const(keys, "U", INT2NUM(GLFW_KEY_U));
    rb_define_const(keys, "V", INT2NUM(GLFW_KEY_V));
    rb_define_const(keys, "W", INT2NUM(GLFW_KEY_W));
    rb_define_const(keys, "X", INT2NUM(GLFW_KEY_X));
    rb_define_const(keys, "Y", INT2NUM(GLFW_KEY_Y));
    rb_define_const(keys, "Z", INT2NUM(GLFW_KEY_Z));
    rb_define_const(keys, "LEFT_BRACKET", INT2NUM(GLFW_KEY_LEFT_BRACKET));
    rb_define_const(keys, "BACKSLASH", INT2NUM(GLFW_KEY_BACKSLASH));
    rb_define_const(keys, "RIGHT_BRACKET", INT2NUM(GLFW_KEY_RIGHT_BRACKET));
    rb_define_const(keys, "GRAVE_ACCENT", INT2NUM(GLFW_KEY_GRAVE_ACCENT));
    rb_define_const(keys, "WORLD_1", INT2NUM(GLFW_KEY_WORLD_1));
    rb_define_const(keys, "WORLD_2", INT2NUM(GLFW_KEY_WORLD_2));
    rb_define_const(keys, "ESCAPE", INT2NUM(GLFW_KEY_ESCAPE));
    rb_define_const(keys, "ENTER", INT2NUM(GLFW_KEY_ENTER));
    rb_define_const(keys, "TAB", INT2NUM(GLFW_KEY_TAB));
    rb_define_const(keys, "BACKSPACE", INT2NUM(GLFW_KEY_BACKSPACE));
    rb_define_const(keys, "INSERT", INT2NUM(GLFW_KEY_INSERT));
    rb_define_const(keys, "DELETE", INT2NUM(GLFW_KEY_DELETE));
    rb_define_const(keys, "RIGHT", INT2NUM(GLFW_KEY_RIGHT));
    rb_define_const(keys, "LEFT", INT2NUM(GLFW_KEY_LEFT));
    rb_define_const(keys, "DOWN", INT2NUM(GLFW_KEY_DOWN));
    rb_define_const(keys, "UP", INT2NUM(GLFW_KEY_UP));
    rb_define_const(keys, "PAGE_UP", INT2NUM(GLFW_KEY_PAGE_UP));
    rb_define_const(keys, "PAGE_DOWN", INT2NUM(GLFW_KEY_PAGE_DOWN));
    rb_define_const(keys, "HOME", INT2NUM(GLFW_KEY_HOME));
    rb_define_const(keys, "END", INT2NUM(GLFW_KEY_END));
    rb_define_const(keys, "CAPS_LOCK", INT2NUM(GLFW_KEY_CAPS_LOCK));
    rb_define_const(keys, "SCROLL_LOCK", INT2NUM(GLFW_KEY_SCROLL_LOCK));
    rb_define_const(keys, "NUM_LOCK", INT2NUM(GLFW_KEY_NUM_LOCK));
    rb_define_const(keys, "PRINT_SCREEN", INT2NUM(GLFW_KEY_PRINT_SCREEN));
    rb_define_const(keys, "PAUSE", INT2NUM(GLFW_KEY_PAUSE));
    rb_define_const(keys, "F1", INT2NUM(GLFW_KEY_F1));
    rb_define_const(keys, "F2", INT2NUM(GLFW_KEY_F2));
    rb_define_const(keys, "F3", INT2NUM(GLFW_KEY_F3));
    rb_define_const(keys, "F4", INT2NUM(GLFW_KEY_F4));
    rb_define_const(keys, "F5", INT2NUM(GLFW_KEY_F5));
    rb_define_const(keys, "F6", INT2NUM(GLFW_KEY_F6));
    rb_define_const(keys, "F7", INT2NUM(GLFW_KEY_F7));
    rb_define_const(keys, "F8", INT2NUM(GLFW_KEY_F8));
    rb_define_const(keys, "F9", INT2NUM(GLFW_KEY_F9));
    rb_define_const(keys, "F10", INT2NUM(GLFW_KEY_F10));
    rb_define_const(keys, "F11", INT2NUM(GLFW_KEY_F11));
    rb_define_const(keys, "F12", INT2NUM(GLFW_KEY_F12));
    rb_define_const(keys, "F13", INT2NUM(GLFW_KEY_F13));
    rb_define_const(keys, "F14", INT2NUM(GLFW_KEY_F14));
    rb_define_const(keys, "F15", INT2NUM(GLFW_KEY_F15));
    rb_define_const(keys, "F16", INT2NUM(GLFW_KEY_F16));
    rb_define_const(keys, "F17", INT2NUM(GLFW_KEY_F17));
    rb_define_const(keys, "F18", INT2NUM(GLFW_KEY_F18));
    rb_define_const(keys, "F19", INT2NUM(GLFW_KEY_F19));
    rb_define_const(keys, "F20", INT2NUM(GLFW_KEY_F20));
    rb_define_const(keys, "F21", INT2NUM(GLFW_KEY_F21));
    rb_define_const(keys, "F22", INT2NUM(GLFW_KEY_F22));
    rb_define_const(keys, "F23", INT2NUM(GLFW_KEY_F23));
    rb_define_const(keys, "F24", INT2NUM(GLFW_KEY_F24));
    rb_define_const(keys, "F25", INT2NUM(GLFW_KEY_F25));
    rb_define_const(keys, "KP_0", INT2NUM(GLFW_KEY_KP_0));
    rb_define_const(keys, "KP_1", INT2NUM(GLFW_KEY_KP_1));
    rb_define_const(keys, "KP_2", INT2NUM(GLFW_KEY_KP_2));
    rb_define_const(keys, "KP_3", INT2NUM(GLFW_KEY_KP_3));
    rb_define_const(keys, "KP_4", INT2NUM(GLFW_KEY_KP_4));
    rb_define_const(keys, "KP_5", INT2NUM(GLFW_KEY_KP_5));
    rb_define_const(keys, "KP_6", INT2NUM(GLFW_KEY_KP_6));
    rb_define_const(keys, "KP_7", INT2NUM(GLFW_KEY_KP_7));
    rb_define_const(keys, "KP_8", INT2NUM(GLFW_KEY_KP_8));
    rb_define_const(keys, "KP_9", INT2NUM(GLFW_KEY_KP_9));
    rb_define_const(keys, "KP_DECIMAL", INT2NUM(GLFW_KEY_KP_DECIMAL));
    rb_define_const(keys, "KP_DIVIDE", INT2NUM(GLFW_KEY_KP_DIVIDE));
    rb_define_const(keys, "KP_MULTIPLY", INT2NUM(GLFW_KEY_KP_MULTIPLY));
    rb_define_const(keys, "KP_SUBTRACT", INT2NUM(GLFW_KEY_KP_SUBTRACT));
    rb_define_const(keys, "KP_ADD", INT2NUM(GLFW_KEY_KP_ADD));
    rb_define_const(keys, "KP_ENTER", INT2NUM(GLFW_KEY_KP_ENTER));
    rb_define_const(keys, "KP_EQUAL", INT2NUM(GLFW_KEY_KP_EQUAL));
    rb_define_const(keys, "LEFT_SHIFT", INT2NUM(GLFW_KEY_LEFT_SHIFT));
    rb_define_const(keys, "LEFT_CONTROL", INT2NUM(GLFW_KEY_LEFT_CONTROL));
    rb_define_const(keys, "LEFT_ALT", INT2NUM(GLFW_KEY_LEFT_ALT));
    rb_define_const(keys, "LEFT_SUPER", INT2NUM(GLFW_KEY_LEFT_SUPER));
    rb_define_const(keys, "RIGHT_SHIFT", INT2NUM(GLFW_KEY_RIGHT_SHIFT));
    rb_define_const(keys, "RIGHT_CONTROL", INT2NUM(GLFW_KEY_RIGHT_CONTROL));
    rb_define_const(keys, "RIGHT_ALT", INT2NUM(GLFW_KEY_RIGHT_ALT));
    rb_define_const(keys, "RIGHT_SUPER", INT2NUM(GLFW_KEY_RIGHT_SUPER));
    rb_define_const(keys, "MENU", INT2NUM(GLFW_KEY_MENU));

    // Modifier Keys
    rb_define_const(mod_keys, "SHIFT", INT2NUM(GLFW_MOD_SHIFT));
    rb_define_const(mod_keys, "CONTROL", INT2NUM(GLFW_MOD_CONTROL));
    rb_define_const(mod_keys, "ALT", INT2NUM(GLFW_MOD_ALT));
    rb_define_const(mod_keys, "SUPER", INT2NUM(GLFW_MOD_SUPER));
    rb_define_const(mod_keys, "CAPS_LOCK", INT2NUM(GLFW_MOD_CAPS_LOCK));
    rb_define_const(mod_keys, "NUM_LOCK", INT2NUM(GLFW_MOD_NUM_LOCK));
}

static VALUE rpg_input_trigger_p(VALUE module, VALUE sym) {
    RPGkeybinding *b = NULL;
    HASH_FIND(hh, bindings, &sym, sizeof(VALUE), b);
    if (b != NULL) {
        for (int i = 0; i < bindings->num_keys; i++) {
            if (key_state[b->keys[i]] == INPUT_STATE_TRIGGER) {
                return Qtrue;
            }
        }

        for (int i = 0; i < bindings->num_buttons; i++) {
            if (mouse_state[b->buttons[i]] == INPUT_STATE_TRIGGER) {
                return Qtrue;
            }
        }
    }
    return Qfalse;
}

static VALUE rpg_input_repeat_p(VALUE module, VALUE sym) {
    RPGkeybinding *b = NULL;
    HASH_FIND(hh, bindings, &sym, sizeof(VALUE), b);
    if (b != NULL) {
        for (int i = 0; i < bindings->num_keys; i++) {
            if (key_state[b->keys[i]] == INPUT_STATE_REPEAT) {
                return Qtrue;
            }
        }

        for (int i = 0; i < bindings->num_buttons; i++) {
            if (mouse_state[b->buttons[i]] == INPUT_STATE_REPEAT) {
                return Qtrue;
            }
        }
    }
    return Qfalse;
}

static VALUE rpg_input_press_p(VALUE module, VALUE sym) {
    RPGkeybinding *b = NULL;
    HASH_FIND(hh, bindings, &sym, sizeof(VALUE), b);
    if (b != NULL) {
        for (int i = 0; i < bindings->num_keys; i++) {
            if (key_state[b->keys[i]] > INPUT_STATE_RELEASE) {
                return Qtrue;
            }
        }

        for (int i = 0; i < bindings->num_buttons; i++) {
            if (mouse_state[b->buttons[i]] > INPUT_STATE_RELEASE) {
                return Qtrue;
            }
        }
    }
    return Qfalse;
}

static VALUE rpg_input_release_p(VALUE module, VALUE sym) {
    RPGkeybinding *b = NULL;
    HASH_FIND(hh, bindings, &sym, sizeof(VALUE), b);
    if (b != NULL) {
        for (int i = 0; i < bindings->num_keys; i++) {
            if (key_state[b->keys[i]] == INPUT_STATE_RELEASE) {
                return Qtrue;
            }
        }

        for (int i = 0; i < bindings->num_buttons; i++) {
            if (mouse_state[b->buttons[i]] == INPUT_STATE_RELEASE) {
                return Qtrue;
            }
        }
    }
    return Qfalse;
}

static VALUE rpg_input_bind(VALUE module, VALUE sym, VALUE keys, VALUE buttons) {
    Check_Type(sym, T_SYMBOL);
    RPGkeybinding *binding = ALLOC(RPGkeybinding), *existing = NULL;
    memset(binding, 0, sizeof(RPGkeybinding));
    binding->symbol = sym;

    long num;
    if (RB_TYPE_P(keys, T_ARRAY)) {
        num = rb_array_len(keys);
        if (num > 0) {
            binding->num_keys = (int)num;
            binding->keys = xmalloc(sizeof(int) * num);
            for (long i = 0; i < num; i++) {
                int key = NUM2INT(rb_ary_entry(keys, i));
                if (key < KEY_FIRST || key > KEY_LAST) {
                    rb_raise(rb_eArgError, "key value out of range (given %d, expected %d..%d)", key, KEY_FIRST, KEY_LAST);
                }
                binding->keys[i] = key;
            }
        }
    }

    if (RB_TYPE_P(buttons, T_ARRAY)) {
        num = rb_array_len(buttons);
        if (num > 0) {
            binding->num_buttons = (int)num;
            binding->buttons = xmalloc(sizeof(int) * num);
            for (long i = 0; i < num; i++) {
                int button = NUM2INT(rb_ary_entry(buttons, i));
                if (button < GLFW_MOUSE_BUTTON_1 || button > GLFW_MOUSE_BUTTON_8) {
                    rb_raise(rb_eArgError, "button value out of range (given %d, expected %d..%d)", button, GLFW_MOUSE_BUTTON_1,
                             GLFW_MOUSE_BUTTON_8);
                }
                binding->buttons[i] = button;
            }
        }
    }

    HASH_REPLACE(hh, bindings, symbol, sizeof(VALUE), binding, existing);
    if (existing != NULL) {
        if (existing->keys != NULL) {
            xfree(existing->keys);
        }
        if (existing->buttons != NULL) {
            xfree(existing->buttons);
        }
        xfree(existing);
    }
    return Qnil;
}

static VALUE rpg_input_unbind(VALUE module, VALUE sym) {
    RPGkeybinding *binding = NULL;
    HASH_FIND(hh, bindings, &sym, sizeof(VALUE), binding);
    if (binding != NULL) {
        HASH_DEL(bindings, binding);
        xfree(binding);
        return Qtrue;
    }
    return Qfalse;
}

static VALUE rpg_input_each_binding(VALUE module) {
    rb_need_block();

    RPGkeybinding *binding = NULL;
    for (binding = bindings; binding != NULL; binding = binding->hh.next) {
        VALUE keys = rb_ary_new_capa(binding->num_keys);
        VALUE buttons = rb_ary_new_capa(binding->num_buttons);

        if (binding->num_keys > 0) {
            for (long i = 0; i < binding->num_keys; i++) {
                rb_ary_store(keys, i, INT2NUM(binding->keys[i]));
            }
        }
        if (binding->num_buttons > 0) {
            for (long i = 0; i < binding->num_buttons; i++) {
                rb_ary_store(buttons, i, INT2NUM(binding->buttons[i]));
            }
        }
        rb_yield_values(3, binding->symbol, keys, buttons);
    }
    return Qnil;
}

VALUE rpg_input_update(VALUE module) {

    // Key States
    for (int i = KEY_FIRST; i <= KEY_LAST; i++) {
        if (key_state[i] == INPUT_STATE_NONE) {
            continue;
        }
        if (key_state[i] == INPUT_STATE_TRIGGER) {
            key_state[i] = INPUT_STATE_PRESS;
        } else if (key_state[i] == INPUT_STATE_REPEAT) {
            key_state[i] = INPUT_STATE_PRESS;
        } else if (key_state[i] == INPUT_STATE_RELEASE) {
            key_state[i] = INPUT_STATE_NONE;
        }
    }

    // Mouse Button States
    for (int i = 0; i <= GLFW_MOUSE_BUTTON_LAST; i++) {
        if (mouse_state[i] == INPUT_STATE_NONE) {
            continue;
        }
        if (mouse_state[i] == INPUT_STATE_TRIGGER) {
            mouse_state[i] = INPUT_STATE_PRESS;
        } else if (mouse_state[i] == INPUT_STATE_REPEAT) {
            mouse_state[i] = INPUT_STATE_PRESS;
        } else if (mouse_state[i] == INPUT_STATE_RELEASE) {
            mouse_state[i] = INPUT_STATE_NONE;
        }
    }

    scroll_x = 0.0;
    scroll_y = 0.0;
}

void rpg_input_mouse_capture_cb(GLFWwindow *window, int entered) {
    if (entered && cb_mouse_enter != Qnil) {
        rb_proc_call(cb_mouse_enter, Qnil);
    } else if (!entered && cb_mouse_leave != Qnil) {
        rb_proc_call(cb_mouse_leave, Qnil);
    }
}

void rpg_input_mouse_scroll_cb(GLFWwindow *window, double x, double y) {
    scroll_x += x;
    scroll_y += y;
}

void rpg_input_mouse_move_cb(GLFWwindow *window, double x, double y) {
    if (cb_mouse_move == Qnil) {
        return;
    }
    VALUE ary = rb_ary_new_capa(2);
    rb_ary_store(ary, 0, DBL2NUM(x));
    rb_ary_store(ary, 1, DBL2NUM(y));
    rb_proc_call(cb_mouse_move, ary);
}

void rpg_input_key_cb(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        key_state[key] = INPUT_STATE_TRIGGER;
    } else if (action == GLFW_REPEAT) {
        key_state[key] = INPUT_STATE_REPEAT;
    } else {
        key_state[key] = INPUT_STATE_RELEASE;
    }
    if (cb_key != Qnil) {
        VALUE args = rb_ary_new_capa(4);
        rb_ary_store(args, 0, INT2NUM(key));
        rb_ary_store(args, 1, INT2NUM(scancode));
        rb_ary_store(args, 2, INT2NUM(action));
        rb_ary_store(args, 3, INT2NUM(mods));
        rb_proc_call(cb_key, args);
    }
}

void rpg_input_mouse_cb(GLFWwindow *window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        mouse_state[button] = INPUT_STATE_TRIGGER;
    } else if (action == GLFW_REPEAT) {
        mouse_state[button] = INPUT_STATE_REPEAT;
    } else {
        mouse_state[button] = INPUT_STATE_RELEASE;
    }
    if (cb_mouse_button != Qnil) {
        VALUE args = rb_ary_new_capa(3);
        rb_ary_store(args, 0, INT2NUM(button));
        rb_ary_store(args, 1, INT2NUM(action));
        rb_ary_store(args, 2, INT2NUM(mods));
        rb_proc_call(cb_mouse_button, args);
    }
}

// Keyboard

static VALUE rpg_kb_trigger_p(VALUE module, VALUE key) { return key_state[NUM2INT(key)] == INPUT_STATE_TRIGGER ? Qtrue : Qfalse; }

static VALUE rpg_kb_repeat_p(VALUE module, VALUE key) { return key_state[NUM2INT(key)] == INPUT_STATE_REPEAT ? Qtrue : Qfalse; }

static VALUE rpg_kb_release_p(VALUE module, VALUE key) { return key_state[NUM2INT(key)] == INPUT_STATE_RELEASE ? Qtrue : Qfalse; }

static VALUE rpg_kb_press_p(VALUE module, VALUE key) { return key_state[NUM2INT(key)] > INPUT_STATE_RELEASE ? Qtrue : Qfalse; }

static VALUE rpg_kb_key_name(VALUE module, VALUE key) {
    const char *name = glfwGetKeyName(NUM2INT(key), 0);
    return name ? rb_str_new_cstr(name) : Qnil;
}

static VALUE rpg_kb_scancode_name(VALUE module, VALUE scancode) {
    const char *name = glfwGetKeyName(GLFW_KEY_UNKNOWN, NUM2INT(scancode));
    return name ? rb_str_new_cstr(name) : Qnil;
}

static VALUE rpg_kb_key_scancode(VALUE module, VALUE key) {
    int scancode = glfwGetKeyScancode(NUM2INT(key));
    return INT2NUM(scancode);
}

// Mouse

static VALUE rpg_mouse_scroll(VALUE module) {
    RPGvector2 *v = ALLOC(RPGvector2);
    v->x = (GLfloat)scroll_x;
    v->x = (GLfloat)scroll_x;
    return Data_Wrap_Struct(rb_cVector2, NULL, RUBY_DEFAULT_FREE, v);
}

static VALUE rpg_mouse_scroll_x(VALUE module) { return DBL2NUM(scroll_x); }

static VALUE rpg_mouse_scroll_y(VALUE module) { return DBL2NUM(scroll_y); }

static VALUE rpg_mouse_trigger_p(VALUE module, VALUE key) { return mouse_state[NUM2INT(key)] == INPUT_STATE_TRIGGER ? Qtrue : Qfalse; }

static VALUE rpg_mouse_repeat_p(VALUE module, VALUE key) { return mouse_state[NUM2INT(key)] == INPUT_STATE_REPEAT ? Qtrue : Qfalse; }

static VALUE rpg_mouse_release_p(VALUE module, VALUE key) { return mouse_state[NUM2INT(key)] == INPUT_STATE_RELEASE ? Qtrue : Qfalse; }

static VALUE rpg_mouse_press_p(VALUE module, VALUE key) { return mouse_state[NUM2INT(key)] > INPUT_STATE_RELEASE ? Qtrue : Qfalse; }

static VALUE rpg_mouse_cursor_mode(int argc, VALUE *argv, VALUE module) {
    VALUE mode;
    rb_scan_args(argc, argv, "01", &mode);
    int m = GLFW_CURSOR_NORMAL;
    if (RTEST(mode)) {
        if (mode == STR2SYM("hidden")) {
            m = GLFW_CURSOR_HIDDEN;
        } else if (mode == STR2SYM("disabled")) {
            m = GLFW_CURSOR_DISABLED;
        }
    }
    glfwSetInputMode(game_window, GLFW_CURSOR, m);
    return Qnil;
}

static VALUE rpg_mouse_change_cursor(int argc, VALUE *argv, VALUE module) {
    VALUE cursor, x_hot, y_hot;
    rb_scan_args(argc, argv, "12", &cursor, &x_hot, &y_hot);

    if (mouse_cursor != NULL) {
        glfwDestroyCursor(mouse_cursor);
    }

    if (NIL_P(cursor)) {
        mouse_cursor = NULL;
    } else if (FIXNUM_P(cursor)) {
        int shape = NUM2INT(cursor);
        mouse_cursor = glfwCreateStandardCursor(shape);
    } else {
        GLFWimage *img = NULL;
        if (RB_IS_A(cursor, rb_cImage)) {
            RPGimage *image = DATA_PTR(cursor);
            img = ALLOC(GLFWimage);
            img->width = image->width;
            img->height = image->height;
            int size;
            img->pixels = rpg_image_pixels(image, &size);

        } else {
            const char *path = StringValueCStr(cursor);
            img = ALLOC(GLFWimage);
            img->pixels = rpg_image_load(path, &img->width, &img->height);
            if (img->pixels == NULL) {
                rb_raise(rb_eRPGError, "failed to load image");
            }
        }
        int x = NIL_P(x_hot) ? 0 : NUM2INT(x_hot);
        int y = NIL_P(y_hot) ? 0 : NUM2INT(y_hot);
        mouse_cursor = glfwCreateCursor(img, x, y);
        xfree(img->pixels);
        xfree(img);
    }
    glfwSetCursor(game_window, mouse_cursor);
    return Qnil;
}

static VALUE rpg_mouse_location(VALUE module) {
    double x, y;
    glfwGetCursorPos(game_window, &x, &y);
    RPGpoint *point = ALLOC(RPGpoint);
    if (game_ratio_x < game_ratio_y) {
        point->x = (GLint)round(x / game_ratio_x);
        point->y = (GLint)round(y / game_ratio_y) - bounds.y;
    } else {
        point->x = (GLint)round(x / game_ratio_y) - bounds.x;
        point->y = (GLint)round(y / game_ratio_y);
    }
    return Data_Wrap_Struct(rb_cPoint, NULL, RUBY_DEFAULT_FREE, point);
}

static VALUE rpg_mouse_x(VALUE module) {
    double x;
    glfwGetCursorPos(game_window, &x, NULL);
    if (game_ratio_x < game_ratio_y) {
        return INT2NUM((GLint)round(x / game_ratio_x));
    } else {
        return INT2NUM((GLint)round(x / game_ratio_y) - bounds.x);
    }
}

static VALUE rpg_mouse_y(VALUE module) {
    double y;
    glfwGetCursorPos(game_window, NULL, &y);
    if (game_ratio_x < game_ratio_y) {
        return INT2NUM((GLint)round(y / game_ratio_x) - bounds.y);
    } else {
        return INT2NUM((GLint)round(y / game_ratio_y));
    }
}

static VALUE rpg_input_on_mouse_move(VALUE module) {
    if (rb_block_given_p()) {
        cb_mouse_move = rb_block_proc();
        glfwSetCursorPosCallback(game_window, rpg_input_mouse_move_cb);
    } else {
        cb_mouse_move = Qnil;
        glfwSetCursorPosCallback(game_window, NULL);
    }
    return Qnil;
}

static VALUE rpg_input_on_key(VALUE module) {
    cb_key = rb_block_given_p() ? rb_block_proc() : Qnil;
    return Qnil;
}

static VALUE rpg_input_on_mouse_button(VALUE module) {
    cb_mouse_button = rb_block_given_p() ? rb_block_proc() : Qnil;
    return Qnil;
}

static VALUE rpg_input_on_mouse_enter(VALUE module) {
    cb_mouse_enter = rb_block_given_p() ? rb_block_proc() : Qnil;
    return Qnil;
}

static VALUE rpg_input_on_mouse_leave(VALUE module) {
    cb_mouse_leave = rb_block_given_p() ? rb_block_proc() : Qnil;
    return Qnil;
}

static VALUE rpg_input_on_text(VALUE module) {
    if (rb_block_given_p()) {
        cb_text = rb_block_proc();
        glfwSetCharCallback(game_window, rpg_input_text_cb);
    } else {
        cb_text = Qnil;
        glfwSetCharCallback(game_window, NULL);
    }
    return Qnil;
}

void rpg_input_text_cb(GLFWwindow *window, unsigned int cp) {
    if (cb_text != Qnil) {
        VALUE args = rb_Array(UINT2NUM(cp));
        rb_proc_call(cb_text, args);
    }
}