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

