#include "./audio.h"
#include "./internal.h"
#include <AL/efx-presets.h>

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

#define RPG_AUDIOFX_DEF_INIT(klass, name)                                                                                                  \
    VALUE klass = rb_define_class_under(effects, name, effect);                                                                            \
    rb_define_method(klass, "initialize", rpg_##klass##_initialize, 0)

#define RPG_AUDIOFX_DEF_ATTR(type, name)                                                                                                   \
    rb_define_method(type, #name, rpg_##type##_##name##_get, 0);                                                                           \
    rb_define_method(type, #name "=", rpg_##type##_##name##_set, 1)

#define RPG_AUDIOFX_INIT_FUNC(klass, type)                                                                                                 \
    static VALUE rpg_##klass##_initialize(VALUE self) {                                                                                    \
        VALUE t = INT2NUM(type);                                                                                                           \
        rb_call_super(1, &t);                                                                                                              \
        return Qnil;                                                                                                                       \
    }

#define RPG_AUDIOFX_ATTR_F(func, param, minv, maxv)                                                                                        \
    static VALUE func##_get(VALUE self) {                                                                                                  \
        RPGeffect *e = DATA_PTR(self);                                                                                                     \
        ALfloat v;                                                                                                                         \
        alGetEffectf(e->effect, param, &v);                                                                                                \
        return DBL2NUM(v);                                                                                                                 \
    }                                                                                                                                      \
    static VALUE func##_set(VALUE self, VALUE value) {                                                                                     \
        RPGeffect *e = DATA_PTR(self);                                                                                                     \
        alEffectf(e->effect, param, clampf(NUM2FLT(value), minv, maxv));                                                                   \
        return value;                                                                                                                      \
    }

#define RPG_AUDIOFX_ATTR_VEC(func, param)                                                                                                  \
    static VALUE func##_get(VALUE self) {                                                                                                  \
        RPGeffect *e = DATA_PTR(self);                                                                                                     \
        RPGvec3 *v = ALLOC(RPGvec3);                                                                                                       \
        alGetEffectfv(e->effect, param, (ALfloat*) v);                                                                                     \
        return Data_Wrap_Struct(rb_cVec3, NULL, RUBY_DEFAULT_FREE, v);                                                                     \
    }                                                                                                                                      \
    static VALUE func##_set(VALUE self, VALUE value) {                                                                                     \
        RPGeffect *e = DATA_PTR(self);                                                                                                     \
        ALfloat *v = DATA_PTR(value);                                                                                                      \
        alEffectfv(e->effect, param, v);                                                                                                   \
        return value;                                                                                                                      \
    }

#define RPG_AUDIOFX_ATTR_I(func, param, minv, maxv)                                                                                        \
    static VALUE func##_get(VALUE self) {                                                                                                  \
        RPGeffect *e = DATA_PTR(self);                                                                                                     \
        ALint v;                                                                                                                           \
        alGetEffecti(e->effect, param, &v);                                                                                                \
        return INT2NUM(v);                                                                                                                 \
    }                                                                                                                                      \
    static VALUE func##_set(VALUE self, VALUE value) {                                                                                     \
        RPGeffect *e = DATA_PTR(self);                                                                                                     \
        alEffecti(e->effect, param, clampi(NUM2INT(value), minv, maxv));                                                                   \
        return value;                                                                                                                      \
    }

#define RPG_AUDIOFX_ATTR_BOOL(func, param)                                                                                                 \
    static VALUE func##_get(VALUE self) {                                                                                                  \
        RPGeffect *e = DATA_PTR(self);                                                                                                     \
        ALint v;                                                                                                                           \
        alGetEffecti(e->effect, param, &v);                                                                                                \
        return RB_BOOL(v);                                                                                                                 \
    }                                                                                                                                      \
    static VALUE func##_set(VALUE self, VALUE value) {                                                                                     \
        RPGeffect *e = DATA_PTR(self);                                                                                                     \
        alEffecti(e->effect, param, RTEST(value));                                                                                         \
        return value;                                                                                                                      \
    }

static void rpg_audiofx_free(void *data) {
    printf("freeing");
    if (data) {
        RPGeffect *e = data;
        alDeleteEffects(1, &e->effect);
        RPG_FREE(data);
    }
}

static VALUE rpg_audiofx_alloc(VALUE klass) {
    RPGeffect *e = ALLOC(RPGeffect);
    memset(e, 0, sizeof(RPGeffect));
    return Data_Wrap_Struct(klass, NULL, rpg_audiofx_free, e);
}

static VALUE rpg_audiofx_initialize(VALUE self, VALUE type) {
    RPGeffect *e = DATA_PTR(self);
    e->type = NUM2INT(type);
    alGenEffects(1, &e->effect);
    alEffecti(e->effect, AL_EFFECT_TYPE, e->type);
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        alDeleteEffects(1, &e->effect);
        rb_raise(rb_eRPGError, "failed to create audio effect - %08X", error);
    }
}

static VALUE rpg_audiofx_valid_p(VALUE self) {
    RPGeffect *e = DATA_PTR(self);
    return RB_BOOL(alIsEffect(e->effect));
}

