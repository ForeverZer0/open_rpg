#include "./rpg.h"
#include <AL/al.h>
#include <AL/alc.h>

ALCcontext *context;
ALCdevice *device;

VALUE rb_mAudio;

static VALUE rpg_audio_terminate(VALUE module) {
    alcDestroyContext(context);
    alcCloseDevice(device);
    return Qnil;
}

static VALUE rpg_audio_play(int argc, VALUE *argv, VALUE module) {
    VALUE sound, volume, opts;
    rb_scan_args(argc, argv, "11:", &sound, &volume, &opts);

    RPGsound *snd = DATA_PTR(sound); // TODO: Check NULL, etc

    ALuint source;
    alGenSources(1, &source);
    alSourcef(source, AL_GAIN, RTEST(volume) ? NUM2FLT(volume) : 1.0f);

    // TODO: Process options, pitch, etc
    alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);
    alSourcei(source, AL_PAUSED, AL_FALSE);


    size_t total = snd->info.frames * snd->info.channels;
    ALushort *data = RPG_MALLOC(total * sizeof(ALushort));
    memset(data, 0, total * sizeof(ALushort));

    sf_count_t read = sf_read_short(snd->file, data, total);
    if (read > 0) {
        ALuint buffer;
        alGenBuffers(1, &buffer);
        alBufferData(buffer, AL_FORMAT_STEREO16, data, total * sizeof(ALushort), snd->info.samplerate);

        alSourcei(source, AL_BUFFER, buffer);
        alSourcePlay(source);
    }


    RPG_FREE(data);

    return Qnil;
}

void rpg_audio_init(VALUE parent) {
    rb_mAudio = rb_define_module_under(parent, "Audio");
    rb_define_singleton_method(rb_mAudio, "terminate", rpg_audio_terminate, 0);

    rb_define_singleton_method(rb_mAudio, "play", rpg_audio_play, -1);








    device = alcOpenDevice(NULL);
    if (!device) {
        rb_raise(rb_eRPGError, "failed to initialize audio device");
    }
    context = alcCreateContext(device, NULL);
    if (!context) {
        rb_raise(rb_eRPGError, "failed to create audio context");
    }
    if (!alcMakeContextCurrent(context)) {
        rb_raise(rb_eRPGError, "failed to set audio context");
    }
}