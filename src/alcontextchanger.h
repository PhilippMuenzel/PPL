// Copyright (C) 2008-2011 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, Inc.

#ifndef ALCONTEXTCHANGER_H
#define ALCONTEXTCHANGER_H

#include <AL/al.h>
#include <AL/alc.h>

#include <boost/noncopyable.hpp>

namespace PPL {

/**
  * @brief RAII class to change the openal context on construction and
  * ensure the old context is restored on destruction.
  *
  * @author (c) 2009-2011 by Philipp Muenzel, Technische Universitaet Darmstadt, Department of Mathematics
  * @version 0.4
  * @file alcontextchanger.h
  */
class ALContextChanger : boost::noncopyable
{
public:

    /**
      * Switch to the openAL context given for the time this object lives.
      *
      * @param own_context the AL context to switch to
      */
    ALContextChanger(ALCcontext* own_context);

    /**
      * switch back to whatever context was active at the time the object was created.
      */
    ~ALContextChanger();

private:
    ALCcontext* m_other_context;
};

}

#endif // ALCONTEXTCHANGER_H
