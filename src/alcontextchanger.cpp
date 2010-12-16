// Copyright (C) 2008,2009,2010 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU General Public License version 2 or later
// as published by the Free Software Foundation, Inc.

#include "alcontextchanger.h"

using namespace XSPL::Sound;

ALContextChanger::ALContextChanger(ALCcontext* own_context) throw()
{
    m_other_context = alcGetCurrentContext();
    alcMakeContextCurrent(own_context);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ALContextChanger::~ALContextChanger() throw()
{
    alcMakeContextCurrent( m_other_context );
}
