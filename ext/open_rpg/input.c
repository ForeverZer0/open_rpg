#include "input.h"

VALUE rb_mInput;
VALUE rb_mKeyboard;
VALUE rb_mMouse;
VALUE rb_mGamepad;
VALUE rb_mKey;
VALUE rb_mModKey;

char key_state[GLFW_KEY_LAST + 1];
char mouse_state[GLFW_MOUSE_BUTTON_LAST + 1];

void rpg_input_init(VALUE parent) {

    rb_mInput = rb_define_module_under(parent, "Input");
    rb_mKeyboard = rb_define_module_under(rb_mInput, "Keyboard");
    rb_mMouse = rb_define_module_under(rb_mInput, "Mouse");
    rb_mGamepad = rb_define_module_under(rb_mInput, "Gamepad");
    rb_mKey = rb_define_module_under(rb_mInput, "Key");
    rb_mModKey = rb_define_module_under(rb_mInput, "ModKey");

    memset(&key_state, 0, sizeof(key_state));
    memset(&mouse_state, 0, sizeof(mouse_state));

    rb_define_singleton_method(rb_mKeyboard, "trigger?", rpg_kb_trigger_p, 1);
    rb_define_singleton_method(rb_mKeyboard, "repeat?", rpg_kb_repeat_p, 1);
    rb_define_singleton_method(rb_mKeyboard, "release?", rpg_kb_release_p, 1);
    rb_define_singleton_method(rb_mKeyboard, "press?", rpg_kb_press_p, 1);

    rb_define_singleton_method(rb_mMouse, "trigger?", rpg_mouse_trigger_p, 1);
    rb_define_singleton_method(rb_mMouse, "repeat?", rpg_mouse_repeat_p, 1);
    rb_define_singleton_method(rb_mMouse, "release?", rpg_mouse_release_p, 1);
    rb_define_singleton_method(rb_mMouse, "press?", rpg_mouse_press_p, 1);

    // Gamepad Constants
    rb_define_const(rb_mGamepad, "A", INT2NUM(GLFW_GAMEPAD_BUTTON_A));
    rb_define_const(rb_mGamepad, "B", INT2NUM(GLFW_GAMEPAD_BUTTON_B));
    rb_define_const(rb_mGamepad, "X", INT2NUM(GLFW_GAMEPAD_BUTTON_X));
    rb_define_const(rb_mGamepad, "Y", INT2NUM(GLFW_GAMEPAD_BUTTON_Y));
    rb_define_const(rb_mGamepad, "LEFT_BUMPER", INT2NUM(GLFW_GAMEPAD_BUTTON_LEFT_BUMPER));
    rb_define_const(rb_mGamepad, "RIGHT_BUMPER", INT2NUM(GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER));
    rb_define_const(rb_mGamepad, "BACK", INT2NUM(GLFW_GAMEPAD_BUTTON_BACK));
    rb_define_const(rb_mGamepad, "START", INT2NUM(GLFW_GAMEPAD_BUTTON_START));
    rb_define_const(rb_mGamepad, "GUIDE", INT2NUM(GLFW_GAMEPAD_BUTTON_GUIDE));
    rb_define_const(rb_mGamepad, "LEFT_THUMB", INT2NUM(GLFW_GAMEPAD_BUTTON_LEFT_THUMB));
    rb_define_const(rb_mGamepad, "RIGHT_THUMB", INT2NUM(GLFW_GAMEPAD_BUTTON_RIGHT_THUMB));
    rb_define_const(rb_mGamepad, "DPAD_UP", INT2NUM(GLFW_GAMEPAD_BUTTON_DPAD_UP));
    rb_define_const(rb_mGamepad, "DPAD_RIGHT", INT2NUM(GLFW_GAMEPAD_BUTTON_DPAD_RIGHT));
    rb_define_const(rb_mGamepad, "DPAD_DOWN", INT2NUM(GLFW_GAMEPAD_BUTTON_DPAD_DOWN));
    rb_define_const(rb_mGamepad, "DPAD_LEFT", INT2NUM(GLFW_GAMEPAD_BUTTON_DPAD_LEFT));
    rb_define_const(rb_mGamepad, "DPAD_CROSS", INT2NUM(GLFW_GAMEPAD_BUTTON_CROSS));
    rb_define_const(rb_mGamepad, "DPAD_CIRCLE", INT2NUM(GLFW_GAMEPAD_BUTTON_CIRCLE));
    rb_define_const(rb_mGamepad, "DPAD_SQUARE", INT2NUM(GLFW_GAMEPAD_BUTTON_SQUARE));
    rb_define_const(rb_mGamepad, "DPAD_TRIANGLE", INT2NUM(GLFW_GAMEPAD_BUTTON_TRIANGLE));

    // Mouse Constants
    rb_define_const(rb_mMouse, "BUTTON_1", INT2NUM(GLFW_MOUSE_BUTTON_1));
    rb_define_const(rb_mMouse, "BUTTON_2", INT2NUM(GLFW_MOUSE_BUTTON_2));
    rb_define_const(rb_mMouse, "BUTTON_3", INT2NUM(GLFW_MOUSE_BUTTON_3));
    rb_define_const(rb_mMouse, "BUTTON_4", INT2NUM(GLFW_MOUSE_BUTTON_4));
    rb_define_const(rb_mMouse, "BUTTON_5", INT2NUM(GLFW_MOUSE_BUTTON_5));
    rb_define_const(rb_mMouse, "BUTTON_6", INT2NUM(GLFW_MOUSE_BUTTON_6));
    rb_define_const(rb_mMouse, "BUTTON_7", INT2NUM(GLFW_MOUSE_BUTTON_7));
    rb_define_const(rb_mMouse, "BUTTON_8", INT2NUM(GLFW_MOUSE_BUTTON_8));
    rb_define_const(rb_mMouse, "LEFT", INT2NUM(GLFW_MOUSE_BUTTON_LEFT));
    rb_define_const(rb_mMouse, "RIGHT", INT2NUM(GLFW_MOUSE_BUTTON_RIGHT));
    rb_define_const(rb_mMouse, "MIDDLE", INT2NUM(GLFW_MOUSE_BUTTON_MIDDLE));

    // Key Constants
    rb_define_const(rb_mKey, "SPACE", INT2NUM(GLFW_KEY_SPACE));
    rb_define_const(rb_mKey, "APOSTROPHE", INT2NUM(GLFW_KEY_APOSTROPHE));
    rb_define_const(rb_mKey, "COMMA", INT2NUM(GLFW_KEY_COMMA));
    rb_define_const(rb_mKey, "MINUS", INT2NUM(GLFW_KEY_MINUS));
    rb_define_const(rb_mKey, "PERIOD", INT2NUM(GLFW_KEY_PERIOD));
    rb_define_const(rb_mKey, "SLASH", INT2NUM(GLFW_KEY_SLASH));
    rb_define_const(rb_mKey, "NUM_0", INT2NUM(GLFW_KEY_0));
    rb_define_const(rb_mKey, "NUM_1", INT2NUM(GLFW_KEY_1));
    rb_define_const(rb_mKey, "NUM_2", INT2NUM(GLFW_KEY_2));
    rb_define_const(rb_mKey, "NUM_3", INT2NUM(GLFW_KEY_3));
    rb_define_const(rb_mKey, "NUM_4", INT2NUM(GLFW_KEY_4));
    rb_define_const(rb_mKey, "NUM_5", INT2NUM(GLFW_KEY_5));
    rb_define_const(rb_mKey, "NUM_6", INT2NUM(GLFW_KEY_6));
    rb_define_const(rb_mKey, "NUM_7", INT2NUM(GLFW_KEY_7));
    rb_define_const(rb_mKey, "NUM_8", INT2NUM(GLFW_KEY_8));
    rb_define_const(rb_mKey, "NUM_9", INT2NUM(GLFW_KEY_9));
    rb_define_const(rb_mKey, "SEMICOLON", INT2NUM(GLFW_KEY_SEMICOLON));
    rb_define_const(rb_mKey, "EQUAL", INT2NUM(GLFW_KEY_EQUAL));
    rb_define_const(rb_mKey, "A", INT2NUM(GLFW_KEY_A));
    rb_define_const(rb_mKey, "B", INT2NUM(GLFW_KEY_B));
    rb_define_const(rb_mKey, "C", INT2NUM(GLFW_KEY_C));
    rb_define_const(rb_mKey, "D", INT2NUM(GLFW_KEY_D));
    rb_define_const(rb_mKey, "E", INT2NUM(GLFW_KEY_E));
    rb_define_const(rb_mKey, "F", INT2NUM(GLFW_KEY_F));
    rb_define_const(rb_mKey, "G", INT2NUM(GLFW_KEY_G));
    rb_define_const(rb_mKey, "H", INT2NUM(GLFW_KEY_H));
    rb_define_const(rb_mKey, "I", INT2NUM(GLFW_KEY_I));
    rb_define_const(rb_mKey, "J", INT2NUM(GLFW_KEY_J));
    rb_define_const(rb_mKey, "K", INT2NUM(GLFW_KEY_K));
    rb_define_const(rb_mKey, "L", INT2NUM(GLFW_KEY_L));
    rb_define_const(rb_mKey, "M", INT2NUM(GLFW_KEY_M));
    rb_define_const(rb_mKey, "N", INT2NUM(GLFW_KEY_N));
    rb_define_const(rb_mKey, "O", INT2NUM(GLFW_KEY_O));
    rb_define_const(rb_mKey, "P", INT2NUM(GLFW_KEY_P));
    rb_define_const(rb_mKey, "Q", INT2NUM(GLFW_KEY_Q));
    rb_define_const(rb_mKey, "R", INT2NUM(GLFW_KEY_R));
    rb_define_const(rb_mKey, "S", INT2NUM(GLFW_KEY_S));
    rb_define_const(rb_mKey, "T", INT2NUM(GLFW_KEY_T));
    rb_define_const(rb_mKey, "U", INT2NUM(GLFW_KEY_U));
    rb_define_const(rb_mKey, "V", INT2NUM(GLFW_KEY_V));
    rb_define_const(rb_mKey, "W", INT2NUM(GLFW_KEY_W));
    rb_define_const(rb_mKey, "X", INT2NUM(GLFW_KEY_X));
    rb_define_const(rb_mKey, "Y", INT2NUM(GLFW_KEY_Y));
    rb_define_const(rb_mKey, "Z", INT2NUM(GLFW_KEY_Z));
    rb_define_const(rb_mKey, "LEFT_BRACKET", INT2NUM(GLFW_KEY_LEFT_BRACKET));
    rb_define_const(rb_mKey, "BACKSLASH", INT2NUM(GLFW_KEY_BACKSLASH));
    rb_define_const(rb_mKey, "RIGHT_BRACKET", INT2NUM(GLFW_KEY_RIGHT_BRACKET));
    rb_define_const(rb_mKey, "GRAVE_ACCENT", INT2NUM(GLFW_KEY_GRAVE_ACCENT));
    rb_define_const(rb_mKey, "WORLD_1", INT2NUM(GLFW_KEY_WORLD_1));
    rb_define_const(rb_mKey, "WORLD_2", INT2NUM(GLFW_KEY_WORLD_2));
    rb_define_const(rb_mKey, "ESCAPE", INT2NUM(GLFW_KEY_ESCAPE));
    rb_define_const(rb_mKey, "ENTER", INT2NUM(GLFW_KEY_ENTER));
    rb_define_const(rb_mKey, "TAB", INT2NUM(GLFW_KEY_TAB));
    rb_define_const(rb_mKey, "BACKSPACE", INT2NUM(GLFW_KEY_BACKSPACE));
    rb_define_const(rb_mKey, "INSERT", INT2NUM(GLFW_KEY_INSERT));
    rb_define_const(rb_mKey, "DELETE", INT2NUM(GLFW_KEY_DELETE));
    rb_define_const(rb_mKey, "RIGHT", INT2NUM(GLFW_KEY_RIGHT));
    rb_define_const(rb_mKey, "LEFT", INT2NUM(GLFW_KEY_LEFT));
    rb_define_const(rb_mKey, "DOWN", INT2NUM(GLFW_KEY_DOWN));
    rb_define_const(rb_mKey, "UP", INT2NUM(GLFW_KEY_UP));
    rb_define_const(rb_mKey, "PAGE_UP", INT2NUM(GLFW_KEY_PAGE_UP));
    rb_define_const(rb_mKey, "PAGE_DOWN", INT2NUM(GLFW_KEY_PAGE_DOWN));
    rb_define_const(rb_mKey, "HOME", INT2NUM(GLFW_KEY_HOME));
    rb_define_const(rb_mKey, "END", INT2NUM(GLFW_KEY_END));
    rb_define_const(rb_mKey, "CAPS_LOCK", INT2NUM(GLFW_KEY_CAPS_LOCK));
    rb_define_const(rb_mKey, "SCROLL_LOCK", INT2NUM(GLFW_KEY_SCROLL_LOCK));
    rb_define_const(rb_mKey, "NUM_LOCK", INT2NUM(GLFW_KEY_NUM_LOCK));
    rb_define_const(rb_mKey, "PRINT_SCREEN", INT2NUM(GLFW_KEY_PRINT_SCREEN));
    rb_define_const(rb_mKey, "PAUSE", INT2NUM(GLFW_KEY_PAUSE));
    rb_define_const(rb_mKey, "F1", INT2NUM(GLFW_KEY_F1));
    rb_define_const(rb_mKey, "F2", INT2NUM(GLFW_KEY_F2));
    rb_define_const(rb_mKey, "F3", INT2NUM(GLFW_KEY_F3));
    rb_define_const(rb_mKey, "F4", INT2NUM(GLFW_KEY_F4));
    rb_define_const(rb_mKey, "F5", INT2NUM(GLFW_KEY_F5));
    rb_define_const(rb_mKey, "F6", INT2NUM(GLFW_KEY_F6));
    rb_define_const(rb_mKey, "F7", INT2NUM(GLFW_KEY_F7));
    rb_define_const(rb_mKey, "F8", INT2NUM(GLFW_KEY_F8));
    rb_define_const(rb_mKey, "F9", INT2NUM(GLFW_KEY_F9));
    rb_define_const(rb_mKey, "F10", INT2NUM(GLFW_KEY_F10));
    rb_define_const(rb_mKey, "F11", INT2NUM(GLFW_KEY_F11));
    rb_define_const(rb_mKey, "F12", INT2NUM(GLFW_KEY_F12));
    rb_define_const(rb_mKey, "F13", INT2NUM(GLFW_KEY_F13));
    rb_define_const(rb_mKey, "F14", INT2NUM(GLFW_KEY_F14));
    rb_define_const(rb_mKey, "F15", INT2NUM(GLFW_KEY_F15));
    rb_define_const(rb_mKey, "F16", INT2NUM(GLFW_KEY_F16));
    rb_define_const(rb_mKey, "F17", INT2NUM(GLFW_KEY_F17));
    rb_define_const(rb_mKey, "F18", INT2NUM(GLFW_KEY_F18));
    rb_define_const(rb_mKey, "F19", INT2NUM(GLFW_KEY_F19));
    rb_define_const(rb_mKey, "F20", INT2NUM(GLFW_KEY_F20));
    rb_define_const(rb_mKey, "F21", INT2NUM(GLFW_KEY_F21));
    rb_define_const(rb_mKey, "F22", INT2NUM(GLFW_KEY_F22));
    rb_define_const(rb_mKey, "F23", INT2NUM(GLFW_KEY_F23));
    rb_define_const(rb_mKey, "F24", INT2NUM(GLFW_KEY_F24));
    rb_define_const(rb_mKey, "F25", INT2NUM(GLFW_KEY_F25));
    rb_define_const(rb_mKey, "KP_0", INT2NUM(GLFW_KEY_KP_0));
    rb_define_const(rb_mKey, "KP_1", INT2NUM(GLFW_KEY_KP_1));
    rb_define_const(rb_mKey, "KP_2", INT2NUM(GLFW_KEY_KP_2));
    rb_define_const(rb_mKey, "KP_3", INT2NUM(GLFW_KEY_KP_3));
    rb_define_const(rb_mKey, "KP_4", INT2NUM(GLFW_KEY_KP_4));
    rb_define_const(rb_mKey, "KP_5", INT2NUM(GLFW_KEY_KP_5));
    rb_define_const(rb_mKey, "KP_6", INT2NUM(GLFW_KEY_KP_6));
    rb_define_const(rb_mKey, "KP_7", INT2NUM(GLFW_KEY_KP_7));
    rb_define_const(rb_mKey, "KP_8", INT2NUM(GLFW_KEY_KP_8));
    rb_define_const(rb_mKey, "KP_9", INT2NUM(GLFW_KEY_KP_9));
    rb_define_const(rb_mKey, "KP_DECIMAL", INT2NUM(GLFW_KEY_KP_DECIMAL));
    rb_define_const(rb_mKey, "KP_DIVIDE", INT2NUM(GLFW_KEY_KP_DIVIDE));
    rb_define_const(rb_mKey, "KP_MULTIPLY", INT2NUM(GLFW_KEY_KP_MULTIPLY));
    rb_define_const(rb_mKey, "KP_SUBTRACT", INT2NUM(GLFW_KEY_KP_SUBTRACT));
    rb_define_const(rb_mKey, "KP_ADD", INT2NUM(GLFW_KEY_KP_ADD));
    rb_define_const(rb_mKey, "KP_ENTER", INT2NUM(GLFW_KEY_KP_ENTER));
    rb_define_const(rb_mKey, "KP_EQUAL", INT2NUM(GLFW_KEY_KP_EQUAL));
    rb_define_const(rb_mKey, "LEFT_SHIFT", INT2NUM(GLFW_KEY_LEFT_SHIFT));
    rb_define_const(rb_mKey, "LEFT_CONTROL", INT2NUM(GLFW_KEY_LEFT_CONTROL));
    rb_define_const(rb_mKey, "LEFT_ALT", INT2NUM(GLFW_KEY_LEFT_ALT));
    rb_define_const(rb_mKey, "LEFT_SUPER", INT2NUM(GLFW_KEY_LEFT_SUPER));
    rb_define_const(rb_mKey, "RIGHT_SHIFT", INT2NUM(GLFW_KEY_RIGHT_SHIFT));
    rb_define_const(rb_mKey, "RIGHT_CONTROL", INT2NUM(GLFW_KEY_RIGHT_CONTROL));
    rb_define_const(rb_mKey, "RIGHT_ALT", INT2NUM(GLFW_KEY_RIGHT_ALT));
    rb_define_const(rb_mKey, "RIGHT_SUPER", INT2NUM(GLFW_KEY_RIGHT_SUPER));
    rb_define_const(rb_mKey, "MENU", INT2NUM(GLFW_KEY_MENU));

    // Modifier Keys
    rb_define_const(rb_mModKey, "SHIFT", INT2NUM(GLFW_MOD_SHIFT));
    rb_define_const(rb_mModKey, "CONTROL", INT2NUM(GLFW_MOD_CONTROL));
    rb_define_const(rb_mModKey, "ALT", INT2NUM(GLFW_MOD_ALT));
    rb_define_const(rb_mModKey, "SUPER", INT2NUM(GLFW_MOD_SUPER));
    rb_define_const(rb_mModKey, "CAPS_LOCK", INT2NUM(GLFW_MOD_CAPS_LOCK));
    rb_define_const(rb_mModKey, "NUM_LOCK", INT2NUM(GLFW_MOD_NUM_LOCK));
}