static VALUE rpg_audiofx_setf(VALUE self, VALUE param, VALUE value) {
    RPGeffect *e = DATA_PTR(self);
    ALenum p = NUM2INT(param);
    if (RB_TYPE_P(value, T_ARRAY)) {
        long n = rb_array_len(value);
        ALfloat args[n];
        for (long i = 0; i < n; i++) {
            args[i] = NUM2FLT(rb_ary_entry(value, i));
        }
        alEffectfv(e->effect, p, args);
    } else {
        alEffectf(e->effect, p, NUM2FLT(value));
    }
    CHECK_AL_ERROR("error setting parameter, ensure param is correct value is within valid range")
    return value;
}

static VALUE rpg_audiofx_seti(VALUE self, VALUE param, VALUE value) {
    RPGeffect *e = DATA_PTR(self);
    ALenum p = NUM2INT(param);
    if (RB_TYPE_P(value, T_ARRAY)) {
        long n = rb_array_len(value);
        ALint args[n];
        for (long i = 0; i < n; i++) {
            args[i] = NUM2INT(rb_ary_entry(value, i));
        }
        alEffectiv(e->effect, p, args);
    } else {
        alEffecti(e->effect, p, NUM2INT(value));
    }
    CHECK_AL_ERROR("error setting parameter, ensure param is correct value is within valid range")
    return value;
}

static VALUE rpg_audiofx_getf(VALUE self, VALUE param, VALUE count) {
    RPGeffect *e = DATA_PTR(self);
    ALenum p = NUM2INT(param);
    int n = NUM2INT(count);
    if (n > 1) {
        ALfloat buffer[n];
        alGetEffectfv(e->effect, p, buffer);
        VALUE ary = rb_ary_new_capa(n);
        for (int i = 0; i < n; i++) {
            rb_ary_store(ary, i, DBL2NUM(buffer[i]));
        }
        return ary;
    } else {
        ALfloat v;
        alGetEffectf(e->effect, p, &v);
        return DBL2NUM(v);
    }
}

static VALUE rpg_audiofx_geti(VALUE self, VALUE param, VALUE count) {
    RPGeffect *e = DATA_PTR(self);
    ALenum p = NUM2INT(param);
    int n = NUM2INT(count);
    if (n > 1) {
        ALint buffer[n];
        alGetEffectiv(e->effect, p, buffer);
        VALUE ary = rb_ary_new_capa(n);
        for (int i = 0; i < n; i++) {
            rb_ary_store(ary, i, INT2NUM(buffer[i]));
        }
        return ary;
    } else {
        ALint v;
        alGetEffecti(e->effect, p, &v);
        return INT2NUM(v);
    }
}

// Reverb

static VALUE rpg_reverb_initialize(int argc, VALUE *argv, VALUE self) {
    VALUE arg;
    rb_scan_args(argc, argv, "01", &arg);

    VALUE type = INT2NUM((alGetEnumValue("AL_EFFECT_EAXREVERB") != 0) ? AL_EFFECT_EAXREVERB : AL_EFFECT_REVERB);
    rb_call_super(1, &type);

    if (RTEST(arg)) {
        RPGeffect *e = DATA_PTR(self);
        EFXEAXREVERBPROPERTIES *preset = DATA_PTR(arg);
        if (e->type == AL_EFFECT_EAXREVERB) {
            // EAX Reverb is only in OpenAL-Soft implementations
            alEffectf(e->effect, AL_EAXREVERB_DENSITY, preset->flDensity);
            alEffectf(e->effect, AL_EAXREVERB_DIFFUSION, preset->flDiffusion);
            alEffectf(e->effect, AL_EAXREVERB_GAIN, preset->flGain);
            alEffectf(e->effect, AL_EAXREVERB_GAINHF, preset->flGainHF);
            alEffectf(e->effect, AL_EAXREVERB_GAINLF, preset->flGainLF);
            alEffectf(e->effect, AL_EAXREVERB_DECAY_TIME, preset->flDecayTime);
            alEffectf(e->effect, AL_EAXREVERB_DECAY_HFRATIO, preset->flDecayHFRatio);
            alEffectf(e->effect, AL_EAXREVERB_DECAY_LFRATIO, preset->flDecayLFRatio);
            alEffectf(e->effect, AL_EAXREVERB_REFLECTIONS_GAIN, preset->flReflectionsGain);
            alEffectf(e->effect, AL_EAXREVERB_REFLECTIONS_DELAY, preset->flReflectionsDelay);
            alEffectfv(e->effect, AL_EAXREVERB_REFLECTIONS_PAN, preset->flReflectionsPan);
            alEffectf(e->effect, AL_EAXREVERB_LATE_REVERB_GAIN, preset->flLateReverbGain);
            alEffectf(e->effect, AL_EAXREVERB_LATE_REVERB_DELAY, preset->flLateReverbDelay);
            alEffectfv(e->effect, AL_EAXREVERB_LATE_REVERB_PAN, preset->flLateReverbPan);
            alEffectf(e->effect, AL_EAXREVERB_ECHO_TIME, preset->flEchoTime);
            alEffectf(e->effect, AL_EAXREVERB_ECHO_DEPTH, preset->flEchoDepth);
            alEffectf(e->effect, AL_EAXREVERB_MODULATION_TIME, preset->flModulationTime);
            alEffectf(e->effect, AL_EAXREVERB_MODULATION_DEPTH, preset->flModulationDepth);
            alEffectf(e->effect, AL_EAXREVERB_AIR_ABSORPTION_GAINHF, preset->flAirAbsorptionGainHF);
            alEffectf(e->effect, AL_EAXREVERB_HFREFERENCE, preset->flHFReference);
            alEffectf(e->effect, AL_EAXREVERB_LFREFERENCE, preset->flLFReference);
            alEffectf(e->effect, AL_EAXREVERB_ROOM_ROLLOFF_FACTOR, preset->flRoomRolloffFactor);
            alEffecti(e->effect, AL_EAXREVERB_DECAY_HFLIMIT, preset->iDecayHFLimit);
        } else {
            // EAX Reverb not present, use standard reverb
            alEffectf(e->effect, AL_REVERB_DENSITY, preset->flDensity);
            alEffectf(e->effect, AL_REVERB_DIFFUSION, preset->flDiffusion);
            alEffectf(e->effect, AL_REVERB_GAIN, preset->flGain);
            alEffectf(e->effect, AL_REVERB_GAINHF, preset->flGainHF);
            alEffectf(e->effect, AL_REVERB_DECAY_TIME, preset->flDecayTime);
            alEffectf(e->effect, AL_REVERB_DECAY_HFRATIO, preset->flDecayHFRatio);
            alEffectf(e->effect, AL_REVERB_REFLECTIONS_GAIN, preset->flReflectionsGain);
            alEffectf(e->effect, AL_REVERB_REFLECTIONS_DELAY, preset->flReflectionsDelay);
            alEffectf(e->effect, AL_REVERB_LATE_REVERB_GAIN, preset->flLateReverbGain);
            alEffectf(e->effect, AL_REVERB_LATE_REVERB_DELAY, preset->flLateReverbDelay);
            alEffectf(e->effect, AL_REVERB_AIR_ABSORPTION_GAINHF, preset->flAirAbsorptionGainHF);
            alEffectf(e->effect, AL_REVERB_ROOM_ROLLOFF_FACTOR, preset->flRoomRolloffFactor);
            alEffecti(e->effect, AL_REVERB_DECAY_HFLIMIT, preset->iDecayHFLimit);
        }
    } 
}

