#include "./internal.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>

#define MAX_GAIN 5.0f

#define CHECK_AL_ERROR(msg)                                                                                                                \
    if (alGetError() != AL_NO_ERROR) {                                                                                                     \
        rb_raise(rb_eRPGError, msg);                                                                                                       \
    }

typedef struct _RPGchannel {
    RPGsound *sound;
    ALuint source;
    ALfloat gain;
    ALfloat pitch;
    ALboolean loop;
    ALboolean dispose;
    pthread_t thread;
#ifdef RPG_AUDIO_FX
    GLuint reverb;
    GLuint aux_slot;
#endif
} RPGchannel;

#ifdef RPG_AUDIO_FX
#include "AL/efx-presets.h"
#include <AL/efx.h>

/* Effect object functions */
static LPALGENEFFECTS alGenEffects;
static LPALDELETEEFFECTS alDeleteEffects;
static LPALISEFFECT alIsEffect;
static LPALEFFECTI alEffecti;
static LPALEFFECTIV alEffectiv;
static LPALEFFECTF alEffectf;
static LPALEFFECTFV alEffectfv;
static LPALGETEFFECTI alGetEffecti;
static LPALGETEFFECTIV alGetEffectiv;
static LPALGETEFFECTF alGetEffectf;
static LPALGETEFFECTFV alGetEffectfv;

/* Auxiliary Effect Slot object functions */
static LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots;
static LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots;
static LPALISAUXILIARYEFFECTSLOT alIsAuxiliaryEffectSlot;
static LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti;
static LPALAUXILIARYEFFECTSLOTIV alAuxiliaryEffectSlotiv;
static LPALAUXILIARYEFFECTSLOTF alAuxiliaryEffectSlotf;
static LPALAUXILIARYEFFECTSLOTFV alAuxiliaryEffectSlotfv;
static LPALGETAUXILIARYEFFECTSLOTI alGetAuxiliaryEffectSloti;
static LPALGETAUXILIARYEFFECTSLOTIV alGetAuxiliaryEffectSlotiv;
static LPALGETAUXILIARYEFFECTSLOTF alGetAuxiliaryEffectSlotf;
static LPALGETAUXILIARYEFFECTSLOTFV alGetAuxiliaryEffectSlotfv;

/* LoadEffect loads the given reverb properties into a new OpenAL effect
 * object, and returns the new effect ID. */
