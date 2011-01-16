// Copyright (C) 2008-2011 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, Inc.

#include <sstream>

#include "alcontextmanager.h"
#include "alcontextchanger.h"

using namespace PPL;

ALContextManager::ALContextManager():
        m_internal_counter(0)
{
    m_device = alcOpenDevice(0);
    m_my_context = alcCreateContext(m_device, 0);
    // Note that owned context is not set active
    alGetError();
    alutInitWithoutContext(NULL, 0);
    alutGetError();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ALContextManager::~ALContextManager()
{
    deleteAllSounds();
    alutExit();
    m_current_context = alcGetCurrentContext();
    if (m_current_context == m_my_context)
        alcMakeContextCurrent(NULL);
    alcDestroyContext(m_my_context);
    alcCloseDevice(m_device);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ALSoundBuffer* ALContextManager::findSoundById(int id) throw(SoundNotFoundError)
{
    std::map<int, ALSoundBuffer*>::iterator it;
    it = m_sounds.find(id);
    if (it != m_sounds.end())
    {
        return it->second;
    } else
    {
        std::stringstream stream;
        stream << "Sound number " << id << " nonexistant.";
        throw SoundNotFoundError(stream.str());
    }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int ALContextManager::addSoundFromFile(const std::string& filename) throw(SoundLoadError)
{
    std::pair<std::map<int, ALSoundBuffer*>::iterator, bool> return_value;
    m_internal_counter++;
    ALContextChanger cc(m_my_context);
    // object cc is not used, but it changes context, and since cc is on the stack,
    // it is destroyed when function exits, which changes back the contexts!
    // this is known as the RAII idiom
    try
    {
        return_value = m_sounds.insert( std::pair<int, ALSoundBuffer*>( m_internal_counter,
                                                                        new ALSoundBuffer(filename)
                                                                        ));
    } catch (ALSoundBuffer::SoundPlayingError& ex)
    {
        std::stringstream stream;
        stream << "Failure inserting soundfile " << filename << " AL error: "
                << ex.what();
        throw SoundLoadError(stream.str());
    }
    if (return_value.second == false) {
        std::stringstream stream;
        stream << "Failure inserting soundfile " << filename;
        throw SoundLoadError(stream.str());
    }
    return m_internal_counter;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool ALContextManager::playSound(int id) throw (SoundNotFoundError, SoundPlayError)
{
    try {
        ALSoundBuffer* sound = findSoundById(id);
        try {
            ALContextChanger cc(m_my_context);
            sound->play();
            return true;
        } catch(ALSoundBuffer::SoundPlayingError& ex)
        {
            std::stringstream stream;
            stream << "Sound number " << id << " failed to play with message: "
                    << ex.what();
            throw SoundPlayError(stream.str());
        }
    } catch (SoundNotFoundError&)
    {
        throw;
    }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void ALContextManager::stopSound(int id) throw (SoundNotFoundError)
{
    try{
        ALSoundBuffer* sound = findSoundById(id);
        ALContextChanger cc(m_my_context);
        sound->stop();
    } catch(SoundNotFoundError&)
    {
        throw;
    }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void ALContextManager::rewindSound(int id) throw (SoundNotFoundError)
{
    try {
        ALSoundBuffer* sound = findSoundById(id);
        ALContextChanger cc(m_my_context);
        sound->rewind();
    } catch (SoundNotFoundError&) {
        throw;
    }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void ALContextManager::loopSound(int id) throw (SoundNotFoundError)
{
    try {
        ALSoundBuffer* sound = findSoundById(id);
        ALContextChanger cc(m_my_context);
        sound->setLoop(true);
    } catch (SoundNotFoundError&) {
        throw;
    }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void ALContextManager::unLoopSound(int id) throw (SoundNotFoundError)
{
    try
    {
        ALSoundBuffer* sound = findSoundById(id);
        ALContextChanger cc(m_my_context);
        sound->setLoop(false);
    } catch (SoundNotFoundError&)
    {
        throw;
    }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void ALContextManager::removeSound(int id) throw (SoundNotFoundError)
{
    std::map<int, ALSoundBuffer*>::iterator it;
    it = m_sounds.find(id);
    if (it != m_sounds.end())
    {
        ALContextChanger cc(m_my_context);
        delete m_sounds.at(id);
        m_sounds.erase(it);
    } else
    {
        std::stringstream stream;
        stream << "Sound number " << id << " nonexistant.";
        throw SoundNotFoundError(stream.str());
    }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void ALContextManager::deleteAllSounds()
{
    for (std::map<int, ALSoundBuffer*>::iterator it = m_sounds.begin (); it!= m_sounds.end() ; it++)
    {
        int id = it->first;
        delete m_sounds.at(id);
        m_sounds.erase ( it );
    }
}
