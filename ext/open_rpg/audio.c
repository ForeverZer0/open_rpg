#include "./internal.h"
#include "./audio.h"
#include <AL/alc.h>

ALCcontext *context;
ALCdevice *device;

VALUE rb_mAudio;
VALUE rb_cSound;
VALUE rb_cChannel;

#pragma region Sound

ALLOC_FUNC(rpg_sound_alloc, RPGsound)
ATTR_READER(rpg_sound_channels, RPGsound, info.channels, INT2NUM)
ATTR_READER(rpg_sound_sample_rate, RPGsound, info.samplerate, INT2NUM)
ATTR_READER(rpg_sound_seekable, RPGsound, info.seekable, RB_BOOL)
ATTR_READER(rpg_sound_format, RPGsound, info.format, INT2NUM)
ATTR_READER(rpg_sound_sections, RPGsound, info.sections, INT2NUM)
ATTR_READER(rpg_sound_frames, RPGsound, info.frames, LL2NUM)

static void rpg_audio_al_format(RPGsound *snd, ALenum *fmt, size_t *sample_size) {
    // FIXME: This is messy. Drop all but Vorbis/WAV support? 
    int type = snd->info.format & SF_FORMAT_SUBMASK;
    if (snd->info.channels == 1) {
        switch (type) {
            case SF_FORMAT_PCM_U8:
            case SF_FORMAT_PCM_S8:
                *sample_size = sizeof(ALbyte);
                *fmt = AL_FORMAT_MONO8;
                break;
            case SF_FORMAT_PCM_16:
                *sample_size = sizeof(ALshort);
                *fmt = AL_FORMAT_MONO16;
                break;
            case SF_FORMAT_DOUBLE:
                *sample_size = sizeof(ALdouble);
                *fmt = AL_FORMAT_MONO_DOUBLE_EXT;
                break;
            case SF_FORMAT_VORBIS:
                *sample_size = sizeof(ALfloat);
                *fmt = AL_FORMAT_VORBIS_EXT;
            case SF_FORMAT_FLOAT:
            default:
                *sample_size = sizeof(ALfloat);
                *fmt = AL_FORMAT_MONO_FLOAT32;
                break;
        }
    } else if (snd->info.channels == 2) {
        switch (type) {

            case SF_FORMAT_PCM_U8:
            case SF_FORMAT_PCM_S8:
                *sample_size = sizeof(ALbyte);
                *fmt = AL_FORMAT_STEREO8;
                break;
            case SF_FORMAT_PCM_16:
                *sample_size = sizeof(ALshort);
                *fmt = AL_FORMAT_STEREO16;
                break;
            case SF_FORMAT_DOUBLE:
                *sample_size = sizeof(ALdouble);
                *fmt = AL_FORMAT_STEREO_DOUBLE_EXT;
                break;
            case SF_FORMAT_VORBIS:
                *sample_size = sizeof(ALfloat);
                *fmt = AL_FORMAT_VORBIS_EXT;
            case SF_FORMAT_FLOAT:
            default:
                *sample_size = sizeof(ALfloat);
                *fmt = AL_FORMAT_STEREO_FLOAT32;
                break;
        }
    } else {
        *sample_size = sizeof(ALfloat);
        *fmt = AL_FORMAT_STEREO_FLOAT32;
    }
}