VALUE rpg_input_update(VALUE module) {

    // Key States
    for (int i = KEY_FIRST; i <= KEY_LAST; i++) {
        if (key_state[i] == INPUT_STATE_NONE) {
            continue;
        }
        if (key_state[i] == INPUT_STATE_TRIGGER) {
            key_state[i] = INPUT_STATE_PRESS;
        }
        else if (key_state[i] == INPUT_STATE_REPEAT) {
            key_state[i] = INPUT_STATE_PRESS;
        } 
        else if (key_state[i] == INPUT_STATE_RELEASE) {
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
        }
        else if (mouse_state[i] == INPUT_STATE_REPEAT) {
            mouse_state[i] = INPUT_STATE_PRESS;
        } 
        else if (mouse_state[i] == INPUT_STATE_RELEASE) {
            mouse_state[i] = INPUT_STATE_NONE;
        }
    }
}

void rpg_input_key_cb(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        key_state[key] = INPUT_STATE_TRIGGER;
    }
    else if (action == GLFW_REPEAT) {
        key_state[key] = INPUT_STATE_REPEAT;
    }
    else {
        key_state[key] = INPUT_STATE_RELEASE;
    }
}

void rpg_input_mouse_cb(GLFWwindow *window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        mouse_state[button] = INPUT_STATE_TRIGGER;
    }
    else if (action == GLFW_REPEAT) {
        mouse_state[button] = INPUT_STATE_REPEAT;
    }
    else {
        mouse_state[button] = INPUT_STATE_RELEASE;
    }
}

