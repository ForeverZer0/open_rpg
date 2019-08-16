
#include "./internal.h"
#include <sndfile.h>

VALUE rb_cSound;

typedef struct RPGsound {
    SNDFILE *file;
    SF_INFO info;
} RPGsound;

ALLOC_FUNC(rpg_sound_alloc, RPGsound)

ATTR_READER(rpg_sound_channels, RPGsound, info.channels, INT2NUM)
ATTR_READER(rpg_sound_sample_rate, RPGsound, info.samplerate, INT2NUM)
ATTR_READER(rpg_sound_seekable, RPGsound, info.seekable, RB_BOOL)
ATTR_READER(rpg_sound_format, RPGsound, info.format, INT2NUM)
ATTR_READER(rpg_sound_sections, RPGsound, info.sections, INT2NUM)
ATTR_READER(rpg_sound_frames, RPGsound, info.frames, LL2NUM)

static VALUE rpg_sound_dispose(VALUE self) {
    RPGsound *snd = DATA_PTR(self);
    if (snd->file) {
        sf_close(snd->file);
    }
    return Qnil;
}

static VALUE rpg_sound_from_file(VALUE klass, VALUE path) {
    RPGsound *snd = ALLOC(RPGsound);
    char *fname = StringValueCStr(path);
    snd->file = sf_open(fname, SFM_READ, &snd->info); // TODO: If path doesn't exists?
    int code = sf_error(snd->file);
    if (code != SF_ERR_NO_ERROR) {
        const char *msg = sf_strerror(snd->file);
        rb_raise(rb_eRPGError, "%s", msg);
    }
    if (snd->file == NULL) {
        rb_raise(rb_eArgError, "failed to load audio file");
    }
    return Data_Wrap_Struct(klass, NULL, RUBY_DEFAULT_FREE, snd);
}

static VALUE rpg_sound_inspect(VALUE self) {
    RPGsound *snd = DATA_PTR(self);
    return rb_sprintf("<Sound: channels:%d, samples:%d, rate:%d>", snd->info.channels, snd->info.frames, snd->info.samplerate);
}

static VALUE rpg_sound_duration(VALUE self) {
    RPGsound *snd = DATA_PTR(self);
    return DBL2NUM((1000.0 / snd->info.samplerate) * snd->info.frames);
}

void rpg_sound_init(VALUE parent) {
    rb_cSound = rb_define_class_under(parent, "Sound", rb_cObject);
    rb_define_alloc_func(rb_cSound, rpg_sound_alloc);
    rb_define_method(rb_cSound, "dispose", rpg_sound_dispose, 0);

    rb_define_method(rb_cSound, "duration", rpg_sound_duration, 0);
    rb_define_method(rb_cSound, "channels", rpg_sound_channels, 0);
    rb_define_method(rb_cSound, "sample_rate", rpg_sound_sample_rate, 0);
    rb_define_method(rb_cSound, "format", rpg_sound_format, 0);
    rb_define_method(rb_cSound, "seekable?", rpg_sound_seekable, 0);
    rb_define_method(rb_cSound, "sections", rpg_sound_sections, 0);
    rb_define_method(rb_cSound, "samples", rpg_sound_frames, 0);
    rb_define_method(rb_cSound, "inspect", rpg_sound_inspect, 0);

    rb_define_singleton_method(rb_cSound, "from_file", rpg_sound_from_file, 1);

    // TODO: Seek
}