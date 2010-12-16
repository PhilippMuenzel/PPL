// Copyright (C) 2008,2009,2010 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU General Public License version 2 or later
// as published by the Free Software Foundation, Inc.

#ifndef ALCONTEXTCHANGER_H
#define ALCONTEXTCHANGER_H

#include <AL/al.h>
#include <AL/alc.h>

#include <boost/noncopyable.hpp>

namespace XSPL{
    namespace Sound {

        /**
          * RAII class to change the openal context on construction and
          * ensure the old context is restored on destruction.
          * @author (c) 2009, 2010 by Philipp Muenzel, Technische Universitaet Darmstadt, Department of Mathematics
          * @version 0.3
          * @file alcontextchanger.h
        */
        class ALContextChanger : boost::noncopyable
        {
        public:
            ALContextChanger(ALCcontext* own_context) throw();
            ~ALContextChanger() throw();

        private:
            ALCcontext* m_other_context;
        };

    }
}

#endif // ALCONTEXTCHANGER_H
