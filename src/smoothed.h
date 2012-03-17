#ifndef SMOOTHED_H
#define SMOOTHED_H

#include <cstring>
#include "namespaces.h"

namespace PPLNAMESPACE {

template <typename T, int n>
class Smoothed
{
public:
    Smoothed()
    {
        memset(m_history, 0, sizeof(T)*n);
    }

    Smoothed(T val)
    {
        for (std::size_t i = 0 ; i < n ; i++)
            m_history[i] = val;
    }

    const Smoothed& operator=(T val)
    {
        memmove(&m_history[1],m_history,(n-1)*sizeof(T));
        m_history[0] = val;
        return *this;
    }

    operator T() const
    {
        T result(0);
        for (std::size_t i = 0 ; i < n ; i++)
            result += m_history[i]/n;
        return result;
    }
private:
    T m_history[n];
};
}
#endif // SMOOTHED_H
