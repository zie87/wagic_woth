//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
//
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
//
//-------------------------------------------------------------------------------------

#ifndef JSOUNDSYSTEM_H
#define JSOUNDSYSTEM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "JTypes.h"

#ifdef WIN32
#include <windows.h>
#elif defined(PSP)
#include <pspgu.h>
#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <time.h>
#include <string.h>
#include <pspaudiolib.h>
#include <psprtc.h>

#include "JAudio.h"
#include "JMP3.h"
#endif

//------------------------------------------------------------------------------------------------

class JMusic {
public:
    JMusic();
    ~JMusic();
    void Update();
    int getPlayTime();

#if defined(PSP)
    JMP3* mTrack;
#else
    void* mTrack;
#endif
};

//------------------------------------------------------------------------------------------------
class JSample {
public:
    JSample();
    ~JSample();

    unsigned long fileSize();

#if defined(PSP)
    WAVDATA* mSample;
#else
    void* mSample;
#endif
};

//////////////////////////////////////////////////////////////////////////
/// Sound engine for playing sound effects (WAV) and background
/// music (MP3).
///
//////////////////////////////////////////////////////////////////////////
class JSoundSystem {
public:
    //////////////////////////////////////////////////////////////////////////
    /// Get the singleton instance
    ///
    //////////////////////////////////////////////////////////////////////////
    static JSoundSystem* GetInstance();

    static void Destroy();

    //////////////////////////////////////////////////////////////////////////
    /// Load music.
    ///
    /// @note MP3 is the only supported format for the moment.
    ///
    /// @param filename - Name of the music file.
    ///
    //////////////////////////////////////////////////////////////////////////
    JMusic* LoadMusic(const char* fileName);

    //////////////////////////////////////////////////////////////////////////
    /// Delete music from memory.
    ///
    /// @param music - Music to be deleted.
    ///
    //////////////////////////////////////////////////////////////////////////
    // void FreeMusic(JMusic *music);

    //////////////////////////////////////////////////////////////////////////
    /// Play music.
    ///
    /// @param music - Music to be played.
    /// @param looping - Play the music in a loop.
    ///
    //////////////////////////////////////////////////////////////////////////
    void PlayMusic(JMusic* music, bool looping = false);

    //////////////////////////////////////////////////////////////////////////
    /// Stop playing.
    ///
    /// @param music - Music to be stopped.
    ///
    //////////////////////////////////////////////////////////////////////////
    void StopMusic(JMusic* music);

    //////////////////////////////////////////////////////////////////////////
    /// Resume playing.
    ///
    /// @param music - Music to be resumed.
    ///
    //////////////////////////////////////////////////////////////////////////
    void ResumeMusic(JMusic* music);

    //////////////////////////////////////////////////////////////////////////
    /// Pause playing.
    ///
    /// @param music - Music to be paused.
    ///
    //////////////////////////////////////////////////////////////////////////
    void PauseMusic(JMusic* music);

    //////////////////////////////////////////////////////////////////////////
    /// Load sound effect.
    ///
    /// @note WAV sound effect only.
    ///
    /// @param fileName - Sound effect for loading.
    ///
    //////////////////////////////////////////////////////////////////////////
    JSample* LoadSample(const char* fileName);

    //////////////////////////////////////////////////////////////////////////
    /// Delete sound effect from memory.
    ///
    /// @param sample - Sound to be deleted.
    ///
    //////////////////////////////////////////////////////////////////////////
    // void FreeSample(JSample *sample);

    //////////////////////////////////////////////////////////////////////////
    /// Play sound effect.
    ///
    /// @param sample - Sound for playing.
    ///
    //////////////////////////////////////////////////////////////////////////
    void PlaySample(JSample* sample);

    //////////////////////////////////////////////////////////////////////////
    /// Set volume for audio playback.
    ///
    /// @param volume - New volume.
    ///
    //////////////////////////////////////////////////////////////////////////
    void SetVolume(int volume);

    void SetMusicVolume(int volume);

    void SetSfxVolume(int volume);

    int mChannel;

protected:
    JSoundSystem();
    ~JSoundSystem();

    void InitSoundSystem();
    void DestroySoundSystem();

private:
    JMusic* mCurrentMusic;
    JSample* mCurrentSample;

    int mVolume;
    int mMusicVolume;
    int mSampleVolume;

    static JSoundSystem* mInstance;
};

#endif
