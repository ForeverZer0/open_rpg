
#include "./internal.h"
#include <AL/efx-presets.h>

VALUE rb_cReverb;

#define RPG_REVERB_PRESET(name)                                                                                                            \
    do {                                                                                                                                   \
        EFXEAXREVERBPROPERTIES v = EFX_REVERB_PRESET_##name;                                                                               \
        EFXEAXREVERBPROPERTIES *rv = ALLOC(EFXEAXREVERBPROPERTIES);                                                                        \
        memcpy(rv, &v, sizeof(EFXEAXREVERBPROPERTIES));                                                                                    \
        VALUE reverb = Data_Wrap_Struct(rb_cReverb, NULL, RUBY_DEFAULT_FREE, rv);                                                          \
        rb_obj_freeze(reverb);                                                                                                             \
        rb_define_const(rb_cReverb, #name, reverb);                                                                                        \
    } while (0)

#define RPG_DEF_REVERB_ATTR(name)                                                                                                          \
    rb_define_method(rb_cReverb, #name, rpg_reverb_get_##name, 0);                                                                         \
    rb_define_method(rb_cReverb, #name"=", rpg_reverb_set_##name, 1)

#define RPG_REVERB_ATTR(name, field, RUBY_MACRO, C_MACRO)                                                                                  \
    ATTR_READER(rpg_reverb_get_##name, EFXEAXREVERBPROPERTIES, field, RUBY_MACRO)                                                          \
    static VALUE rpg_reverb_set_##name(VALUE self, VALUE value) {                                                                          \
        if (RB_OBJ_FROZEN(self)) {                                                                                                         \
            rb_raise(rb_eFrozenError, "cannot modifiy frozen Reverb");                                                                     \
        }                                                                                                                                  \
        EFXEAXREVERBPROPERTIES *r = DATA_PTR(self);                                                                                        \
        r->field = C_MACRO(value);                                                                                                         \
    }

ALLOC_FUNC(rpg_reverb_alloc, EFXEAXREVERBPROPERTIES);
RPG_REVERB_ATTR(density, flDensity, DBL2NUM, NUM2FLT)
RPG_REVERB_ATTR(diffusion, flDiffusion, DBL2NUM, NUM2FLT)
RPG_REVERB_ATTR(gain, flGain, DBL2NUM, NUM2FLT)
RPG_REVERB_ATTR(gain_hf, flGainHF, DBL2NUM, NUM2FLT)
RPG_REVERB_ATTR(gain_lf, flGainLF, DBL2NUM, NUM2FLT)
RPG_REVERB_ATTR(decay_time, flDecayTime, DBL2NUM, NUM2FLT)
RPG_REVERB_ATTR(decay_hf_ratio, flDecayHFRatio, DBL2NUM, NUM2FLT)
RPG_REVERB_ATTR(decay_lf_ratio, flDecayLFRatio, DBL2NUM, NUM2FLT)
RPG_REVERB_ATTR(reflections_gain, flReflectionsGain, DBL2NUM, NUM2FLT)
RPG_REVERB_ATTR(reflections_delay, flReflectionsDelay, DBL2NUM, NUM2FLT)
RPG_REVERB_ATTR(late_reverb_gain, flLateReverbGain, DBL2NUM, NUM2FLT)
RPG_REVERB_ATTR(late_reverb_delay, flLateReverbDelay, DBL2NUM, NUM2FLT)
RPG_REVERB_ATTR(echo_time, flEchoTime, DBL2NUM, NUM2FLT)
RPG_REVERB_ATTR(echo_depth, flEchoDepth, DBL2NUM, NUM2FLT)
RPG_REVERB_ATTR(modulation_time, flModulationTime, DBL2NUM, NUM2FLT)
RPG_REVERB_ATTR(modulation_depth, flModulationDepth, DBL2NUM, NUM2FLT)
RPG_REVERB_ATTR(hf_reference, flHFReference, DBL2NUM, NUM2FLT)
RPG_REVERB_ATTR(lf_reference, flLFReference, DBL2NUM, NUM2FLT)
RPG_REVERB_ATTR(rolloff, flRoomRolloffFactor, DBL2NUM, NUM2FLT)
RPG_REVERB_ATTR(air_absorption, flAirAbsorptionGainHF, DBL2NUM, NUM2FLT)
RPG_REVERB_ATTR(decay_limit, iDecayHFLimit, RTEST, RB_BOOL)

static VALUE rpg_reverb_get_reflections_pan(VALUE self) {
    EFXEAXREVERBPROPERTIES *r = DATA_PTR(self);
    RPGvec3 *v = ALLOC(RPGvec3);
    memcpy(v, &r->flReflectionsPan[0], sizeof(RPGvec3));
    return Data_Wrap_Struct(rb_cVec3, NULL, RUBY_DEFAULT_FREE, v);
}

static VALUE rpg_reverb_set_reflections_pan(VALUE self, VALUE value) {
    if (RB_OBJ_FROZEN(self)) {                                                                                                         
        rb_raise(rb_eFrozenError, "cannot modifiy frozen object");                                                                     
    }   
    EFXEAXREVERBPROPERTIES *r = DATA_PTR(self);
    RPGvec3 *v = DATA_PTR(value);
    memcpy(&r->flReflectionsPan[0], v, sizeof(RPGvec3));
    return value;
}

static VALUE rpg_reverb_get_late_reverb_pan(VALUE self) {
    EFXEAXREVERBPROPERTIES *r = DATA_PTR(self);
    RPGvec3 *v = ALLOC(RPGvec3);
    memcpy(v, &r->flLateReverbPan[0], sizeof(RPGvec3));
    return Data_Wrap_Struct(rb_cVec3, NULL, RUBY_DEFAULT_FREE, v);
}

static VALUE rpg_reverb_set_late_reverb_pan(VALUE self, VALUE value) {
    if (RB_OBJ_FROZEN(self)) {                                                                                                         
        rb_raise(rb_eFrozenError, "cannot modifiy frozen object");                                                                     
    }  
    EFXEAXREVERBPROPERTIES *r = DATA_PTR(self);
    RPGvec3 *v = DATA_PTR(value);
    memcpy(&r->flLateReverbPan[0], v, sizeof(RPGvec3));
    return value;
}

void rpg_reverb_init(VALUE parent) {

    rb_cReverb = rb_define_class_under(parent, "Reverb", rb_cObject);
    rb_define_alloc_func(rb_cReverb, rpg_reverb_alloc);

    RPG_DEF_REVERB_ATTR(density);
    RPG_DEF_REVERB_ATTR(diffusion);
    RPG_DEF_REVERB_ATTR(gain);
    RPG_DEF_REVERB_ATTR(gain_hf);
    RPG_DEF_REVERB_ATTR(gain_lf);
    RPG_DEF_REVERB_ATTR(decay_time);
    RPG_DEF_REVERB_ATTR(decay_hf_ratio);
    RPG_DEF_REVERB_ATTR(decay_lf_ratio);
    RPG_DEF_REVERB_ATTR(reflections_gain);
    RPG_DEF_REVERB_ATTR(reflections_delay);
    RPG_DEF_REVERB_ATTR(late_reverb_gain);
    RPG_DEF_REVERB_ATTR(late_reverb_delay);
    RPG_DEF_REVERB_ATTR(echo_time);
    RPG_DEF_REVERB_ATTR(echo_depth);
    RPG_DEF_REVERB_ATTR(modulation_time);
    RPG_DEF_REVERB_ATTR(modulation_depth);
    RPG_DEF_REVERB_ATTR(hf_reference);
    RPG_DEF_REVERB_ATTR(lf_reference);
    RPG_DEF_REVERB_ATTR(rolloff);
    RPG_DEF_REVERB_ATTR(air_absorption);
    RPG_DEF_REVERB_ATTR(decay_limit);
    RPG_DEF_REVERB_ATTR(reflections_pan);
    RPG_DEF_REVERB_ATTR(late_reverb_pan);

    // General
    RPG_REVERB_PRESET(GENERIC);
    RPG_REVERB_PRESET(PADDEDCELL);
    RPG_REVERB_PRESET(ROOM);
    RPG_REVERB_PRESET(BATHROOM);
    RPG_REVERB_PRESET(LIVINGROOM);
    RPG_REVERB_PRESET(STONEROOM);
    RPG_REVERB_PRESET(AUDITORIUM);
    RPG_REVERB_PRESET(CONCERTHALL);
    RPG_REVERB_PRESET(CAVE);
    RPG_REVERB_PRESET(ARENA);
    RPG_REVERB_PRESET(HANGAR);
    RPG_REVERB_PRESET(CARPETEDHALLWAY);
    RPG_REVERB_PRESET(HALLWAY);
    RPG_REVERB_PRESET(STONECORRIDOR);
    RPG_REVERB_PRESET(ALLEY);
    RPG_REVERB_PRESET(FOREST);
    RPG_REVERB_PRESET(CITY);
    RPG_REVERB_PRESET(MOUNTAINS);
    RPG_REVERB_PRESET(QUARRY);
    RPG_REVERB_PRESET(PLAIN);
    RPG_REVERB_PRESET(PARKINGLOT);
    RPG_REVERB_PRESET(SEWERPIPE);
    RPG_REVERB_PRESET(UNDERWATER);
    RPG_REVERB_PRESET(DRUGGED);
    RPG_REVERB_PRESET(DIZZY);
    RPG_REVERB_PRESET(PSYCHOTIC);

    /* Castle Presets */
    RPG_REVERB_PRESET(CASTLE_SMALLROOM);
    RPG_REVERB_PRESET(CASTLE_SHORTPASSAGE);
    RPG_REVERB_PRESET(CASTLE_MEDIUMROOM);
    RPG_REVERB_PRESET(CASTLE_LARGEROOM);
    RPG_REVERB_PRESET(CASTLE_LONGPASSAGE);
    RPG_REVERB_PRESET(CASTLE_HALL);
    RPG_REVERB_PRESET(CASTLE_CUPBOARD);
    RPG_REVERB_PRESET(CASTLE_COURTYARD);
    RPG_REVERB_PRESET(CASTLE_ALCOVE);

    /* Factory Presets */
    RPG_REVERB_PRESET(FACTORY_SMALLROOM);
    RPG_REVERB_PRESET(FACTORY_SHORTPASSAGE);
    RPG_REVERB_PRESET(FACTORY_MEDIUMROOM);
    RPG_REVERB_PRESET(FACTORY_LARGEROOM);
    RPG_REVERB_PRESET(FACTORY_LONGPASSAGE);
    RPG_REVERB_PRESET(FACTORY_HALL);
    RPG_REVERB_PRESET(FACTORY_CUPBOARD);
    RPG_REVERB_PRESET(FACTORY_COURTYARD);
    RPG_REVERB_PRESET(FACTORY_ALCOVE);

    /* Ice Palace Presets */
    RPG_REVERB_PRESET(ICEPALACE_SMALLROOM);
    RPG_REVERB_PRESET(ICEPALACE_SHORTPASSAGE);
    RPG_REVERB_PRESET(ICEPALACE_MEDIUMROOM);
    RPG_REVERB_PRESET(ICEPALACE_LARGEROOM);
    RPG_REVERB_PRESET(ICEPALACE_LONGPASSAGE);
    RPG_REVERB_PRESET(ICEPALACE_HALL);
    RPG_REVERB_PRESET(ICEPALACE_CUPBOARD);
    RPG_REVERB_PRESET(ICEPALACE_COURTYARD);
    RPG_REVERB_PRESET(ICEPALACE_ALCOVE);

    /* Space Station Presets */
    RPG_REVERB_PRESET(SPACESTATION_SMALLROOM);
    RPG_REVERB_PRESET(SPACESTATION_SHORTPASSAGE);
    RPG_REVERB_PRESET(SPACESTATION_MEDIUMROOM);
    RPG_REVERB_PRESET(SPACESTATION_LARGEROOM);
    RPG_REVERB_PRESET(SPACESTATION_LONGPASSAGE);
    RPG_REVERB_PRESET(SPACESTATION_HALL);
    RPG_REVERB_PRESET(SPACESTATION_CUPBOARD);
    RPG_REVERB_PRESET(SPACESTATION_ALCOVE);

    /* Wooden Galleon Presets */
    RPG_REVERB_PRESET(WOODEN_SMALLROOM);
    RPG_REVERB_PRESET(WOODEN_SHORTPASSAGE);
    RPG_REVERB_PRESET(WOODEN_MEDIUMROOM);
    RPG_REVERB_PRESET(WOODEN_LARGEROOM);
    RPG_REVERB_PRESET(WOODEN_LONGPASSAGE);
    RPG_REVERB_PRESET(WOODEN_HALL);
    RPG_REVERB_PRESET(WOODEN_CUPBOARD);
    RPG_REVERB_PRESET(WOODEN_COURTYARD);
    RPG_REVERB_PRESET(WOODEN_ALCOVE);

    /* Sports Presets */
    RPG_REVERB_PRESET(SPORT_EMPTYSTADIUM);
    RPG_REVERB_PRESET(SPORT_SQUASHCOURT);
    RPG_REVERB_PRESET(SPORT_SMALLSWIMMINGPOOL);
    RPG_REVERB_PRESET(SPORT_LARGESWIMMINGPOOL);
    RPG_REVERB_PRESET(SPORT_GYMNASIUM);
    RPG_REVERB_PRESET(SPORT_FULLSTADIUM);
    RPG_REVERB_PRESET(SPORT_STADIUMTANNOY);

    /* Prefab Presets */
    RPG_REVERB_PRESET(PREFAB_WORKSHOP);
    RPG_REVERB_PRESET(PREFAB_SCHOOLROOM);
    RPG_REVERB_PRESET(PREFAB_PRACTISEROOM);
    RPG_REVERB_PRESET(PREFAB_OUTHOUSE);
    RPG_REVERB_PRESET(PREFAB_CARAVAN);

    /* Dome and Pipe Presets */
    RPG_REVERB_PRESET(DOME_TOMB);
    RPG_REVERB_PRESET(PIPE_SMALL);
    RPG_REVERB_PRESET(DOME_SAINTPAULS);
    RPG_REVERB_PRESET(PIPE_LONGTHIN);
    RPG_REVERB_PRESET(PIPE_LARGE);
    RPG_REVERB_PRESET(PIPE_RESONANT);

    /* Outdoors Presets */
    RPG_REVERB_PRESET(OUTDOORS_BACKYARD);
    RPG_REVERB_PRESET(OUTDOORS_ROLLINGPLAINS);
    RPG_REVERB_PRESET(OUTDOORS_DEEPCANYON);
    RPG_REVERB_PRESET(OUTDOORS_CREEK);
    RPG_REVERB_PRESET(OUTDOORS_VALLEY);

    /* Mood Presets */
    RPG_REVERB_PRESET(MOOD_HEAVEN);
    RPG_REVERB_PRESET(MOOD_HELL);
    RPG_REVERB_PRESET(MOOD_MEMORY);

    /* Driving Presets */
    RPG_REVERB_PRESET(DRIVING_COMMENTATOR);
    RPG_REVERB_PRESET(DRIVING_PITGARAGE);
    RPG_REVERB_PRESET(DRIVING_INCAR_RACER);
    RPG_REVERB_PRESET(DRIVING_INCAR_SPORTS);
    RPG_REVERB_PRESET(DRIVING_INCAR_LUXURY);
    RPG_REVERB_PRESET(DRIVING_FULLGRANDSTAND);
    RPG_REVERB_PRESET(DRIVING_EMPTYGRANDSTAND);
    RPG_REVERB_PRESET(DRIVING_TUNNEL);

    /* City Presets */
    RPG_REVERB_PRESET(CITY_STREETS);
    RPG_REVERB_PRESET(CITY_SUBWAY);
    RPG_REVERB_PRESET(CITY_MUSEUM);
    RPG_REVERB_PRESET(CITY_LIBRARY);
    RPG_REVERB_PRESET(CITY_UNDERPASS);
    RPG_REVERB_PRESET(CITY_ABANDONED);

    /* Misc. Presets */
    RPG_REVERB_PRESET(DUSTYROOM);
    RPG_REVERB_PRESET(CHAPEL);
    RPG_REVERB_PRESET(SMALLWATERROOM);
}
