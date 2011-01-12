// Copyright (C) 2008-2011 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU General Public License version 2 or later
// as published by the Free Software Foundation, Inc.

#include <sstream>

#include "alsoundbuffer.h"

using namespace PPL;

ALSoundBuffer::ALSoundBuffer(const std::string& filename) throw(SoundPlayingError):
        m_name(filename)
{
    ALfloat source_position[] = { 0.0, 0.0, 0.0 };
    ALfloat source_velocity[] = { 0.0, 0.0, 0.0 };
    m_loop = AL_FALSE;
    // Clear present errors
    alGetError();
    alutGetError();

    m_buffer = alutCreateBufferFromFile(filename.c_str());
    if( m_buffer == AL_NONE)
    {
        std::stringstream stream;
        stream << "ALUT: Buffer creation failed: "
                << alutGetErrorString(alutGetError());
        throw SoundBufferError(stream.str());
    } else {
        if (alGetError() != AL_NO_ERROR)
        {
            throw SoundBufferError("Error in creating buffer");
        }
        alGenSources(1, &m_source);
        if (alGetError() != AL_NO_ERROR)
        {
            throw SoundSourceError("Error in creating source");
        }
        alSourcei (m_source, AL_BUFFER,   m_buffer   );
        alSourcef (m_source, AL_PITCH,    1.0      );
        alSourcef (m_source, AL_GAIN,     1.0      );
        alSourcefv(m_source, AL_POSITION, source_position);
        alSourcefv(m_source, AL_VELOCITY, source_velocity);
        alSourcei (m_source, AL_LOOPING,  m_loop     );
        if (alGetError() != AL_NO_ERROR)
        {
            throw SoundSourceError("Error in setting up source");
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ALSoundBuffer::~ALSoundBuffer()
{
    alDeleteSources(1, &m_source);
    alDeleteBuffers(1, &m_buffer);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool ALSoundBuffer::play() throw(SoundPlayingError)
{
    if (!(alIsSource( m_source ) == AL_TRUE))
    {
        std::stringstream stream;
        stream << "Error: " << m_name << " has no source";
        throw SoundSourceError(stream.str());
    }
    ALint buf;
    alGetSourcei(m_source, AL_BUFFER, &buf);
    if (!(alIsBuffer(buf) == AL_TRUE))
    {
        std::stringstream stream;
        stream << "Error: " << m_name << " has no buffer";
        throw SoundBufferError(stream.str());
    }

    if (alGetError() != AL_NO_ERROR)
    {
        std::stringstream stream;
        stream << "Error, cannot play " << m_name;
        throw SoundPlayingError(stream.str());
    }
    ALfloat listener_position[]= { 0.0, 0.0, 0.0 };
    ALfloat listener_velocity[] = { 0.0, 0.0, 0.0 };
    ALfloat listener_orientation[] = { 0.0, 0.0, -1.0,  0.0, 1.0, 0.0 };
    alListenerfv(AL_POSITION,    listener_position);
    alListenerfv(AL_VELOCITY,    listener_velocity);
    alListenerfv(AL_ORIENTATION, listener_orientation);
    if (alGetError() != AL_NO_ERROR)
    {
        std::stringstream stream;
        stream << "Error cannot play " << m_name <<
                ". Setup of source and listener failed";
        throw SoundPlayingError(stream.str());
    }
    alSourcePlay( m_source );
    return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void ALSoundBuffer::stop()
{
    alSourceStop( m_source );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void ALSoundBuffer::rewind()
{
    alSourceRewind( m_source );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void ALSoundBuffer::setLoop(bool yes)
{
    ALboolean loop = yes ? AL_TRUE : AL_FALSE;
    alSourcei( m_source, AL_LOOPING, loop );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool ALSoundBuffer::isPlaying() const
{
    ALint state;
    alGetSourcei( m_source, AL_SOURCE_STATE, &state );
    return (state == AL_PLAYING);
}
