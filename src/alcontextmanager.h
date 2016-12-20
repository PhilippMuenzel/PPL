/* Copyright (c) 2013, Philipp Muenzel mail@philippmuenzel.de
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// The views and conclusions contained in the software and documentation are those
// of the authors and should not be interpreted as representing official policies,
// either expressed or implied, of the FreeBSD Project.
*/

#ifndef ALCONTEXTMANAGER_H
#define ALCONTEXTMANAGER_H

#include <string>

#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_map.hpp>

#if APL == 1
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#elif IBM == 1
#include <AL/al.h>
#include <AL/alc.h>
#elif LIN == 1
#include <AL/al.h>
#include <AL/alc.h>
#else
#error "No platform defined"
#endif

#include "alsoundbuffer.h"
#include "namespaces.h"

namespace PPLNAMESPACE {

/**
  * Encapsulates all openAL and alut related stuff for looking up devices, handling different
  * contexts etc. Stores SoundBuffer istances to keep track of the currently loaded sounds
  * and provides convenient functions for standard operations like playing.
  *
  * @version 0.5
  * @author (c) 2009-2011 by Philipp Muenzel
  */
class ALContextManager : boost::noncopyable
{
public:
    class SoundNotFoundError : public std::runtime_error
    {
    public:
        SoundNotFoundError(const std::string& msg):
            std::runtime_error(msg)
        {}
    };

    class SoundLoadError : public std::runtime_error
    {
    public:
        SoundLoadError(const std::string& msg):
            std::runtime_error(msg)
        {}
    };

    class SoundPlayError : public std::runtime_error
    {
    public:
        SoundPlayError(const std::string& msg):
            std::runtime_error(msg)
        {}
    };

    /**
      * initializes openAL and alut and creates context
      */
    ALContextManager();

    /**
      * deletes context and cleans up loaded sounds
      */
    ~ALContextManager();

    /**
      * tries to load a sound by a file (format depends on what alut distro supports)
      * and stores it locally in a map, providing access by an integer key
      * @param filename the sound to load as a path relative to X-Planes main directory
      * @return unique integer id for adressing the sound buffer
      * @exception throws a SoundLoadError if file cannot be found or has unacceptable format
      */
    int addSoundFromFile(const std::string& filename) throw(SoundLoadError);

    /**
      * removes the sound from the map and deletes its buffer
      * @param id the sound buffers id in the map
      */
    void removeSound(int id) throw(SoundNotFoundError);

    /**
      * starts playback of the sound (playback continues when function returns)
      * @param id the sound buffers id in the map
      */
    bool playSound(int id) throw(SoundNotFoundError, SoundPlayError);

    /**
      * stops playback of the sound
      * @param id the sound buffers id in the map
      */
    void stopSound(int id) throw(SoundNotFoundError);

    /**
      * marks the sound to be played in a loop (when playback starts by play() )
      * @param id the sound buffers id in the map
      */
    void loopSound(int id) throw(SoundNotFoundError);

    /**
      * removes the looping flag (playback does stop when the sound's end is reached)
      * @param id the sound buffers id in the map
      */
    void unLoopSound(int id) throw(SoundNotFoundError);

    /**
      * rewinds the sound to it's starting position
      * @param id the sound buffers id in the map
      */
    void rewindSound(int id) throw(SoundNotFoundError);


private:
    ALSoundBuffer* findSoundById(int id) throw(SoundNotFoundError);
    void deleteAllSounds();

private:
    boost::ptr_map<int, ALSoundBuffer> m_sounds;
    int m_internal_counter;
    ALCdevice* m_device;
    ALCcontext* m_my_context;
    ALCcontext* m_current_context;
};

}

#endif // ALCONTEXTMANAGER_H
