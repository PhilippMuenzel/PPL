// Copyright (C) 2008-2011 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, Inc.

#ifndef PROCESSOR_H
#define PROCESSOR_H

namespace PPL {

class Processor
{
public:
    Processor(float time_to_start = -1.f);
    virtual ~Processor();
    virtual float callback(float, float, int) = 0;
private:
    static float flightloopcallback(float, float, int, void* refcon);
};

}

#endif // PROCESSOR_H
