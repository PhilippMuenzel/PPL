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
