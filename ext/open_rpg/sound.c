
#include "./internal.h"
#include "AL/alext.h"

VALUE rb_cSound;

ALLOC_FUNC(rpg_sound_alloc, RPGsound)
ATTR_READER(rpg_sound_channels, RPGsound, info.channels, INT2NUM)
ATTR_READER(rpg_sound_sample_rate, RPGsound, info.samplerate, INT2NUM)
ATTR_READER(rpg_sound_seekable, RPGsound, info.seekable, RB_BOOL)
ATTR_READER(rpg_sound_format, RPGsound, info.format, INT2NUM)
ATTR_READER(rpg_sound_sections, RPGsound, info.sections, INT2NUM)
ATTR_READER(rpg_sound_frames, RPGsound, info.frames, LL2NUM)

static int rpg_sound_al_format(SF_INFO *info, size_t *sample_size) {

    int subtype = info->format & SF_FORMAT_SUBMASK;
    if (info->channels == 1) {
        switch (subtype) {
            case SF_FORMAT_PCM_U8:
            case SF_FORMAT_PCM_S8: 
                *sample_size = sizeof(ALbyte);
                return AL_FORMAT_MONO8;
            case SF_FORMAT_PCM_16: 
                *sample_size = sizeof(ALshort);
                return AL_FORMAT_MONO16;
            case SF_FORMAT_DOUBLE: 
                *sample_size = sizeof(ALdouble);
                return AL_FORMAT_MONO_DOUBLE_EXT; 
            case SF_FORMAT_FLOAT:
            case SF_FORMAT_VORBIS:
            default:
                *sample_size = sizeof(ALfloat);
                return AL_FORMAT_MONO_FLOAT32;
        }
    }
    if (info->channels == 2) {
        switch (subtype) {

            case SF_FORMAT_PCM_U8:
            case SF_FORMAT_PCM_S8: 
                *sample_size = sizeof(ALbyte);
                return AL_FORMAT_STEREO8;
            case SF_FORMAT_PCM_16: 
                *sample_size = sizeof(ALshort);
                return AL_FORMAT_STEREO16;
            case SF_FORMAT_DOUBLE: 
                *sample_size = sizeof(ALdouble);
                return AL_FORMAT_STEREO_DOUBLE_EXT; 

            case SF_FORMAT_FLOAT:
            case SF_FORMAT_VORBIS: 
            default:
                *sample_size = sizeof(ALfloat);
                return AL_FORMAT_STEREO_FLOAT32;
        }
    }
    // TODO: Implement channels > 2


    return info->channels == 1 ? AL_FORMAT_MONO_FLOAT32 : AL_FORMAT_STEREO_FLOAT32;
}

int rpg_sound_load(const char *fname, RPGsound *sound) {
    RPG_THROW_UNLESS_FILE(fname);
    sound->file = sf_open(fname, SFM_READ, &sound->info);
    int code = sf_error(sound->file);
    sound->al_format = rpg_sound_al_format(&sound->info, &sound->sample_size);
    return code;
}

static VALUE rpg_sound_dispose(VALUE self) {
    RPGsound *snd = DATA_PTR(self);
    if (snd->file) {
        sf_close(snd->file);
        snd->file = NULL;
    }
    return Qnil;
}

static VALUE rpg_sound_initialize(VALUE self, VALUE path) {
    RPGsound *snd = DATA_PTR(self);
    char *fname = StringValueCStr(path);
    if (rpg_sound_load(fname, snd)) {
        const char *msg = sf_strerror(snd->file);
        rb_raise(rb_eRPGError, "failed to load audio file - %s", msg);
    }
    return Qnil;
}

static VALUE rpg_sound_inspect(VALUE self) {
    RPGsound *snd = DATA_PTR(self);
    return rb_sprintf("<Sound: channels:%d, samples:%d, rate:%d>", snd->info.channels, snd->info.frames, snd->info.samplerate);
}