// RPG_AUDIOFX_INIT_FUNC(reverb, AL_EFFECT_EAXREVERB)
RPG_AUDIOFX_ATTR_F(rpg_reverb_density, AL_EAXREVERB_DENSITY, AL_EAXREVERB_MIN_DENSITY, AL_EAXREVERB_MAX_DENSITY)
RPG_AUDIOFX_ATTR_F(rpg_reverb_diffusion, AL_EAXREVERB_DIFFUSION, AL_EAXREVERB_MIN_DIFFUSION, AL_EAXREVERB_MAX_DIFFUSION)
RPG_AUDIOFX_ATTR_F(rpg_reverb_gain, AL_EAXREVERB_GAIN, AL_EAXREVERB_MIN_GAIN, AL_EAXREVERB_MAX_GAIN)
RPG_AUDIOFX_ATTR_F(rpg_reverb_gain_hf, AL_EAXREVERB_GAINHF, AL_EAXREVERB_MIN_GAINHF, AL_EAXREVERB_MAX_GAINHF)
RPG_AUDIOFX_ATTR_F(rpg_reverb_gain_lf, AL_EAXREVERB_GAINLF, AL_EAXREVERB_MIN_GAINLF, AL_EAXREVERB_MAX_GAINLF)
RPG_AUDIOFX_ATTR_F(rpg_reverb_decay_time, AL_EAXREVERB_DECAY_TIME, AL_EAXREVERB_MIN_DECAY_TIME, AL_EAXREVERB_MAX_DECAY_TIME)
RPG_AUDIOFX_ATTR_F(rpg_reverb_hf_ratio, AL_EAXREVERB_DECAY_HFRATIO, AL_EAXREVERB_MIN_DECAY_HFRATIO, AL_EAXREVERB_MAX_DECAY_HFRATIO)
RPG_AUDIOFX_ATTR_F(rpg_reverb_lf_ratio, AL_EAXREVERB_DECAY_LFRATIO, AL_EAXREVERB_MIN_DECAY_LFRATIO, AL_EAXREVERB_MAX_DECAY_LFRATIO)
RPG_AUDIOFX_ATTR_F(rpg_reverb_reflections_gain, AL_EAXREVERB_REFLECTIONS_GAIN, AL_EAXREVERB_MIN_REFLECTIONS_GAIN, AL_EAXREVERB_MAX_REFLECTIONS_GAIN)
RPG_AUDIOFX_ATTR_F(rpg_reverb_reflections_delay, AL_EAXREVERB_REFLECTIONS_DELAY, AL_EAXREVERB_MIN_REFLECTIONS_DELAY, AL_EAXREVERB_MAX_REFLECTIONS_DELAY)
RPG_AUDIOFX_ATTR_F(rpg_reverb_late_gain, AL_EAXREVERB_LATE_REVERB_GAIN, AL_EAXREVERB_MIN_LATE_REVERB_GAIN, AL_EAXREVERB_MAX_LATE_REVERB_GAIN)
RPG_AUDIOFX_ATTR_F(rpg_reverb_late_delay, AL_EAXREVERB_LATE_REVERB_DELAY, AL_EAXREVERB_MIN_LATE_REVERB_DELAY, AL_EAXREVERB_MAX_LATE_REVERB_DELAY)
RPG_AUDIOFX_ATTR_F(rpg_reverb_echo_time, AL_EAXREVERB_ECHO_TIME, AL_EAXREVERB_MIN_ECHO_TIME, AL_EAXREVERB_MAX_ECHO_TIME)
RPG_AUDIOFX_ATTR_F(rpg_reverb_echo_depth, AL_EAXREVERB_ECHO_DEPTH, AL_EAXREVERB_MIN_ECHO_DEPTH, AL_EAXREVERB_MAX_ECHO_DEPTH)
RPG_AUDIOFX_ATTR_F(rpg_reverb_modulation_time, AL_EAXREVERB_MODULATION_TIME, AL_EAXREVERB_MIN_MODULATION_TIME, AL_EAXREVERB_MAX_MODULATION_TIME)
RPG_AUDIOFX_ATTR_F(rpg_reverb_modulation_depth, AL_EAXREVERB_MODULATION_DEPTH, AL_EAXREVERB_MIN_MODULATION_DEPTH, AL_EAXREVERB_MAX_MODULATION_DEPTH)
RPG_AUDIOFX_ATTR_F(rpg_reverb_air_absorption, AL_EAXREVERB_AIR_ABSORPTION_GAINHF, AL_EAXREVERB_MAX_AIR_ABSORPTION_GAINHF, AL_EAXREVERB_MAX_AIR_ABSORPTION_GAINHF)
RPG_AUDIOFX_ATTR_F(rpg_reverb_hf_reference, AL_EAXREVERB_HFREFERENCE, AL_EAXREVERB_MIN_HFREFERENCE, AL_EAXREVERB_MAX_HFREFERENCE)
RPG_AUDIOFX_ATTR_F(rpg_reverb_lf_reference, AL_EAXREVERB_LFREFERENCE, AL_EAXREVERB_MIN_LFREFERENCE, AL_EAXREVERB_MAX_LFREFERENCE)
RPG_AUDIOFX_ATTR_F(rpg_reverb_room_rolloff, AL_EAXREVERB_ROOM_ROLLOFF_FACTOR, AL_EAXREVERB_MIN_ROOM_ROLLOFF_FACTOR, AL_EAXREVERB_MAX_ROOM_ROLLOFF_FACTOR)
RPG_AUDIOFX_ATTR_BOOL(rpg_reverb_decay_limit, AL_EAXREVERB_DECAY_HFLIMIT)
RPG_AUDIOFX_ATTR_VEC(rpg_reverb_reflections_pan, AL_EAXREVERB_REFLECTIONS_PAN)
RPG_AUDIOFX_ATTR_VEC(rpg_reverb_late_pan, AL_EAXREVERB_LATE_REVERB_PAN)