static void rpg_audio_buffer_sound(RPGsound *sound) {
    if (sound == NULL || sound->buffer > 0) {
        // Return early if sound is NULL or already buffered
        return;
    }

    ALenum fmt;
    size_t sample_size;
    rpg_audio_al_format(sound, &fmt, &sample_size);

    pthread_mutex_lock(&sound->mutex);
    alGenBuffers(1, &sound->buffer);
    size_t n = sound->info.frames * sound->info.channels;
    void *data = RPG_MALLOC(n * sample_size);
    switch (fmt) {
        case AL_FORMAT_MONO8:
        case AL_FORMAT_STEREO8:
            sf_read_raw(sound->file, data, n);
            break;
        case AL_FORMAT_MONO16:
        case AL_FORMAT_STEREO16:
            sf_read_short(sound->file, data, n);
            break;
        case AL_FORMAT_STEREO_DOUBLE_EXT:
            sf_read_double(sound->file, data, n);
            break;
        case AL_FORMAT_MONO_FLOAT32:
        case AL_FORMAT_STEREO_FLOAT32:
        default:
            sf_read_float(sound->file, data, n);
            break;
    }
    alBufferData(sound->buffer, fmt, data, n * sample_size, sound->info.samplerate);
    CHECK_AL_ERROR("failed to buffer audio data");
    RPG_FREE(data);

    sf_close(sound->file);
    sound->file = NULL;
    pthread_mutex_unlock(&sound->mutex);
}

int rpg_sound_load(const char *fname, ALboolean buffer_now, RPGsound *sound) {
    RPG_THROW_UNLESS_FILE(fname);
    sound->file = sf_open(fname, SFM_READ, &sound->info);
    if (buffer_now) {
        rpg_audio_buffer_sound(sound);
    }
    return sf_error(sound->file);
}

static VALUE rpg_sound_dispose(VALUE self) {
    RPGsound *snd = DATA_PTR(self);
    if (snd->file) {
        sf_close(snd->file);
        snd->file = NULL;
    }
    alDeleteBuffers(1, &snd->buffer);
    pthread_mutex_destroy(&snd->mutex);
    return Qnil;
}

static VALUE rpg_sound_initialize(VALUE self, VALUE path, VALUE buffer_now) {
    RPGsound *snd = DATA_PTR(self);
    char *fname = StringValueCStr(path);
    if (rpg_sound_load(fname, RTEST(buffer_now), snd)) {
        const char *msg = sf_strerror(snd->file);
        rb_raise(rb_eRPGError, "failed to load audio file - %s", msg);
    }
    pthread_mutex_init(&snd->mutex, NULL);
    return Qnil;
}

static VALUE rpg_sound_inspect(VALUE self) {
    RPGsound *snd = DATA_PTR(self);
    return rb_sprintf("<Sound: channels:%d, samples:%d, rate:%d>", snd->info.channels, snd->info.frames, snd->info.samplerate);
}

