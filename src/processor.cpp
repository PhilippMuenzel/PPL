// Copyright (C) 2008-2011 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, Inc.

#include "processor.h"
#include "XPLMProcessing.h"

using namespace PPL;

Processor::Processor(float time_to_start)
{
    XPLMRegisterFlightLoopCallback(Processor::flightloopcallback, time_to_start, this);
}

Processor::~Processor()
{
    XPLMUnregisterFlightLoopCallback(Processor::flightloopcallback, this);
}

float Processor::flightloopcallback(float inElapsedSinceLastCall,
                                    float inElapsedTimeSinceLastFlightLoop,
                                    int inCounter,
                                    void *inRefcon)
{
    Processor* proc = static_cast<Processor*>(inRefcon);
    return proc->callback(inElapsedSinceLastCall, inElapsedTimeSinceLastFlightLoop, inCounter);
}

