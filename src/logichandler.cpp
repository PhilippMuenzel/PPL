// Copyright (C) 2008,2009,2010 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU General Public License version 2 or later
// as published by the Free Software Foundation, Inc.

#include <iostream>

#include "XPLMProcessing.h"

#include "logichandler.h"
#include "log.h"

using namespace PPL;

float HandlerCallbackInit(float, float, int, void* inRefCon)
{
    LogicHandler* handler = static_cast<LogicHandler*>(inRefCon);
    handler->initializeAtStart();
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

float HandlerCallbackProcess(float, float, int, void* inRefCon)
{
    LogicHandler* handler = static_cast<LogicHandler*>(inRefCon);
    if (!handler->isSuspended())
        handler->processInTheLoop();
    return handler->loopFrequency();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


void LogicHandler::hookToSim()
{
    if (!this->aquireSimData())
    {
        Log() << Log::Error << "Registering data refs failed for " << this->name() << Log::endl;
    }
    if (!this->publishData())
    {
        Log() << Log::Error << "Publishing Data failed for " << this->name() << Log::endl;
    }
    this->suspend(false);
    XPLMRegisterFlightLoopCallback(HandlerCallbackInit,-1,this);
    XPLMRegisterFlightLoopCallback(HandlerCallbackProcess,-3,this);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void LogicHandler::unhookFromSim()
{
    XPLMUnregisterFlightLoopCallback(HandlerCallbackInit, this);
    XPLMUnregisterFlightLoopCallback(HandlerCallbackProcess, this);
    this->withdrawPublishedData();
}
