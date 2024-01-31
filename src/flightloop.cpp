// Copyright (c) 2023 Julia DeMille
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors
// may be used to endorse or promote products derived from this software without
// specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS”
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include "flightloop.h"
#include "XPLMProcessing.h"

namespace PPL {

template <typename T>
    requires impl_FlightLoopHandler<T>
FlightLoop<T>::FlightLoop(FlightLoopPhase phase, T handler_)
    : handler(handler_)
{
    XPLMFlightLoopPhaseType x_phase;
    switch (phase) {
    case FlightLoopPhase::BeforeFlightModel:
        x_phase = xplm_FlightLoop_Phase_BeforeFlightModel;
        break;
    case FlightLoopPhase::AfterFlightModel:
        x_phase = xplm_FlightLoop_Phase_AfterFlightModel;
        break;
    }
    XPLMCreateFlightLoop_t create_fl = {
        sizeof(XPLMCreateFlightLoop_t),
        x_phase,
        FlightLoop<T>::do_flight_loop,
        &handler
    };
    loop_id = XPLMCreateFlightLoop(&create_fl);
}

template <typename T>
    requires impl_FlightLoopHandler<T>
void FlightLoop<T>::schedule(float when, bool relative_to_now)
{
    XPLMScheduleFlightLoop(loop_id, when, relative_to_now);
}

template <typename T>
    requires impl_FlightLoopHandler<T>
FlightLoop<T>::~FlightLoop<T>()
{
    XPLMDestroyFlightLoop(loop_id);
}

template <typename T>
    requires impl_FlightLoopHandler<T>
float FlightLoop<T>::do_flight_loop(float time_since_last_call, float time_since_last_loop, int counter, void* refcon)
{
    T* handler = static_cast<T*>(refcon);
    handler->do_flight_loop(time_since_last_call, time_since_last_loop, counter);
}

}