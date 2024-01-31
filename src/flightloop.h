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

#ifndef PPL_FLIGHTLOOP_H
#define PPL_FLIGHTLOOP_H

#include "XPLMProcessing.h"
#include <concepts>

namespace PPL {

/**
 * @brief Phase to run a @ref PPL::FlightLoop in.
 */
enum class FlightLoopPhase {
    /**
     * @brief Run the @ref PPL::FlightLoop before the flight model.
     */
    BeforeFlightModel,
    /**
     * @brief Run the @red PPL::FlightLoop after the flight model.
     */
    AfterFlightModel
};

/**
 * @brief Abstract class to be inherited by flight loop handlers.
 */
class FlightLoopHandler {
public:
    FlightLoopHandler();
    virtual ~FlightLoopHandler() = 0;
    /**
     * @brief Handler function for the flight loop.
     *
     * @param time_since_last_call Time in seconds since this loop was last called.
     * @param time_since_last_loop Time in seconds since any flight loop was last called.
     * @param counter A counter that the simulator increases each time it dispatches a flight loop.
     * @return float When to next run this function:
     *               Positive values: N seconds.
     *               Negative values (must be integer): N simulation frames.
     *               Zero: Deactivate this callback. It will remain registered.
     */
    virtual float do_flight_loop(float time_since_last_call, float time_since_last_loop, int counter) = 0;
};

template <typename T>
concept impl_FlightLoopHandler = std::derived_from<T, FlightLoopHandler> && (!std::same_as<T, FlightLoopHandler>);

/**
 * @brief A flight loop object.
 *
 * @tparam T The handler type for the flight loop. Must implement @ref PPL::FlightLoopHandler.
 */
template <typename T>
    requires impl_FlightLoopHandler<T>
class FlightLoop {
public:
    /**
     * @brief Construct a new flight loop object.
     *
     * @param phase The phase in which to execute this flight loop.
     * @param handler The handler for the flight loop.
     */
    FlightLoop(FlightLoopPhase phase, T handler);
    /**
     * @brief Schedule this flight loop.
     *
     * @param when When to run this flight loop next. A positive value indicates seconds.
     *             A negative value (must be an integer) indicates number of simulation frames.
     * @param relative_to_now Interpret `when` as relative to now. If `false`, it will be interpreted as
     *                        relative to the time the callback was last called (or when it was registered, if never called).
     */
    void schedule(float when, bool relative_to_now);
    ~FlightLoop();

private:
    static float do_flight_loop(float time_since_last_call, float time_since_last_loop, int counter, void* refcon);
    XPLMFlightLoopID loop_id;
    T handler;
};

} // namespace PPL

#endif // PPL_FLIGHTLOOP_H