// Copyright (C) 2008-2011 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, Inc.

#ifndef ALSOUNDBUFFER_H
#define ALSOUNDBUFFER_H

#include <string>
#include <stdexcept>

#ifdef CRJ
#define boost crjboost
#endif
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

namespace PPL {


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
