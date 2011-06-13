// Copyright (C) 2008-2011 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, Inc.

#ifndef BASICS_H
#define BASICS_H

#if (defined(_WIN32) || defined(__CYGWIN__)) && defined(_MSC_VER)   // Windows && MS Visual C
#    define MSVC_EXPORT __declspec(dllexport)
#    pragma warning( disable : 4996 )   // disable deprecation warnings
#    pragma warning( disable : 4091 )   // disable typedef warning without variable declaration
#    pragma warning( disable : 4275 )   // non &#8211; DLL-interface classkey 'identifier' used as base for DLL-interface classkey 'identifier'
#    pragma warning( disable : 4251 )   // like warning above but for templates (like std::string)
#    pragma warning( disable : 4290 )   // exception handling of MSVC is lousy
#else
#    define MSVC_EXPORT
#endif

#if defined(_MSC_VER)
#    include <float.h>
#    define isnan _isnan
#endif
#include <cmath>

namespace PPL {

template <typename T>
T round(T r)
{
    return (r > 0.0) ? std::floor(r + 0.5) : std::ceil(r - 0.5);
}

template <typename T>
T min(const T& a, const T& b)
{
    return (a<b)?a:b;
}

template <typename T>
T max(const T& a, const T& b)
{
    return (a<b)?b:a;
}

inline long ipow(long base, long exp)
{
    int result = 1;
    while (exp)
    {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        base *= base;
    }
    return result;
}

inline double mods(double y, double x)
{
    return y - x*std::floor(y/x);
}

}

#endif // BASICS_H
