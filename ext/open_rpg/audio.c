#include "./rpg.h"

VALUE rb_mAudio;

static VALUE rpg_audio_terminate(VALUE module) {

    return Qnil;
}

void rpg_audio_init(VALUE parent) {
    rb_mAudio = rb_define_module_under(parent, "Audio");
    rb_define_singleton_method(rb_mAudio, "terminate", rpg_audio_terminate, 0);

}