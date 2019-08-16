#include "./rpg.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>

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
    if (snd->file) {

    ALuint source, buffer;
    alGenSources(1, &source);
    alGenBuffers(1, &buffer);

    size_t n = snd->info.frames * snd->info.channels;
    void *data = RPG_MALLOC(n * snd->sample_size);

    sf_read_float(snd->file, data, n);

    alBufferData(buffer, snd->al_format, data, n * snd->sample_size, snd->info.samplerate);
    
    alSourcei(source, AL_BUFFER, buffer);
    alSourcePlay(source);

    }
    return Qnil;
}

void rpg_audio_init(VALUE parent) {
    rb_mAudio = rb_define_module_under(parent, "Audio");
    rb_define_singleton_method(rb_mAudio, "terminate", rpg_audio_terminate, 0);

    rb_define_singleton_method(rb_mAudio, "play", rpg_audio_play, -1);


    device = alcOpenDevice(NULL);
    context = alcCreateContext(device, NULL); // TODO: Play around with attr
    alcMakeContextCurrent(context);

}