#include "./audio.h"
#include "./internal.h"

VALUE rb_cEffect;

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

#define RPG_AUDIOFX_INIT_FUNC(klass, type)                                                                                                 \
    static VALUE rpg_##klass##_initialize(VALUE self) {                                                                                       \
        VALUE t = NUM2INT(type);                                                                                                           \
        rb_call_super(1, &t);                                                                                                              \
        return Qnil;                                                                                                                       \
    }

#define RPG_AUDIOFX_DEF_ATTR(type, name)                                                                                                   \
    rb_define_method(type, #name, rpg_##type##_##name##_get, 0);                                                                           \
    rb_define_method(type, #name "=", rpg_##type##_##name##_set, 1)

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

ALLOC_FUNC(rpg_audiofx_alloc, RPGeffect);

static void rpg_audiofx_free(void *data) {
    RPGeffect *e = data;
    alDeleteEffects(1, &e->effect);
    RPG_FREE(data);
}

static VALUE rpg_audiofx_initialize(VALUE self, VALUE type) {
    RPGeffect *e = DATA_PTR(self);
    e->type = NUM2INT(type);
    alGenEffects(1, &e->effect);
    alEffecti(e->effect, AL_EFFECT_TYPE, e->type);
    if (alGetError() != AL_NO_ERROR) {
        alDeleteEffects(1, &e->effect);
        rb_raise(rb_eRPGError, "failed to create audio effect");
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
    // TODO: Throw if error, or silent fail?
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
    // TODO: Throw if error, or silent fail?
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

// CHorus
RPG_AUDIOFX_INIT_FUNC(chorus, AL_EFFECT_CHORUS)
RPG_AUDIOFX_ATTR_I(rpg_chorus_waveform, AL_CHORUS_WAVEFORM, AL_CHORUS_MIN_WAVEFORM, AL_CHORUS_MAX_WAVEFORM)
RPG_AUDIOFX_ATTR_I(rpg_chorus_phase, AL_CHORUS_PHASE, AL_CHORUS_MIN_PHASE, AL_CHORUS_MAX_PHASE)
RPG_AUDIOFX_ATTR_F(rpg_chorus_rate, AL_CHORUS_RATE, AL_CHORUS_MIN_RATE, AL_CHORUS_MAX_RATE)
RPG_AUDIOFX_ATTR_F(rpg_chorus_depth, AL_CHORUS_DEPTH, AL_CHORUS_MIN_DEPTH, AL_CHORUS_MAX_DEPTH)
RPG_AUDIOFX_ATTR_F(rpg_chorus_feedback, AL_CHORUS_FEEDBACK, AL_CHORUS_MIN_FEEDBACK, AL_CHORUS_MAX_FEEDBACK)
RPG_AUDIOFX_ATTR_F(rpg_chorus_delay, AL_CHORUS_DELAY, AL_CHORUS_MIN_DELAY, AL_CHORUS_MAX_DELAY)

// Flanger
RPG_AUDIOFX_INIT_FUNC(flanger, AL_EFFECT_FLANGER)
RPG_AUDIOFX_ATTR_I(rpg_flanger_waveform, AL_FLANGER_WAVEFORM, AL_FLANGER_MIN_WAVEFORM, AL_FLANGER_MAX_WAVEFORM)
RPG_AUDIOFX_ATTR_I(rpg_flanger_phase, AL_FLANGER_PHASE, AL_FLANGER_MIN_PHASE, AL_FLANGER_MAX_PHASE)
RPG_AUDIOFX_ATTR_F(rpg_flanger_rate, AL_FLANGER_RATE, AL_FLANGER_MIN_RATE, AL_FLANGER_MAX_RATE)
RPG_AUDIOFX_ATTR_F(rpg_flanger_depth, AL_FLANGER_DEPTH, AL_FLANGER_MIN_DEPTH, AL_FLANGER_MAX_DEPTH)
RPG_AUDIOFX_ATTR_F(rpg_flanger_feedback, AL_FLANGER_FEEDBACK, AL_FLANGER_MIN_FEEDBACK, AL_FLANGER_MAX_FEEDBACK)
RPG_AUDIOFX_ATTR_F(rpg_flanger_delay, AL_FLANGER_DELAY, AL_FLANGER_MIN_DELAY, AL_FLANGER_MAX_DELAY)

void rpg_audiofx_init(VALUE parent) {
    rb_cEffect = rb_define_class_under(parent, "Effect", rb_cObject);
    rb_define_alloc_func(rb_cEffect, rpg_audiofx_alloc);

    rb_define_method(rb_cEffect, "initialize", rpg_audiofx_initialize, 1);
    rb_define_method(rb_cEffect, "valid?", rpg_audiofx_valid_p, 0);
    rb_define_method(rb_cEffect, "setf", rpg_audiofx_setf, 2);
    rb_define_method(rb_cEffect, "seti", rpg_audiofx_seti, 2);
    rb_define_method(rb_cEffect, "getf", rpg_audiofx_getf, 2);
    rb_define_method(rb_cEffect, "geti", rpg_audiofx_geti, 2);

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


    // Chorus
    VALUE chorus = rb_define_class_under(effects, "Chorus", rb_cEffect);
    rb_define_method(chorus, "initialize", rpg_chorus_initialize, 0);
    RPG_AUDIOFX_DEF_ATTR(chorus, waveform);
    RPG_AUDIOFX_DEF_ATTR(chorus, phase);
    RPG_AUDIOFX_DEF_ATTR(chorus, rate);
    RPG_AUDIOFX_DEF_ATTR(chorus, depth);
    RPG_AUDIOFX_DEF_ATTR(chorus, feedback);
    RPG_AUDIOFX_DEF_ATTR(chorus, delay);

    // Flanger
    VALUE flanger = rb_define_class_under(effects, "Flanger", rb_cEffect);
    rb_define_method(flanger, "initialize", rpg_flanger_initialize, 0);
    RPG_AUDIOFX_DEF_ATTR(flanger, waveform);
    RPG_AUDIOFX_DEF_ATTR(flanger, phase);
    RPG_AUDIOFX_DEF_ATTR(flanger, rate);
    RPG_AUDIOFX_DEF_ATTR(flanger, depth);
    RPG_AUDIOFX_DEF_ATTR(flanger, feedback);
    RPG_AUDIOFX_DEF_ATTR(flanger, delay);
}