// Copyright (C) 2008,2009,2010 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU General Public License version 2 or later
// as published by the Free Software Foundation, Inc.

#ifndef ALSOUNDBUFFER_H
#define ALSOUNDBUFFER_H

#include <string>
#include <stdexcept>

#include <boost/noncopyable.hpp>

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>

namespace XSPL {
    namespace Sound {


        /**
          * This class encapsulates openALs buffers and sources, does the resource handling
          * and provides the listener setup.
          * @version 0.3
          * @author (c) 2009, 2010 by Philipp Muenzel, Technische Universitaet Darmstadt, Department of Mathematics
          * @file alsoundbuffer.h
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
            ~ALSoundBuffer() throw();

            /**
              * set up the listener at coordinate origin and play the sound buffer
              * @return play command was successfull (false could mean invalid source or buffer)
              */
            bool play() throw(SoundPlayingError);

            /**
              * set source to looping the sound (effective next time play() called)
              * @param yes loop true or false
              */
            void setLoop(bool yes) throw();

            /**
              * stop playback of the sound
              */
            void stop() throw();

            /**
              * rewind to the start of the sound
              */
            void rewind() throw();

            /**
              * is the sound currently in playback
              * @return bool is playing right now ?
              */
            bool isPlaying() const throw();

        private:
            std::string m_name;
            ALuint m_buffer;
            ALuint m_source;
            ALboolean m_loop;
        };

    }
}

#endif // SOUNDBUFFER_H
