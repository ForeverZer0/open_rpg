#ifndef OPEN_RPG_AUDIO_H
#define OPEN_RPG_AUDIO_H 1

#include "./rpg.h"
#include <AL/al.h>
#include <AL/alext.h>
#include <AL/efx.h>
#include <sndfile.h>

#define AL_LOAD_PROC(x, y) ((x) = (y)alGetProcAddress(#x))

#define CHECK_AL_ERROR(msg)                                                                                                                \
    if (alGetError() != AL_NO_ERROR) {                                                                                                     \
        rb_raise(rb_eRPGError, msg);                                                                                                       \
    }

void rpg_audio_init(VALUE parent);
void rpg_audiofx_init(VALUE parent);
void rpg_reverb_init(VALUE parent);

/**
 * @brief The OpenRPG::Audio module.
 */
extern VALUE rb_mAudio;

/**
 * @brief The base OpenRPG::Audio::Effect class
 */
extern VALUE rb_cReverbPreset;

/**
 * @brief Contains a loaded sound file and information about its internal format.
 */
typedef struct _RPGsound {
    SNDFILE *file;         /** A pointer to the audio file. */
    SF_INFO info;          /** Info structure describing internal format of the audio file. */
    ALuint buffer;         /** The OpenAL buffer for the sound, which can be shared by multiple sources. */
    pthread_mutex_t mutex; /** Mutex lock for thread-safety. */
} RPGsound;

typedef struct {
    ALint type;
    ALuint effect;
} RPGeffect;

/**
 * @brief Loads an audio file into an RPGsound structure.
 *
 * @param fname The path of the audio file.
 * @param sound A pointer where to store the loaded sound.
 * @param buffer_now Indicate to buffer data now on the current thread (blocking), else it will buffer later (non-blocking), but may have slight delay first play for large files.
 * @return int Non-zero if an error occurred loading.
 */
int rpg_sound_load(const char *fname, ALboolean buffer_now, RPGsound *sound);

#endif /* OPEN_RPG_AUDIO_H */