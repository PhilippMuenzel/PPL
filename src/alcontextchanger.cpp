// Copyright (C) 2008-2011 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, Inc.

#include "alcontextchanger.h"

using namespace PPL;

ALContextChanger::ALContextChanger(ALCcontext* own_context)
{
    m_other_context = alcGetCurrentContext();
    alcMakeContextCurrent(own_context);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ALContextChanger::~ALContextChanger()
{
    alcMakeContextCurrent( m_other_context );
}