// Keyboard

static VALUE rpg_kb_trigger_p(VALUE module, VALUE key) {
    return key_state[NUM2INT(key)] == INPUT_STATE_TRIGGER ? Qtrue : Qfalse;
}

static VALUE rpg_kb_repeat_p(VALUE module, VALUE key) {
    return key_state[NUM2INT(key)] == INPUT_STATE_REPEAT ? Qtrue : Qfalse;
}

static VALUE rpg_kb_release_p(VALUE module, VALUE key) {
    return key_state[NUM2INT(key)] == INPUT_STATE_RELEASE ? Qtrue : Qfalse;
}

static VALUE rpg_kb_press_p(VALUE module, VALUE key) {
    return key_state[NUM2INT(key)] > INPUT_STATE_RELEASE ? Qtrue : Qfalse;
}

// Mouse

static VALUE rpg_mouse_trigger_p(VALUE module, VALUE key) {
    return mouse_state[NUM2INT(key)] == INPUT_STATE_TRIGGER ? Qtrue : Qfalse;
}

static VALUE rpg_mouse_repeat_p(VALUE module, VALUE key) {
    return mouse_state[NUM2INT(key)] == INPUT_STATE_REPEAT ? Qtrue : Qfalse;
}

static VALUE rpg_mouse_release_p(VALUE module, VALUE key) {
    return mouse_state[NUM2INT(key)] == INPUT_STATE_RELEASE ? Qtrue : Qfalse;
}

static VALUE rpg_mouse_press_p(VALUE module, VALUE key) {
    return mouse_state[NUM2INT(key)] > INPUT_STATE_RELEASE ? Qtrue : Qfalse;
}