static ALuint rpg_audio_create_reverb(const EFXEAXREVERBPROPERTIES *reverb) {
    ALuint effect = 0;
    ALenum err;
    /* Create the effect object and check if we can do EAX reverb. */
    alGenEffects(1, &effect);
    if (alGetEnumValue("AL_EFFECT_EAXREVERB") != 0) {
        /* EAX Reverb is available. Set the EAX effect type then load the
         * reverb properties. */
        alEffecti(effect, AL_EFFECT_TYPE, AL_EFFECT_EAXREVERB);
        alEffectf(effect, AL_EAXREVERB_DENSITY, reverb->flDensity);
        alEffectf(effect, AL_EAXREVERB_DIFFUSION, reverb->flDiffusion);
        alEffectf(effect, AL_EAXREVERB_GAIN, reverb->flGain);
        alEffectf(effect, AL_EAXREVERB_GAINHF, reverb->flGainHF);
        alEffectf(effect, AL_EAXREVERB_GAINLF, reverb->flGainLF);
        alEffectf(effect, AL_EAXREVERB_DECAY_TIME, reverb->flDecayTime);
        alEffectf(effect, AL_EAXREVERB_DECAY_HFRATIO, reverb->flDecayHFRatio);
        alEffectf(effect, AL_EAXREVERB_DECAY_LFRATIO, reverb->flDecayLFRatio);
        alEffectf(effect, AL_EAXREVERB_REFLECTIONS_GAIN, reverb->flReflectionsGain);
        alEffectf(effect, AL_EAXREVERB_REFLECTIONS_DELAY, reverb->flReflectionsDelay);
        alEffectfv(effect, AL_EAXREVERB_REFLECTIONS_PAN, reverb->flReflectionsPan);
        alEffectf(effect, AL_EAXREVERB_LATE_REVERB_GAIN, reverb->flLateReverbGain);
        alEffectf(effect, AL_EAXREVERB_LATE_REVERB_DELAY, reverb->flLateReverbDelay);
        alEffectfv(effect, AL_EAXREVERB_LATE_REVERB_PAN, reverb->flLateReverbPan);
        alEffectf(effect, AL_EAXREVERB_ECHO_TIME, reverb->flEchoTime);
        alEffectf(effect, AL_EAXREVERB_ECHO_DEPTH, reverb->flEchoDepth);
        alEffectf(effect, AL_EAXREVERB_MODULATION_TIME, reverb->flModulationTime);
        alEffectf(effect, AL_EAXREVERB_MODULATION_DEPTH, reverb->flModulationDepth);
        alEffectf(effect, AL_EAXREVERB_AIR_ABSORPTION_GAINHF, reverb->flAirAbsorptionGainHF);
        alEffectf(effect, AL_EAXREVERB_HFREFERENCE, reverb->flHFReference);
        alEffectf(effect, AL_EAXREVERB_LFREFERENCE, reverb->flLFReference);
        alEffectf(effect, AL_EAXREVERB_ROOM_ROLLOFF_FACTOR, reverb->flRoomRolloffFactor);
        alEffecti(effect, AL_EAXREVERB_DECAY_HFLIMIT, reverb->iDecayHFLimit);
    } else {
        /* No EAX Reverb. Set the standard reverb effect type then load the
         * available reverb properties. */
        alEffecti(effect, AL_EFFECT_TYPE, AL_EFFECT_REVERB);
        alEffectf(effect, AL_REVERB_DENSITY, reverb->flDensity);
        alEffectf(effect, AL_REVERB_DIFFUSION, reverb->flDiffusion);
        alEffectf(effect, AL_REVERB_GAIN, reverb->flGain);
        alEffectf(effect, AL_REVERB_GAINHF, reverb->flGainHF);
        alEffectf(effect, AL_REVERB_DECAY_TIME, reverb->flDecayTime);
        alEffectf(effect, AL_REVERB_DECAY_HFRATIO, reverb->flDecayHFRatio);
        alEffectf(effect, AL_REVERB_REFLECTIONS_GAIN, reverb->flReflectionsGain);
        alEffectf(effect, AL_REVERB_REFLECTIONS_DELAY, reverb->flReflectionsDelay);
        alEffectf(effect, AL_REVERB_LATE_REVERB_GAIN, reverb->flLateReverbGain);
        alEffectf(effect, AL_REVERB_LATE_REVERB_DELAY, reverb->flLateReverbDelay);
        alEffectf(effect, AL_REVERB_AIR_ABSORPTION_GAINHF, reverb->flAirAbsorptionGainHF);
        alEffectf(effect, AL_REVERB_ROOM_ROLLOFF_FACTOR, reverb->flRoomRolloffFactor);
        alEffecti(effect, AL_REVERB_DECAY_HFLIMIT, reverb->iDecayHFLimit);
    }
    /* Check if an error occured, and clean up if so. */
    err = alGetError();
    if (err != AL_NO_ERROR) {
        fprintf(stderr, "OpenAL error: %s\n", alGetString(err));
        if (alIsEffect(effect))
            alDeleteEffects(1, &effect);
        return 0;
    }
    return effect;
}

#endif

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

