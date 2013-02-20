// Copyright (c) 2013, Philipp Muenzel mail@philippmuenzel.de
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

#ifndef ALSOUNDBUFFER_H
#define ALSOUNDBUFFER_H

#include <string>
#include <stdexcept>

#include <boost/noncopyable.hpp>

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

#include "namespaces.h"


namespace PPLNAMESPACE {

/**
  * This class encapsulates openALs buffers and sources, does the resource handling
  * and provides the listener setup.
  * @version 0.5
  * @author (c) 2009-2011 by Philipp Muenzel
  */
class ALSoundBuffer: boost::noncopyable
{
public:
    class SoundPlayingError : public std::runtime_error
    {
    public:
        SoundPlayingError(const std::string& msg):
            std::runtime_error(msg)
        {}
    };

    class SoundBufferError : public SoundPlayingError
    {
    public:
        SoundBufferError(const std::string& msg):
            SoundPlayingError(msg)
        {}
    };

    class SoundSourceError : public SoundPlayingError
    {
    public:
        SoundSourceError(const std::string& msg):
            SoundPlayingError(msg)
        {}
    };

    /**
      * create a new soundbuffer by creating a buffer from a file (format depends
      * on what alut distro supports). Sets up the openAL source at coordinate origin
      * @param filename the sound to load as a path relative to X-Planes main directory
      * @exception throws an SoundPlayingError if Buffer could not be created
      */
    ALSoundBuffer(const std::string& filename) throw(SoundPlayingError);

    /**
      * release the resources, delete buffer and source
      */
    ~ALSoundBuffer();

    /**
      * set up the listener at coordinate origin and play the sound buffer
      * @return play command was successfull (false could mean invalid source or buffer)
      */
    bool play() throw(SoundPlayingError);

    /**
      * set source to looping the sound (effective next time play() called)
      * @param yes loop true or false
      */
    void setLoop(bool yes);

    /**
      * stop playback of the sound
      */
    void stop();

    /**
      * rewind to the start of the sound
      */
    void rewind();

    /**
      * is the sound currently in playback
      * @return bool is playing right now ?
      */
    bool isPlaying() const;

private:
    std::string m_name;
    ALuint m_buffer;
    ALuint m_source;
    ALboolean m_loop;
};

ALuint LoadWav(const std::string& fileName);

}


#endif // ALSOUNDBUFFER_H