// CHorus
RPG_AUDIOFX_INIT_FUNC(chorus, AL_EFFECT_CHORUS)
RPG_AUDIOFX_ATTR_I(rpg_chorus_waveform, AL_CHORUS_WAVEFORM, AL_CHORUS_MIN_WAVEFORM, AL_CHORUS_MAX_WAVEFORM)
RPG_AUDIOFX_ATTR_I(rpg_chorus_phase, AL_CHORUS_PHASE, AL_CHORUS_MIN_PHASE, AL_CHORUS_MAX_PHASE)
RPG_AUDIOFX_ATTR_F(rpg_chorus_rate, AL_CHORUS_RATE, AL_CHORUS_MIN_RATE, AL_CHORUS_MAX_RATE)
RPG_AUDIOFX_ATTR_F(rpg_chorus_depth, AL_CHORUS_DEPTH, AL_CHORUS_MIN_DEPTH, AL_CHORUS_MAX_DEPTH)
RPG_AUDIOFX_ATTR_F(rpg_chorus_feedback, AL_CHORUS_FEEDBACK, AL_CHORUS_MIN_FEEDBACK, AL_CHORUS_MAX_FEEDBACK)
RPG_AUDIOFX_ATTR_F(rpg_chorus_delay, AL_CHORUS_DELAY, AL_CHORUS_MIN_DELAY, AL_CHORUS_MAX_DELAY)

// Distortion
RPG_AUDIOFX_INIT_FUNC(distortion, AL_EFFECT_DISTORTION)
RPG_AUDIOFX_ATTR_F(rpg_distortion_edge, AL_DISTORTION_EDGE, AL_DISTORTION_MIN_EDGE, AL_DISTORTION_MAX_EDGE)
RPG_AUDIOFX_ATTR_F(rpg_distortion_gain, AL_DISTORTION_GAIN, AL_DISTORTION_MIN_GAIN, AL_DISTORTION_MAX_GAIN)
RPG_AUDIOFX_ATTR_F(rpg_distortion_lowpass_cutoff, AL_DISTORTION_LOWPASS_CUTOFF, AL_DISTORTION_MIN_LOWPASS_CUTOFF, AL_DISTORTION_MAX_LOWPASS_CUTOFF)
RPG_AUDIOFX_ATTR_F(rpg_distortion_eq_center, AL_DISTORTION_EQCENTER, AL_DISTORTION_MIN_EQCENTER, AL_DISTORTION_MAX_EQCENTER)
RPG_AUDIOFX_ATTR_F(rpg_distortion_eq_bandwidth, AL_DISTORTION_EQBANDWIDTH, AL_DISTORTION_MIN_EQBANDWIDTH, AL_DISTORTION_MAX_EQBANDWIDTH)