int rpg_sound_load(const char *fname, RPGsound *sound) {
    RPG_THROW_UNLESS_FILE(fname);
    sound->file = sf_open(fname, SFM_READ, &sound->info);
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

static VALUE rpg_sound_initialize(VALUE self, VALUE path) {
    RPGsound *snd = DATA_PTR(self);
    char *fname = StringValueCStr(path);
    if (rpg_sound_load(fname, snd)) {
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

static void rpg_audio_al_format(RPGsound *snd, ALenum *fmt, size_t *sample_size) {

    int subtype = snd->info.format & SF_FORMAT_SUBMASK;
    if (snd->info.channels == 1) {
        switch (subtype) {
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
            case SF_FORMAT_FLOAT:
            case SF_FORMAT_VORBIS:
            default:
                *sample_size = sizeof(ALfloat);
                *fmt = AL_FORMAT_MONO_FLOAT32;
                break;
        }
    } else if (snd->info.channels == 2) {
        switch (subtype) {

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
            case SF_FORMAT_FLOAT:
            case SF_FORMAT_VORBIS:
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

static void *rpg_audio_channel(void *channel) {
    RPGchannel *c = channel;
    RPGsound *snd = c->sound;
    pthread_t thread = c->thread;

    if (!snd->buffer) {
        if (snd == NULL) {
            return NULL;
        }

        ALenum fmt;
        size_t sample_size;
        rpg_audio_al_format(snd, &fmt, &sample_size);

        pthread_mutex_lock(&snd->mutex);
        alGenBuffers(1, &snd->buffer);
        size_t n = snd->info.frames * snd->info.channels;
        void *data = RPG_MALLOC(n * sample_size);
        switch (fmt) {
            case AL_FORMAT_MONO8:
            case AL_FORMAT_STEREO8:
                sf_read_raw(snd->file, data, n);
                break;
            case AL_FORMAT_MONO16:
            case AL_FORMAT_STEREO16:
                sf_read_short(snd->file, data, n);
                break;
            case AL_FORMAT_STEREO_DOUBLE_EXT:
                sf_read_double(snd->file, data, n);
                break;
            case AL_FORMAT_MONO_FLOAT32:
            case AL_FORMAT_STEREO_FLOAT32:
            default:
                sf_read_float(snd->file, data, n);
                break;
        }
        alBufferData(snd->buffer, fmt, data, n * sample_size, snd->info.samplerate);
        CHECK_AL_ERROR("failed to buffer audio data");
        RPG_FREE(data);

        sf_close(snd->file);
        snd->file = NULL;

        pthread_mutex_unlock(&snd->mutex);
    }

    alSourcef(c->source, AL_MAX_GAIN, MAX_GAIN);
    alSourcef(c->source, AL_GAIN, c->gain);
    alSourcef(c->source, AL_PITCH, c->pitch);
    alSourcei(c->source, AL_LOOPING, c->loop);
    alSourcei(c->source, AL_BUFFER, snd->buffer);
#ifdef RPG_AUDIO_FX
    alSource3i(c->source, AL_AUXILIARY_SEND_FILTER, c->aux_slot, 0, AL_FILTER_NULL);
    CHECK_AL_ERROR("failed to set auxillary slot for audio channel");
#endif
    alSourcePlay(c->source);
    CHECK_AL_ERROR("failed to play sound");
    ALboolean done = AL_FALSE;
    GLint state;
    while (!done) {
        sleep(1);
        alGetSourcei(c->source, AL_SOURCE_STATE, &state);
        if (state != AL_PLAYING && state != AL_PAUSED) {
            alDeleteSources(1, &c->source);
#ifdef RPG_AUDIO_FX
            alDeleteAuxiliaryEffectSlots(1, &c->aux_slot);
            alDeleteEffects(1, &c->reverb);
#endif
            if (c->dispose) {
                alDeleteBuffers(1, &snd->buffer);
                RPG_FREE(snd);
                RPG_FREE(channel);
            }
            pthread_join(c->thread, NULL);
            break;
        }
    }
    return NULL;
}

static VALUE rpg_audio_play_file(int argc, VALUE *argv, VALUE module) {

    VALUE path, volume, pitch, opts;
    rb_scan_args(argc, argv, "12:", &path, &volume, &pitch, &opts);

    char *fname = StringValueCStr(path);
    RPGsound *snd = ALLOC(RPGsound);
    memset(snd, 0, sizeof(RPGsound));
    pthread_mutex_init(&snd->mutex, NULL);

    if (rpg_sound_load(fname, snd)) {
        const char *msg = sf_strerror(snd->file);
        rb_raise(rb_eRPGError, "failed to load audio file - %s", msg);
    }

    RPGchannel *channel = ALLOC(RPGchannel);
    alGenSources(1, &channel->source);
    channel->sound = snd;
    channel->dispose = AL_TRUE;

    channel->gain = RTEST(volume) ? fmaxf(NUM2FLT(volume), 0.0f) : 1.0f;
    channel->pitch = RTEST(pitch) ? fmaxf(NUM2FLT(pitch), 0.0f) : 1.0f;

    if (RTEST(opts)) {
        channel->loop = RTEST(rb_hash_aref(opts, STR2SYM("loop")));
    } else {
        channel->loop = AL_FALSE;
    }

    pthread_create(&channel->thread, NULL, rpg_audio_channel, channel);
    return Qnil;
}

static VALUE rpg_audio_play_sound(int argc, VALUE *argv, VALUE module) {
    VALUE sound, volume, pitch, opts;
    rb_scan_args(argc, argv, "12:", &sound, &volume, &pitch, &opts);

    RPGchannel *channel = ALLOC(RPGchannel);
    alGenSources(1, &channel->source);
    channel->sound = DATA_PTR(sound);
    channel->dispose = AL_FALSE;

    channel->gain = RTEST(volume) ? fmaxf(NUM2FLT(volume), 0.0f) : 1.0f;
    channel->pitch = RTEST(pitch) ? fmaxf(NUM2FLT(pitch), 0.0f) : 1.0f;

    EFXEAXREVERBPROPERTIES reverb = EFX_REVERB_PRESET_GENERIC;
    if (RTEST(opts)) {
        VALUE opt = rb_hash_aref(opts, STR2SYM("loop"));
        channel->loop = RTEST(opt);

        opt = rb_hash_aref(opts, STR2SYM("reverb"));
        if (RTEST(opt)) {
            VALUE r = rb_hash_aref(opts, STR2SYM("reverb"));
            reverb = *((EFXEAXREVERBPROPERTIES *)DATA_PTR(r));
        }
    } else {
        channel->loop = AL_FALSE;
    }

#ifdef RPG_AUDIO_FX
    channel->reverb = rpg_audio_create_reverb(&reverb);
    alGenAuxiliaryEffectSlots(1, &channel->aux_slot);
    alAuxiliaryEffectSloti(channel->aux_slot, AL_EFFECTSLOT_EFFECT, channel->reverb);
    CHECK_AL_ERROR("failed to create reverb effect");
#endif

    pthread_create(&channel->thread, NULL, rpg_audio_channel, channel);
    return Data_Wrap_Struct(rb_cChannel, NULL, RUBY_DEFAULT_FREE, channel);
}

#pragma region Channel

static VALUE rpg_channel_volume(VALUE self) {
    RPGchannel *s = DATA_PTR(self);
    ALfloat gain;
    alGetSourcef(s->source, AL_GAIN, &gain);
    return DBL2NUM(gain);
}

static VALUE rpg_channel_set_volume(VALUE self, VALUE value) {
    RPGchannel *s = DATA_PTR(self);
    ALfloat gain = fmaxf(NUM2FLT(value), 0.0f);
    alSourcef(s->source, AL_GAIN, gain);
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
    alSourcePlay(s->source);
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
    CHECK_AL_ERROR("failed to open audio device")

    context = alcCreateContext(device, NULL);
    CHECK_AL_ERROR("failed to create audio context");

    if (!alcMakeContextCurrent(context)) {
        rb_raise(rb_eRPGError, "failed to make OpenAL context current");
    }

    // Audio
    rb_mAudio = rb_define_module_under(parent, "Audio");
    rb_define_singleton_method(rb_mAudio, "terminate", rpg_audio_terminate, 0);
    rb_define_singleton_method(rb_mAudio, "play_sound", rpg_audio_play_sound, -1);
    rb_define_singleton_method(rb_mAudio, "play_file", rpg_audio_play_file, -1);

    // Sound
    rb_cSound = rb_define_class_under(rb_mAudio, "Sound", rb_cObject);
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

#ifdef RPG_AUDIO_FX

#define LOAD_PROC(x, y) ((x) = (y)alGetProcAddress(#x))
    LOAD_PROC(alGenEffects, LPALGENEFFECTS);
    LOAD_PROC(alDeleteEffects, LPALDELETEEFFECTS);
    LOAD_PROC(alIsEffect, LPALISEFFECT);
    LOAD_PROC(alEffecti, LPALEFFECTI);
    LOAD_PROC(alEffectiv, LPALEFFECTIV);
    LOAD_PROC(alEffectf, LPALEFFECTF);
    LOAD_PROC(alEffectfv, LPALEFFECTFV);
    LOAD_PROC(alGetEffecti, LPALGETEFFECTI);
    LOAD_PROC(alGetEffectiv, LPALGETEFFECTIV);
    LOAD_PROC(alGetEffectf, LPALGETEFFECTF);
    LOAD_PROC(alGetEffectfv, LPALGETEFFECTFV);
    LOAD_PROC(alGenAuxiliaryEffectSlots, LPALGENAUXILIARYEFFECTSLOTS);
    LOAD_PROC(alDeleteAuxiliaryEffectSlots, LPALDELETEAUXILIARYEFFECTSLOTS);
    LOAD_PROC(alIsAuxiliaryEffectSlot, LPALISAUXILIARYEFFECTSLOT);
    LOAD_PROC(alAuxiliaryEffectSloti, LPALAUXILIARYEFFECTSLOTI);
    LOAD_PROC(alAuxiliaryEffectSlotiv, LPALAUXILIARYEFFECTSLOTIV);
    LOAD_PROC(alAuxiliaryEffectSlotf, LPALAUXILIARYEFFECTSLOTF);
    LOAD_PROC(alAuxiliaryEffectSlotfv, LPALAUXILIARYEFFECTSLOTFV);
    LOAD_PROC(alGetAuxiliaryEffectSloti, LPALGETAUXILIARYEFFECTSLOTI);
    LOAD_PROC(alGetAuxiliaryEffectSlotiv, LPALGETAUXILIARYEFFECTSLOTIV);
    LOAD_PROC(alGetAuxiliaryEffectSlotf, LPALGETAUXILIARYEFFECTSLOTF);
    LOAD_PROC(alGetAuxiliaryEffectSlotfv, LPALGETAUXILIARYEFFECTSLOTFV);
#undef LOAD_PROC
#endif

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
}