static VALUE rpg_sound_duration(VALUE self) {
    RPGsound *snd = DATA_PTR(self);
    return DBL2NUM((1000.0 / snd->info.samplerate) * snd->info.frames);
}

static VALUE rpg_sound_type(VALUE self) {
    RPGsound *snd = DATA_PTR(self);
    return INT2NUM(snd->info.format & SF_FORMAT_TYPEMASK);
}

static VALUE rpg_sound_subtype(VALUE self) {
    RPGsound *snd = DATA_PTR(self);
    return INT2NUM(snd->info.format & SF_FORMAT_SUBMASK);
}

static VALUE rpg_sound_endian(VALUE self) {
    RPGsound *snd = DATA_PTR(self);
    return INT2NUM(snd->info.format & SF_FORMAT_ENDMASK);
}

static VALUE rpg_sound_seek(VALUE self) {
    // TODO: Seek
    return self;
}

void rpg_sound_init(VALUE parent) {
    rb_cSound = rb_define_class_under(parent, "Sound", rb_cObject);
    rb_define_alloc_func(rb_cSound, rpg_sound_alloc);
    rb_define_method(rb_cSound, "initialize", rpg_sound_initialize, 1);
    rb_define_method(rb_cSound, "dispose", rpg_sound_dispose, 0);

    rb_define_method(rb_cSound, "duration", rpg_sound_duration, 0);
    rb_define_method(rb_cSound, "channels", rpg_sound_channels, 0);
    rb_define_method(rb_cSound, "sample_rate", rpg_sound_sample_rate, 0);
    rb_define_method(rb_cSound, "format", rpg_sound_format, 0);
    rb_define_method(rb_cSound, "seekable?", rpg_sound_seekable, 0);
    rb_define_method(rb_cSound, "sections", rpg_sound_sections, 0);
    rb_define_method(rb_cSound, "samples", rpg_sound_frames, 0);
    rb_define_method(rb_cSound, "inspect", rpg_sound_inspect, 0);

    rb_define_method(rb_cSound, "type", rpg_sound_type, 0);
    rb_define_method(rb_cSound, "subtype", rpg_sound_subtype, 0);
    rb_define_method(rb_cSound, "endian", rpg_sound_endian, 0);
    
    VALUE mod = rb_define_module_under(rb_cSound, "Type");
	rb_define_const(mod, "WAV", INT2NUM(SF_FORMAT_WAV));
	rb_define_const(mod, "AIFF", INT2NUM(SF_FORMAT_AIFF));
	rb_define_const(mod, "AU", INT2NUM(SF_FORMAT_AU));
	rb_define_const(mod, "RAW", INT2NUM(SF_FORMAT_RAW));
	rb_define_const(mod, "PAF", INT2NUM(SF_FORMAT_PAF));
	rb_define_const(mod, "SVX", INT2NUM(SF_FORMAT_SVX));
	rb_define_const(mod, "NIST", INT2NUM(SF_FORMAT_NIST));
	rb_define_const(mod, "VOC", INT2NUM(SF_FORMAT_VOC));
	rb_define_const(mod, "IRCAM", INT2NUM(SF_FORMAT_IRCAM));
	rb_define_const(mod, "W64", INT2NUM(SF_FORMAT_W64));
	rb_define_const(mod, "MAT4", INT2NUM(SF_FORMAT_MAT4));
	rb_define_const(mod, "MAT5", INT2NUM(SF_FORMAT_MAT5));
	rb_define_const(mod, "PVF", INT2NUM(SF_FORMAT_PVF));
	rb_define_const(mod, "XI", INT2NUM(SF_FORMAT_XI));
	rb_define_const(mod, "HTK", INT2NUM(SF_FORMAT_HTK));
	rb_define_const(mod, "SDS", INT2NUM(SF_FORMAT_SDS));
	rb_define_const(mod, "AVR", INT2NUM(SF_FORMAT_AVR));
	rb_define_const(mod, "WAVEX", INT2NUM(SF_FORMAT_WAVEX));
	rb_define_const(mod, "SD2", INT2NUM(SF_FORMAT_SD2));
	rb_define_const(mod, "FLAC", INT2NUM(SF_FORMAT_FLAC));
	rb_define_const(mod, "CAF", INT2NUM(SF_FORMAT_CAF));
	rb_define_const(mod, "WVE", INT2NUM(SF_FORMAT_WVE));
	rb_define_const(mod, "OGG", INT2NUM(SF_FORMAT_OGG));
	rb_define_const(mod, "MPC2K", INT2NUM(SF_FORMAT_MPC2K));
	rb_define_const(mod, "RF64", INT2NUM(SF_FORMAT_RF64));

    mod = rb_define_module_under(rb_cSound, "SubType");
	rb_define_const(mod, "PCM_S8", INT2NUM(SF_FORMAT_PCM_S8));
	rb_define_const(mod, "PCM_16", INT2NUM(SF_FORMAT_PCM_16));
	rb_define_const(mod, "PCM_24", INT2NUM(SF_FORMAT_PCM_24));
	rb_define_const(mod, "PCM_32", INT2NUM(SF_FORMAT_PCM_32));
	rb_define_const(mod, "PCM_U8", INT2NUM(SF_FORMAT_PCM_U8));
	rb_define_const(mod, "FLOAT", INT2NUM(SF_FORMAT_FLOAT));
	rb_define_const(mod, "DOUBLE", INT2NUM(SF_FORMAT_DOUBLE));
	rb_define_const(mod, "ULAW", INT2NUM(SF_FORMAT_ULAW));
	rb_define_const(mod, "ALAW", INT2NUM(SF_FORMAT_ALAW));
	rb_define_const(mod, "IMA_ADPCM", INT2NUM(SF_FORMAT_IMA_ADPCM));
	rb_define_const(mod, "MS_ADPCM", INT2NUM(SF_FORMAT_MS_ADPCM));
	rb_define_const(mod, "GSM610", INT2NUM(SF_FORMAT_GSM610));
	rb_define_const(mod, "VOX_ADPCM", INT2NUM(SF_FORMAT_VOX_ADPCM));
	rb_define_const(mod, "G721_32", INT2NUM(SF_FORMAT_G721_32));
	rb_define_const(mod, "G723_24", INT2NUM(SF_FORMAT_G723_24));
	rb_define_const(mod, "G723_40", INT2NUM(SF_FORMAT_G723_40));
	rb_define_const(mod, "DWVW_12", INT2NUM(SF_FORMAT_DWVW_12));
	rb_define_const(mod, "DWVW_16", INT2NUM(SF_FORMAT_DWVW_16));
	rb_define_const(mod, "DWVW_24", INT2NUM(SF_FORMAT_DWVW_24));
	rb_define_const(mod, "DWVW_N", INT2NUM(SF_FORMAT_DWVW_N));
	rb_define_const(mod, "DPCM_8", INT2NUM(SF_FORMAT_DPCM_8));
	rb_define_const(mod, "DPCM_16", INT2NUM(SF_FORMAT_DPCM_16));
	rb_define_const(mod, "VORBIS", INT2NUM(SF_FORMAT_VORBIS));
	rb_define_const(mod, "ALAC_16", INT2NUM(SF_FORMAT_ALAC_16));
	rb_define_const(mod, "ALAC_20", INT2NUM(SF_FORMAT_ALAC_20));
	rb_define_const(mod, "ALAC_24", INT2NUM(SF_FORMAT_ALAC_24));
	rb_define_const(mod, "ALAC_32", INT2NUM(SF_FORMAT_ALAC_32));

    mod = rb_define_module_under(rb_cSound, "Endian");
    rb_define_const(mod, "FILE", INT2NUM(SF_ENDIAN_FILE));
    rb_define_const(mod, "LITTLE", INT2NUM(SF_ENDIAN_LITTLE));
    rb_define_const(mod, "BIG", INT2NUM(SF_ENDIAN_BIG));
    rb_define_const(mod, "CPU", INT2NUM(SF_ENDIAN_CPU));
}