// Echo
RPG_AUDIOFX_INIT_FUNC(echo, AL_EFFECT_ECHO)
RPG_AUDIOFX_ATTR_F(rpg_echo_delay, AL_ECHO_DELAY, AL_ECHO_MIN_DELAY, AL_ECHO_MAX_DELAY)
RPG_AUDIOFX_ATTR_F(rpg_echo_lr_delay, AL_ECHO_LRDELAY, AL_ECHO_MIN_LRDELAY, AL_ECHO_MAX_LRDELAY)
RPG_AUDIOFX_ATTR_F(rpg_echo_damping, AL_ECHO_DAMPING, AL_ECHO_MIN_DAMPING, AL_ECHO_MAX_DAMPING)
RPG_AUDIOFX_ATTR_F(rpg_echo_feedback, AL_ECHO_FEEDBACK, AL_ECHO_MIN_FEEDBACK, AL_ECHO_MAX_FEEDBACK)
RPG_AUDIOFX_ATTR_F(rpg_echo_spread, AL_ECHO_SPREAD, AL_ECHO_MIN_SPREAD, AL_ECHO_MAX_SPREAD)

// Flanger
RPG_AUDIOFX_INIT_FUNC(flanger, AL_EFFECT_FLANGER)
RPG_AUDIOFX_ATTR_I(rpg_flanger_waveform, AL_FLANGER_WAVEFORM, AL_FLANGER_MIN_WAVEFORM, AL_FLANGER_MAX_WAVEFORM)
RPG_AUDIOFX_ATTR_I(rpg_flanger_phase, AL_FLANGER_PHASE, AL_FLANGER_MIN_PHASE, AL_FLANGER_MAX_PHASE)
RPG_AUDIOFX_ATTR_F(rpg_flanger_rate, AL_FLANGER_RATE, AL_FLANGER_MIN_RATE, AL_FLANGER_MAX_RATE)
RPG_AUDIOFX_ATTR_F(rpg_flanger_depth, AL_FLANGER_DEPTH, AL_FLANGER_MIN_DEPTH, AL_FLANGER_MAX_DEPTH)
RPG_AUDIOFX_ATTR_F(rpg_flanger_feedback, AL_FLANGER_FEEDBACK, AL_FLANGER_MIN_FEEDBACK, AL_FLANGER_MAX_FEEDBACK)
RPG_AUDIOFX_ATTR_F(rpg_flanger_delay, AL_FLANGER_DELAY, AL_FLANGER_MIN_DELAY, AL_FLANGER_MAX_DELAY)

// FrequencyShifter
RPG_AUDIOFX_INIT_FUNC(shifter, AL_EFFECT_FREQUENCY_SHIFTER)
RPG_AUDIOFX_ATTR_F(rpg_shifter_freq, AL_FREQUENCY_SHIFTER_FREQUENCY, AL_FREQUENCY_SHIFTER_MIN_FREQUENCY, AL_FREQUENCY_SHIFTER_MAX_FREQUENCY)
RPG_AUDIOFX_ATTR_I(rpg_shifter_left_dir, AL_FREQUENCY_SHIFTER_LEFT_DIRECTION, AL_FREQUENCY_SHIFTER_MIN_RIGHT_DIRECTION, AL_FREQUENCY_SHIFTER_MAX_LEFT_DIRECTION)
RPG_AUDIOFX_ATTR_I(rpg_shifter_right_dir, AL_FREQUENCY_SHIFTER_RIGHT_DIRECTION, AL_FREQUENCY_SHIFTER_RIGHT_DIRECTION, AL_FREQUENCY_SHIFTER_MAX_RIGHT_DIRECTION)

// Vocal Morpher
RPG_AUDIOFX_INIT_FUNC(vmorph, AL_EFFECT_VOCAL_MORPHER)
RPG_AUDIOFX_ATTR_I(rpg_vmorph_phoneme_a, AL_VOCAL_MORPHER_PHONEMEA, AL_VOCAL_MORPHER_MIN_PHONEMEA, AL_VOCAL_MORPHER_MAX_PHONEMEA)
RPG_AUDIOFX_ATTR_I(rpg_vmorph_phoneme_b, AL_VOCAL_MORPHER_PHONEMEB, AL_VOCAL_MORPHER_MIN_PHONEMEB, AL_VOCAL_MORPHER_MAX_PHONEMEB)
RPG_AUDIOFX_ATTR_I(rpg_vmorph_coarse_a, AL_VOCAL_MORPHER_PHONEMEA_COARSE_TUNING, AL_VOCAL_MORPHER_MIN_PHONEMEA_COARSE_TUNING, AL_VOCAL_MORPHER_MAX_PHONEMEA_COARSE_TUNING)
RPG_AUDIOFX_ATTR_I(rpg_vmorph_coarse_b, AL_VOCAL_MORPHER_PHONEMEB_COARSE_TUNING, AL_VOCAL_MORPHER_MIN_PHONEMEB_COARSE_TUNING, AL_VOCAL_MORPHER_MAX_PHONEMEB_COARSE_TUNING)
RPG_AUDIOFX_ATTR_I(rpg_vmorph_waveform, AL_VOCAL_MORPHER_WAVEFORM, AL_VOCAL_MORPHER_MIN_WAVEFORM, AL_VOCAL_MORPHER_MAX_WAVEFORM)
RPG_AUDIOFX_ATTR_F(rpg_vmorph_rate, AL_VOCAL_MORPHER_RATE, AL_VOCAL_MORPHER_MIN_RATE, AL_VOCAL_MORPHER_MAX_RATE)

