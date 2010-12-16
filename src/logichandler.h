// Copyright (C) 2008,2009,2010 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU General Public License version 2 or later
// as published by the Free Software Foundation, Inc.

#ifndef logichandler_h
#define logichandler_h

#include <vector>
#include <string>

namespace PPL {

/**
  * abstract class for all kinds of processing that takes values from XP,
  * do calculations in the flightloop and publish the result in a custom Dataref
  * @author (c) 2009, 2010 by Philipp Muenzel
  * @version 0.3
  * @file logichandler.h
  */
class LogicHandler {

 public:


    virtual ~LogicHandler() {}

    /**
     * reimplement to do the setups that have to be done once when data is acessible
     */
    virtual bool initializeAtStart() = 0;


    /**
      * reimplement this function to re-initialize when a new user aircraft is loaded
      */
    virtual bool initializeOnAircraftChange() = 0;


    /**
     * reimplement to do the calculations that are done periodically (via the flightloop)
     */
    virtual bool processInTheLoop() = 0;


    /**
     * reimplement to process a signal from outside (e.g. interplugin message)
     */
    virtual bool processInput(long input, void* param) = 0;


    /**
     * reimplement to return the value for the next call of your processing- positive for seconds, negative for ticks
     */
    virtual float loopFrequency() = 0;


    /**
      * reimplement to suspend all processing by this handler if asked
      */
    virtual void suspend(bool yes) = 0;


    /**
     * reimplement to tell if this handler is currenty suspended
     */
    virtual bool isSuspended() const= 0;


    /**
      * name of this handler
      */
    virtual std::string name() const= 0;

    /**
      * enable processing for this handler and register in X-Planes flight-loop
      */
    void hookToSim();

    /**
      * stop processing and unregister from X-Planes flight-loop
      */
    void unhookFromSim();

private:

    /**
     * reimplement to register for the data needed for calculation
     */
    virtual bool aquireSimData() = 0;


    /**
     * reimplement to register the acessors to the owned data to form a custom dataref(s)
     */
    virtual bool publishData() = 0;


    /**
     * reimplement to unregister the custom dataref(s)
     */
    virtual bool withdrawPublishedData() = 0;

};

}

#endif
