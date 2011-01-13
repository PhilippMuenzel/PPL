// Copyright (C) 2008-2011 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU General Public License version 2 or later
// as published by the Free Software Foundation, Inc.

#include <cstring>

#include "dataref.h"

using namespace PPL;

template <>
void DataRef<int>::checkDataType()
{
    if ( XPLMGetDataRefTypes(m_data_ref) != xplmType_Int )
        throw IncompatibleTypeException("Declared to be int, but isn't.");
}

template <>
void DataRef<float>::checkDataType()
{
    if ( XPLMGetDataRefTypes(m_data_ref) != xplmType_Float )
        throw IncompatibleTypeException("Declared to be float, but isn't.");
}

template <>
void DataRef<double>::checkDataType()
{
    if ( XPLMGetDataRefTypes(m_data_ref) != (xplmType_Float | xplmType_Double) )
        throw IncompatibleTypeException("Declared to be double, but isn't.");
}

template <>
void DataRef<std::vector<float> >::checkDataType()
{
    if (XPLMGetDataRefTypes(m_data_ref) != xplmType_FloatArray)
        throw IncompatibleTypeException("Declared to be a float array, but isn't.");
}

template <>
void DataRef<std::vector<int> >::checkDataType()
{
    if (XPLMGetDataRefTypes(m_data_ref) != xplmType_IntArray)
        throw IncompatibleTypeException("Declared to be a int array, but isn't.");
}

template <>
void DataRef<std::string>::checkDataType()
{
    if (XPLMGetDataRefTypes(m_data_ref) != xplmType_Data)
        throw IncompatibleTypeException("Declared to be a byte array, but isn't.");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template <>
DataRef<int>::operator int() const
{
    return XPLMGetDatai(m_data_ref);
}

template <>
DataRef<float>::operator float() const
{
    return XPLMGetDataf(m_data_ref);
}

template <>
DataRef<double>::operator double() const
{
    return XPLMGetDatad(m_data_ref);
}

template <>
DataRef<std::vector<float> >::operator std::vector<float>() const
{
    std::vector<float> values(XPLMGetDatavf(m_data_ref, NULL, 0, 0));
    XPLMGetDatavf(m_data_ref, &values[0], 0, values.size());
    return values;
}

template <>
DataRef<std::vector<int> >::operator std::vector<int>() const
{
    std::vector<int> values(XPLMGetDatavi(m_data_ref, NULL, 0, 0));
    XPLMGetDatavi(m_data_ref, &values[0], 0, values.size());
    return values;
}

template <>
DataRef<std::string>::operator std::string() const
{
    long n = XPLMGetDatab(m_data_ref, NULL, 0, 0);
    std::vector<char> out_string(n);
    XPLMGetDatab(m_data_ref, &out_string[0], 0, n);
    return std::string(out_string.begin(), out_string.end());
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


template <>
const DataRef<int>& DataRef<int>::operator=(const int& value)
{
    if (m_read_write != ReadOnly)
        XPLMSetDatai(m_data_ref, value);
    return *this;
}

template <>
const DataRef<float>& DataRef<float>::operator=(const float& value)
{
    if (m_read_write != ReadOnly)
        XPLMSetDataf(m_data_ref, value);
    return *this;
}

template <>
const DataRef<double>& DataRef<double>::operator=(const double& value)
{
    if (m_read_write != ReadOnly)
        XPLMSetDatad(m_data_ref, value);
    return *this;
}

template <>
const DataRef<std::vector<float> >& DataRef<std::vector<float> >::operator=(const std::vector<float>& value)
{
    if (m_read_write != ReadOnly)
        XPLMSetDatavf(m_data_ref, const_cast<float*>(&value[0]), 0, value.size());
    return *this;
}

template <>
const DataRef<std::vector<int> >& DataRef<std::vector<int> >::operator=(const std::vector<int>& value)
{
    if (m_read_write != ReadOnly)
        XPLMSetDatavi(m_data_ref, const_cast<int*>(&value[0]), 0, value.size());
    return *this;
}

template <>
const DataRef<std::string>& DataRef<std::string>::operator=(const std::string& value)
{
    if (m_read_write != ReadOnly)
        XPLMSetDatab(m_data_ref, const_cast<char*>(value.c_str()) , 0, value.size() + 1);
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template<>
typename basic_trait<std::vector<float> >::Basic DataRef<std::vector<float> >::operator[](std::size_t index) const
{
    std::vector<float> vf(*this);
    return vf[index];
}

template<>
typename basic_trait<std::vector<int> >::Basic DataRef<std::vector<int> >::operator[](std::size_t index) const
{
    std::vector<int> vi(*this);
    return vi[index];
}