// PitchShifter
RPG_AUDIOFX_INIT_FUNC(pitch, AL_EFFECT_PITCH_SHIFTER)
RPG_AUDIOFX_ATTR_I(rpg_pitch_coarse, AL_PITCH_SHIFTER_COARSE_TUNE, AL_PITCH_SHIFTER_MIN_COARSE_TUNE, AL_PITCH_SHIFTER_MAX_COARSE_TUNE)
RPG_AUDIOFX_ATTR_I(rpg_pitch_fine, AL_PITCH_SHIFTER_FINE_TUNE, AL_PITCH_SHIFTER_MIN_FINE_TUNE, AL_PITCH_SHIFTER_MAX_FINE_TUNE)

// RingModulator
RPG_AUDIOFX_INIT_FUNC(ringmod, AL_EFFECT_RING_MODULATOR)
RPG_AUDIOFX_ATTR_F(rpg_ringmod_freq, AL_RING_MODULATOR_FREQUENCY, AL_RING_MODULATOR_MIN_FREQUENCY, AL_RING_MODULATOR_MAX_FREQUENCY)
RPG_AUDIOFX_ATTR_F(rpg_ringmod_cutoff, AL_RING_MODULATOR_HIGHPASS_CUTOFF, AL_RING_MODULATOR_MIN_HIGHPASS_CUTOFF,AL_RING_MODULATOR_MAX_HIGHPASS_CUTOFF)
RPG_AUDIOFX_ATTR_I(rpg_ringmod_waveform, AL_RING_MODULATOR_WAVEFORM, AL_RING_MODULATOR_MIN_WAVEFORM, AL_RING_MODULATOR_MAX_WAVEFORM)

// AutoWah
RPG_AUDIOFX_INIT_FUNC(autowah, AL_EFFECT_AUTOWAH)
RPG_AUDIOFX_ATTR_F(rpg_autowah_attack, AL_AUTOWAH_ATTACK_TIME, AL_AUTOWAH_MIN_ATTACK_TIME, AL_AUTOWAH_MAX_ATTACK_TIME)
RPG_AUDIOFX_ATTR_F(rpg_autowah_release, AL_AUTOWAH_RELEASE_TIME, AL_AUTOWAH_MIN_RELEASE_TIME, AL_AUTOWAH_MAX_RELEASE_TIME)
RPG_AUDIOFX_ATTR_F(rpg_autowah_resonance, AL_AUTOWAH_RESONANCE, AL_AUTOWAH_MIN_RESONANCE, AL_AUTOWAH_MAX_RESONANCE)
RPG_AUDIOFX_ATTR_F(rpg_autowah_peak_gain, AL_AUTOWAH_PEAK_GAIN, AL_AUTOWAH_MIN_PEAK_GAIN, AL_AUTOWAH_MAX_PEAK_GAIN)

// Compressor
RPG_AUDIOFX_INIT_FUNC(compressor, AL_EFFECT_COMPRESSOR)
RPG_AUDIOFX_ATTR_BOOL(rpg_compressor_enabled, AL_COMPRESSOR_ONOFF)