static VALUE rpg_sound_duration(VALUE self) {
    RPGsound *snd = DATA_PTR(self);
    RPGtimespan *ts = ALLOC(RPGtimespan);
    ts->ms = (GLuint64)round((1000.0 / snd->info.samplerate) * snd->info.frames);
    return Data_Wrap_Struct(rb_cTimeSpan, NULL, RUBY_DEFAULT_FREE, ts);
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

#pragma endregion Sound

static VALUE rpg_audio_terminate(VALUE module) {
    alcDestroyContext(context);
    alcCloseDevice(device);
    return Qnil;
}

static VALUE rpg_audio_channel(VALUE module, VALUE index) {
    int i = NUM2INT(index);
    if (index < 0 || index >= MAX_CHANNELS) {
        return Qnil;
    }
    RPGchannel *c = rpgCHANNELS[i];
    return c ? Data_Wrap_Struct(rb_cChannel, NULL, NULL, c) : Qnil;
}

static VALUE rpg_audio_each_channel(VALUE module) {
    RETURN_ENUMERATOR(module, 0, NULL);
    for (int i = 0; i < MAX_CHANNELS; i++) {
        if (rpgCHANNELS[i]) {
            rb_yield(Data_Wrap_Struct(rb_cChannel, NULL, NULL, rpgCHANNELS[i]));
        }
    }
    return Qnil;
} 

static void rpg_audio_channel_free(void *channel) {
    if (channel) {
        RPGchannel *c = channel;
        alSourceStop(c->source);
        alDeleteSources(1, &c->source);
        if (c->slots) {
            for (int i = 0; i < c->num_slots; i++) {
                alDeleteAuxiliaryEffectSlots(1, &c->slots[i].slot);
            }
            RPG_FREE(c->slots);
        }
        if (c->dispose) {
            if (c->sound) {
                sf_close(c->sound->file);
                pthread_mutex_destroy(&c->sound->mutex);
                alDeleteBuffers(1, &c->sound->buffer);
                c->sound = NULL;
            }
            RPG_FREE(c->sound);
        }
        RPG_FREE(channel);
    }
    // TODO: Remove from channels
}

static void *rpg_audio_channel_play(void *channel) {
    RPGchannel *c = channel;

    if (!c->sound->buffer) {
        rpg_audio_buffer_sound(c->sound);
    }
    alSourcei(c->source, AL_BUFFER, c->sound->buffer);
    alSourcePlay(c->source);
    CHECK_AL_ERROR("failed to play sound");

    return NULL;
}

static void rpg_audio_source_sound(RPGchannel *channel, VALUE source) {
    if (RB_TYPE_P(source, T_DATA)) {
        channel->sound = DATA_PTR(source);
        channel->dispose = AL_FALSE;
    } else {
        char *fname = StringValueCStr(source);
        RPGsound *snd = ALLOC(RPGsound);
        memset(snd, 0, sizeof(RPGsound));
        pthread_mutex_init(&snd->mutex, NULL);
        if (rpg_sound_load(fname, AL_FALSE, snd)) {
            const char *msg = sf_strerror(snd->file);
            rb_raise(rb_eRPGError, "failed to load audio file - %s", msg);
        }
        channel->sound = snd;
        channel->dispose = AL_TRUE;
    }
    alGenSources(1, &channel->source);
}

static int rpg_audio_index(VALUE index) {
    int i = NUM2INT(index);
    if (i < 0 || i >= MAX_CHANNELS) {
        rb_raise(rb_eRPGError, "index out of valid range (given %d, expected 0...%s)", i, MAX_CHANNELS);
    }
    if (rpgCHANNELS[i]) {
        GLint state;
        alGetSourcei(rpgCHANNELS[i]->source, AL_SOURCE_STATE, &state);
        if (state != AL_STOPPED) {
            alSourceStop(rpgCHANNELS[i]->source);
        }
        rpg_audio_channel_free(rpgCHANNELS[i]);
        rpgCHANNELS[i] = NULL;
    }
    return i;
}

static void rpg_audio_effects(RPGchannel *channel, VALUE effects) {
    if (RB_TYPE_P(effects, T_ARRAY)) {
        long n = rb_array_len(effects);
        channel->num_slots = (ALint) n;
        channel->slots = RPG_MALLOC(n * sizeof(RPGslot));
        ALuint slots[n];
        alGenAuxiliaryEffectSlots(n, slots);
        for (long i = 0; i < n; i++) {
            VALUE item = rb_ary_entry(effects, i);
            RPGeffect *fx = DATA_PTR(item);
            alAuxiliaryEffectSloti(slots[i], AL_EFFECTSLOT_EFFECT, fx->effect);
            channel->slots[i].effect = fx->effect;
            channel->slots[i].slot = slots[i];
        }
    } else {
        channel->num_slots = 1;
        RPGslot *slot = ALLOC(RPGslot);
        alGenAuxiliaryEffectSlots(1, &slot->slot);
        RPGeffect *f = DATA_PTR(effects);
        slot->effect = f->effect;
        alAuxiliaryEffectSloti(slot->slot, AL_EFFECTSLOT_EFFECT, f->effect);
        channel->slots = slot;
    }
}

static VALUE rpg_audio_play(int argc, VALUE *argv, VALUE module) {
    VALUE index, sound, opts;
    rb_scan_args(argc, argv, "2:", &index, &sound, &opts);

    RPGchannel *channel = ALLOC(RPGchannel);
    channel->index = rpg_audio_index(index);
    memset(channel, 0, sizeof(RPGchannel));
    rpg_audio_source_sound(channel, sound);

    if (RTEST(opts)) {
        // Gain
        VALUE opt = rb_hash_aref(opts, STR2SYM("volume"));
        if (RTEST(opt)) {
            alSourcef(channel->source, AL_GAIN, clampf(NUM2FLT(opt), 0.0f, 1.0f));
        }
        // Pitch
        opt = rb_hash_aref(opts, STR2SYM("pitch"));
        if (RTEST(opt)) {
            alSourcef(channel->source, AL_PITCH, fmaxf(NUM2FLT(opt), 0.0f));
        }
        // Loop
        opt = rb_hash_aref(opts, STR2SYM("loop"));
        alSourcei(channel->source, AL_LOOPING, RTEST(opt));
        // Effects
        opt = rb_hash_aref(opts, STR2SYM("effects"));
        if (RTEST(opt)) {
            rpg_audio_effects(channel, opt);
        }
    }

    for (int i = 0; i < channel->num_slots; i++) {
        RPGslot slot = channel->slots[i];
        alSource3i(channel->source, AL_AUXILIARY_SEND_FILTER, slot.slot, i, AL_FILTER_NULL);
    }

    pthread_create(&channel->thread, NULL, rpg_audio_channel_play, channel);
    rpgCHANNELS[channel->index] = channel;
    return Data_Wrap_Struct(rb_cChannel, NULL, NULL, channel);
}

#pragma region Channel

// TODO: Channel dispose?

ATTR_READER(rpg_channel_index, RPGchannel, index, INT2NUM)

static VALUE rpg_channel_volume(VALUE self) {
    RPGchannel *s = DATA_PTR(self);
    ALfloat gain;
    alGetSourcef(s->source, AL_GAIN, &gain);
    return DBL2NUM(gain);
}

static VALUE rpg_channel_set_volume(VALUE self, VALUE value) {
    RPGchannel *s = DATA_PTR(self);
    alSourcef(s->source, AL_GAIN, clampf(NUM2FLT(value), 0.0f, 1.0f));
    return value;
}

static VALUE rpg_channel_pitch(VALUE self) {
    RPGchannel *s = DATA_PTR(self);
    ALfloat pitch;
    alGetSourcef(s->source, AL_PITCH, &pitch);
    return DBL2NUM(pitch);
}

static VALUE rpg_channel_set_pitch(VALUE self, VALUE value) {
    RPGchannel *s = DATA_PTR(self);
    ALfloat pitch = fmaxf(NUM2FLT(value), 0.0f);
    alSourcef(s->source, AL_PITCH, pitch);
    return value;
}

static VALUE rpg_channel_paused_p(VALUE self) {
    RPGchannel *s = DATA_PTR(self);
    ALint value;
    alGetSourcei(s->source, AL_SOURCE_STATE, &value);
    return RB_BOOL(value == AL_PAUSED);
}

static VALUE rpg_channel_playing_p(VALUE self) {
    RPGchannel *s = DATA_PTR(self);
    ALint value;
    alGetSourcei(s->source, AL_SOURCE_STATE, &value);
    return RB_BOOL(value == AL_PLAYING);
}

static VALUE rpg_channel_looping_p(VALUE self) {
    RPGchannel *s = DATA_PTR(self);
    ALint value;
    alGetSourcei(s->source, AL_LOOPING, &value);
    return RB_BOOL(value);
}

static VALUE rpg_channel_play(VALUE self) {
    RPGchannel *s = DATA_PTR(self);
    ALint state;
    alGetSourcei(s->source, AL_SOURCE_STATE, &state);
    if (state != AL_PLAYING) {
        alSourcePlay(s->source);
    }
    return self;
}

static VALUE rpg_channel_stop(VALUE self) {
    RPGchannel *s = DATA_PTR(self);
    alSourceStop(s->source);
    return self;
}

static VALUE rpg_channel_pause(VALUE self) {
    RPGchannel *s = DATA_PTR(self);
    alSourcePause(s->source);
    return self;
}

static VALUE rpg_channel_sound(VALUE self) {
    RPGchannel *s = DATA_PTR(self);
    return s->sound ? Data_Wrap_Struct(rb_cSound, NULL, NULL, s->sound) : Qnil;
}

static VALUE rpg_channel_rewind(VALUE self) {
    RPGchannel *s = DATA_PTR(self);
    alSourceRewind(s->source);
    return self;
}

#pragma endregion Channel

static inline GLenum rpg_channel_unit(VALUE unit) {
    GLenum u = AL_SEC_OFFSET;
    if (RTEST(unit)) {
        if (unit == STR2SYM("samples")) {
            u = AL_SAMPLE_OFFSET;
        } else if (unit == STR2SYM("bytes")) {
            u = AL_BYTE_OFFSET;
        }
    }
    return u;
}

static VALUE rpg_channel_valid_p(VALUE self) {
    RPGchannel *c = DATA_PTR(self);
    if (!alIsSource(c->source))
        return Qfalse;
    if (c->sound && (c->sound->file || alIsBuffer(c->sound->buffer))) {
        return Qtrue;
    }
    return Qfalse;
}

static VALUE rpg_channel_position(int argc, VALUE *argv, VALUE self) {
    VALUE unit;
    rb_scan_args(argc, argv, "01", &unit);
    GLenum u = rpg_channel_unit(unit);
    RPGchannel *s = DATA_PTR(self);
    if (u == AL_SEC_OFFSET) {
        ALfloat f;
        alGetSourcef(s->source, u, &f);
        return DBL2NUM(f);
    } else {
        ALint i;
        alGetSourcei(s->source, u, &i);
        return INT2NUM(i);
    }
}

static VALUE rpg_channel_seek(int argc, VALUE *argv, VALUE self) {
    VALUE value, unit;
    rb_scan_args(argc, argv, "11", &value, &unit);
    GLenum u = rpg_channel_unit(unit);
    RPGchannel *s = DATA_PTR(self);
    if (u == AL_SEC_OFFSET) {
        alSourcef(s->source, u, NUM2FLT(value));
        CHECK_AL_ERROR("could not seek audio buffer")
    } else {
        alSourcei(s->source, u, NUM2INT(value));
        CHECK_AL_ERROR("could not seek audio buffer")
    }
    return value;
}

void rpg_audio_init(VALUE parent) {
    device = alcOpenDevice(NULL);
    if (device == NULL) {
        rb_raise(rb_eRPGError, "failed to open audio device");
    }

    context = alcCreateContext(device, NULL);
    if (context == NULL) {
        rb_raise(rb_eRPGError, "failed to create audio context");
    }

    if (!alcMakeContextCurrent(context)) {
        rb_raise(rb_eRPGError, "failed to make OpenAL context current");
    }

    // Audio
    rb_mAudio = rb_define_module_under(parent, "Audio");
    rb_define_singleton_method(rb_mAudio, "terminate", rpg_audio_terminate, 0);
    rb_define_singleton_method(rb_mAudio, "play", rpg_audio_play, -1);
    rb_define_singleton_method(rb_mAudio, "channel", rpg_audio_channel, 1);
    rb_define_singleton_method(rb_mAudio, "[]", rpg_audio_channel, 1);
    rb_define_singleton_method(rb_mAudio, "each_channel", rpg_audio_each_channel, 0);

    // Sound
    rb_cSound = rb_define_class_under(rb_mAudio, "Sound", rb_cObject);
    rb_define_alloc_func(rb_cSound, rpg_sound_alloc);
    rb_define_method(rb_cSound, "initialize", rpg_sound_initialize, 2);
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

    // Channel
    rb_cChannel = rb_define_class_under(rb_mAudio, "Channel", rb_cObject);
    rb_define_method(rb_cChannel, "volume", rpg_channel_volume, 0);
    rb_define_method(rb_cChannel, "pitch", rpg_channel_pitch, 0);
    rb_define_method(rb_cChannel, "volume=", rpg_channel_set_volume, 1);
    rb_define_method(rb_cChannel, "pitch=", rpg_channel_set_pitch, 1);
    rb_define_method(rb_cChannel, "playing?", rpg_channel_playing_p, 0);
    rb_define_method(rb_cChannel, "paused?", rpg_channel_paused_p, 0);
    rb_define_method(rb_cChannel, "looping?", rpg_channel_looping_p, 0);
    rb_define_method(rb_cChannel, "pause", rpg_channel_pause, 0);
    rb_define_method(rb_cChannel, "stop", rpg_channel_stop, 0);
    rb_define_method(rb_cChannel, "play", rpg_channel_play, 0);
    rb_define_method(rb_cChannel, "sound", rpg_channel_sound, 0);
    rb_define_method(rb_cChannel, "rewind", rpg_channel_rewind, 0);
    rb_define_method(rb_cChannel, "position", rpg_channel_position, -1);
    rb_define_method(rb_cChannel, "seek", rpg_channel_seek, -1);
    rb_define_method(rb_cChannel, "index", rpg_channel_index, 0);
    rb_define_method(rb_cChannel, "valid?", rpg_channel_valid_p, 0);


    AL_LOAD_PROC(alGenAuxiliaryEffectSlots, LPALGENAUXILIARYEFFECTSLOTS);
    AL_LOAD_PROC(alDeleteAuxiliaryEffectSlots, LPALDELETEAUXILIARYEFFECTSLOTS);
    AL_LOAD_PROC(alIsAuxiliaryEffectSlot, LPALISAUXILIARYEFFECTSLOT);
    AL_LOAD_PROC(alAuxiliaryEffectSloti, LPALAUXILIARYEFFECTSLOTI);
    AL_LOAD_PROC(alAuxiliaryEffectSlotiv, LPALAUXILIARYEFFECTSLOTIV);
    AL_LOAD_PROC(alAuxiliaryEffectSlotf, LPALAUXILIARYEFFECTSLOTF);
    AL_LOAD_PROC(alAuxiliaryEffectSlotfv, LPALAUXILIARYEFFECTSLOTFV);
    AL_LOAD_PROC(alGetAuxiliaryEffectSloti, LPALGETAUXILIARYEFFECTSLOTI);
    AL_LOAD_PROC(alGetAuxiliaryEffectSlotiv, LPALGETAUXILIARYEFFECTSLOTIV);
    AL_LOAD_PROC(alGetAuxiliaryEffectSlotf, LPALGETAUXILIARYEFFECTSLOTF);
    AL_LOAD_PROC(alGetAuxiliaryEffectSlotfv, LPALGETAUXILIARYEFFECTSLOTFV);

    // Sound::Type
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

    // Sound::SubType
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

    // Sound::Endian
    mod = rb_define_module_under(rb_cSound, "Endian");
    rb_define_const(mod, "FILE", INT2NUM(SF_ENDIAN_FILE));
    rb_define_const(mod, "LITTLE", INT2NUM(SF_ENDIAN_LITTLE));
    rb_define_const(mod, "BIG", INT2NUM(SF_ENDIAN_BIG));
    rb_define_const(mod, "CPU", INT2NUM(SF_ENDIAN_CPU));

    rpg_audiofx_init(rb_mAudio);
    rpgCHANNELS = RPG_MALLOC(sizeof(void*) * MAX_CHANNELS);
    memset(rpgCHANNELS, 0, sizeof(void*) * MAX_CHANNELS);
}