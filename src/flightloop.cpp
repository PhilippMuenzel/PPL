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