// Equalizer
RPG_AUDIOFX_INIT_FUNC(eq, AL_EFFECT_EQUALIZER)
RPG_AUDIOFX_ATTR_F(rpg_eq_low_gain, AL_EQUALIZER_LOW_GAIN, AL_EQUALIZER_MIN_LOW_GAIN, AL_EQUALIZER_MAX_LOW_GAIN)
RPG_AUDIOFX_ATTR_F(rpg_eq_low_cutoff, AL_EQUALIZER_LOW_CUTOFF, AL_EQUALIZER_MIN_LOW_CUTOFF, AL_EQUALIZER_MAX_LOW_CUTOFF)
RPG_AUDIOFX_ATTR_F(rpg_eq_mid1_gain, AL_EQUALIZER_MID1_GAIN, AL_EQUALIZER_MIN_MID1_GAIN, AL_EQUALIZER_MAX_MID1_GAIN)
RPG_AUDIOFX_ATTR_F(rpg_eq_mid1_center, AL_EQUALIZER_MID1_CENTER, AL_EQUALIZER_MIN_MID1_CENTER, AL_EQUALIZER_MAX_MID1_CENTER)
RPG_AUDIOFX_ATTR_F(rpg_eq_mid1_width, AL_EQUALIZER_MID1_WIDTH, AL_EQUALIZER_MIN_MID1_WIDTH, AL_EQUALIZER_MAX_MID1_WIDTH)
RPG_AUDIOFX_ATTR_F(rpg_eq_mid2_gain, AL_EQUALIZER_MID2_GAIN, AL_EQUALIZER_MIN_MID2_GAIN, AL_EQUALIZER_MAX_MID2_GAIN)
RPG_AUDIOFX_ATTR_F(rpg_eq_mid2_center, AL_EQUALIZER_MID2_CENTER, AL_EQUALIZER_MIN_MID2_CENTER, AL_EQUALIZER_MAX_MID2_CENTER)
RPG_AUDIOFX_ATTR_F(rpg_eq_mid2_width, AL_EQUALIZER_MID2_WIDTH, AL_EQUALIZER_MIN_MID2_WIDTH, AL_EQUALIZER_MAX_MID2_WIDTH)
RPG_AUDIOFX_ATTR_F(rpg_eq_high_gain, AL_EQUALIZER_HIGH_GAIN, AL_EQUALIZER_MIN_HIGH_GAIN, AL_EQUALIZER_MAX_HIGH_GAIN)
RPG_AUDIOFX_ATTR_F(rpg_eq_high_cutoff, AL_EQUALIZER_HIGH_CUTOFF, AL_EQUALIZER_MIN_HIGH_CUTOFF, AL_EQUALIZER_MAX_HIGH_CUTOFF)

void rpg_audiofx_init(VALUE parent) {
    AL_LOAD_PROC(alGenEffects, LPALGENEFFECTS);
    AL_LOAD_PROC(alDeleteEffects, LPALDELETEEFFECTS);
    AL_LOAD_PROC(alIsEffect, LPALISEFFECT);
    AL_LOAD_PROC(alEffecti, LPALEFFECTI);
    AL_LOAD_PROC(alEffectiv, LPALEFFECTIV);
    AL_LOAD_PROC(alEffectf, LPALEFFECTF);
    AL_LOAD_PROC(alEffectfv, LPALEFFECTFV);
    AL_LOAD_PROC(alGetEffecti, LPALGETEFFECTI);
    AL_LOAD_PROC(alGetEffectiv, LPALGETEFFECTIV);
    AL_LOAD_PROC(alGetEffectf, LPALGETEFFECTF);
    AL_LOAD_PROC(alGetEffectfv, LPALGETEFFECTFV);

    VALUE effects = rb_define_module_under(parent, "Effects");
    VALUE effect = rb_define_class_under(effects, "Effect", rb_cObject);
    rpg_reverb_init(effects); // FIXME: Combine source files?

    rb_define_method(effect, "initialize", rpg_audiofx_initialize, 1);
    rb_define_method(effect, "valid?", rpg_audiofx_valid_p, 0);
    rb_define_method(effect, "setf", rpg_audiofx_setf, 2);
    rb_define_method(effect, "seti", rpg_audiofx_seti, 2);
    rb_define_method(effect, "getf", rpg_audiofx_getf, 2);
    rb_define_method(effect, "geti", rpg_audiofx_geti, 2);

    // Reverb
    VALUE reverb = rb_define_class_under(effects, "Reverb", effect);
    rb_define_alloc_func(reverb, rpg_audiofx_alloc);
    rb_define_method(reverb, "initialize", rpg_reverb_initialize, -1);
    RPG_AUDIOFX_DEF_ATTR(reverb, density);
    RPG_AUDIOFX_DEF_ATTR(reverb, diffusion);
    RPG_AUDIOFX_DEF_ATTR(reverb, gain);
    RPG_AUDIOFX_DEF_ATTR(reverb, gain_hf);
    RPG_AUDIOFX_DEF_ATTR(reverb, gain_lf);
    RPG_AUDIOFX_DEF_ATTR(reverb, decay_time);
    RPG_AUDIOFX_DEF_ATTR(reverb, hf_ratio);
    RPG_AUDIOFX_DEF_ATTR(reverb, lf_ratio);
    RPG_AUDIOFX_DEF_ATTR(reverb, reflections_gain);
    RPG_AUDIOFX_DEF_ATTR(reverb, reflections_delay);
    RPG_AUDIOFX_DEF_ATTR(reverb, late_gain);
    RPG_AUDIOFX_DEF_ATTR(reverb, late_delay);
    RPG_AUDIOFX_DEF_ATTR(reverb, echo_time);
    RPG_AUDIOFX_DEF_ATTR(reverb, echo_depth);
    RPG_AUDIOFX_DEF_ATTR(reverb, modulation_time);
    RPG_AUDIOFX_DEF_ATTR(reverb, modulation_depth);
    RPG_AUDIOFX_DEF_ATTR(reverb, air_absorption);
    RPG_AUDIOFX_DEF_ATTR(reverb, hf_reference);
    RPG_AUDIOFX_DEF_ATTR(reverb, lf_reference);
    RPG_AUDIOFX_DEF_ATTR(reverb, room_rolloff);
    RPG_AUDIOFX_DEF_ATTR(reverb, decay_limit);
    RPG_AUDIOFX_DEF_ATTR(reverb, reflections_pan);
    RPG_AUDIOFX_DEF_ATTR(reverb, late_pan);

    // Chorus
    RPG_AUDIOFX_DEF_INIT(chorus, "Chorus");
    RPG_AUDIOFX_DEF_ATTR(chorus, waveform);
    RPG_AUDIOFX_DEF_ATTR(chorus, phase);
    RPG_AUDIOFX_DEF_ATTR(chorus, rate);
    RPG_AUDIOFX_DEF_ATTR(chorus, depth);
    RPG_AUDIOFX_DEF_ATTR(chorus, feedback);
    RPG_AUDIOFX_DEF_ATTR(chorus, delay);

    // Distortion
    RPG_AUDIOFX_DEF_INIT(distortion, "Distortion");
    RPG_AUDIOFX_DEF_ATTR(distortion, edge);
    RPG_AUDIOFX_DEF_ATTR(distortion, gain);
    RPG_AUDIOFX_DEF_ATTR(distortion, lowpass_cutoff);
    RPG_AUDIOFX_DEF_ATTR(distortion, eq_center);
    RPG_AUDIOFX_DEF_ATTR(distortion, eq_bandwidth);

    // Echo
    RPG_AUDIOFX_DEF_INIT(echo, "Echo");
    RPG_AUDIOFX_DEF_ATTR(echo, delay);
    RPG_AUDIOFX_DEF_ATTR(echo, lr_delay);
    RPG_AUDIOFX_DEF_ATTR(echo, damping);
    RPG_AUDIOFX_DEF_ATTR(echo, feedback);
    RPG_AUDIOFX_DEF_ATTR(echo, spread);

    // Flanger
    RPG_AUDIOFX_DEF_INIT(flanger, "Flanger");
    RPG_AUDIOFX_DEF_ATTR(flanger, waveform);
    RPG_AUDIOFX_DEF_ATTR(flanger, phase);
    RPG_AUDIOFX_DEF_ATTR(flanger, rate);
    RPG_AUDIOFX_DEF_ATTR(flanger, depth);
    RPG_AUDIOFX_DEF_ATTR(flanger, feedback);
    RPG_AUDIOFX_DEF_ATTR(flanger, delay);

    // FrequencyShifter
    RPG_AUDIOFX_DEF_INIT(shifter, "FrequencyShifter");
    RPG_AUDIOFX_DEF_ATTR(shifter, freq);
    RPG_AUDIOFX_DEF_ATTR(shifter, left_dir);
    RPG_AUDIOFX_DEF_ATTR(shifter, right_dir);

    // Vocal Morpher
    RPG_AUDIOFX_DEF_INIT(vmorph, "VocalMorpher");
    RPG_AUDIOFX_DEF_ATTR(vmorph, phoneme_a);
    RPG_AUDIOFX_DEF_ATTR(vmorph, phoneme_b);
    RPG_AUDIOFX_DEF_ATTR(vmorph, coarse_a);
    RPG_AUDIOFX_DEF_ATTR(vmorph, coarse_b);
    RPG_AUDIOFX_DEF_ATTR(vmorph, waveform);
    RPG_AUDIOFX_DEF_ATTR(vmorph, rate);

    // PitchShifer
    RPG_AUDIOFX_DEF_INIT(pitch, "PitchShifter");
    RPG_AUDIOFX_DEF_ATTR(pitch, coarse);
    RPG_AUDIOFX_DEF_ATTR(pitch, fine);

    // RingModulator
    RPG_AUDIOFX_DEF_INIT(ringmod, "RingModulator");
    RPG_AUDIOFX_DEF_ATTR(ringmod, freq);
    RPG_AUDIOFX_DEF_ATTR(ringmod, cutoff);
    RPG_AUDIOFX_DEF_ATTR(ringmod, waveform);

    // AutoWah
    RPG_AUDIOFX_DEF_INIT(autowah, "AutoWah");
    RPG_AUDIOFX_DEF_ATTR(autowah, attack);
    RPG_AUDIOFX_DEF_ATTR(autowah, release);
    RPG_AUDIOFX_DEF_ATTR(autowah, resonance);
    RPG_AUDIOFX_DEF_ATTR(autowah, peak_gain);

    // Compressor
    RPG_AUDIOFX_DEF_INIT(compressor, "Compressor");
    RPG_AUDIOFX_DEF_ATTR(compressor, enabled);

    // Equalizer
    RPG_AUDIOFX_DEF_INIT(eq, "Equalizer");
    RPG_AUDIOFX_DEF_ATTR(eq, low_gain);
    RPG_AUDIOFX_DEF_ATTR(eq, low_cutoff);
    RPG_AUDIOFX_DEF_ATTR(eq, mid1_gain);
    RPG_AUDIOFX_DEF_ATTR(eq, mid1_center);
    RPG_AUDIOFX_DEF_ATTR(eq, mid1_width);
    RPG_AUDIOFX_DEF_ATTR(eq, mid2_gain);
    RPG_AUDIOFX_DEF_ATTR(eq, mid2_center);
    RPG_AUDIOFX_DEF_ATTR(eq, mid2_width);
    RPG_AUDIOFX_DEF_ATTR(eq, high_gain);
    RPG_AUDIOFX_DEF_ATTR(eq, high_cutoff);
}
