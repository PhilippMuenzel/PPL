// Copyright (C) 2008,2009,2010 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU General Public License version 2 or later
// as published by the Free Software Foundation, Inc.

#include <cstring>

#include "owneddata.h"

using namespace PPL;

template <>
void OwnedData<int>::registerRead()
{
    m_data_ref = XPLMRegisterDataAccessor( m_data_ref_identifier.c_str(), xplmType_Int, 0,
                                                 readFunc<int>, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                                 NULL, NULL, NULL, NULL, this, NULL );
    if (m_data_ref == 0)
        throw DataRefNotPublishedException("");
    else
        m_is_registered = true;
}

template <>
void OwnedData<float>::registerRead()
{
    m_data_ref = XPLMRegisterDataAccessor( m_data_ref_identifier.c_str(), xplmType_Float, 0,
                                                 NULL, NULL, readFunc<float>, NULL, NULL, NULL, NULL, NULL,
                                                 NULL, NULL, NULL, NULL, this, NULL );
    if (m_data_ref == 0)
        throw DataRefNotPublishedException("");
    else
        m_is_registered = true;
}

template <>
void OwnedData<double>::registerRead()
{
    m_data_ref = XPLMRegisterDataAccessor( m_data_ref_identifier.c_str(), xplmType_Double, 0,
                                                 NULL, NULL, NULL, NULL, readFunc<double>, NULL, NULL, NULL,
                                                 NULL, NULL, NULL, NULL, this, NULL);
    if (m_data_ref == 0)
        throw DataRefNotPublishedException("");
    else
        m_is_registered = true;
}

template <>
void OwnedData<std::string>::registerRead()
{
    m_data_ref = XPLMRegisterDataAccessor( m_data_ref_identifier.c_str(), xplmType_Data, 0,
                                                 NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                                 NULL, NULL, readFuncStr, NULL, this, NULL );
    if (m_data_ref == 0)
        throw DataRefNotPublishedException("");
    else
        m_is_registered = true;
}

template <>
void OwnedData<std::vector<float> >::registerRead()
{
    m_data_ref = XPLMRegisterDataAccessor( m_data_ref_identifier.c_str(), xplmType_FloatArray, 0,
                                                 NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                                 readFuncVF, NULL, NULL, NULL, this, NULL );
    if (m_data_ref == 0)
        throw DataRefNotPublishedException("");
    else
        m_is_registered = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


template <>
void OwnedData<int>::registerWrite()
{
    m_data_ref = XPLMRegisterDataAccessor( m_data_ref_identifier.c_str(), xplmType_Int, 1,
                                             NULL, writeFunc<int>, NULL, NULL, NULL, NULL, NULL, NULL,
                                             NULL, NULL, NULL, NULL, NULL, this );
    if (m_data_ref == 0)
        throw DataRefNotPublishedException("");
    else
        m_is_registered = true;
}

template <>
void OwnedData<float>::registerWrite()
{
    m_data_ref = XPLMRegisterDataAccessor( m_data_ref_identifier.c_str(), xplmType_Float, 1,
                                             NULL, NULL, NULL, writeFunc<float>, NULL, NULL, NULL, NULL,
                                             NULL, NULL, NULL, NULL, NULL, this );
    if (m_data_ref == 0)
        throw DataRefNotPublishedException("");
    else
        m_is_registered = true;
}

template <>
void OwnedData<double>::registerWrite()
{
    m_data_ref = XPLMRegisterDataAccessor( m_data_ref_identifier.c_str(), xplmType_Double, 1,
                                                 NULL, NULL, NULL, NULL, NULL, writeFunc<double>, NULL, NULL,
                                                 NULL, NULL, NULL, NULL, NULL, this );
    if (m_data_ref == 0)
        throw DataRefNotPublishedException("");
    else
        m_is_registered = true;
}

template <>
void OwnedData<std::string>::registerWrite()
{
    m_data_ref = XPLMRegisterDataAccessor( m_data_ref_identifier.c_str(), xplmType_Data, 1,
                                                 NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                                 NULL, NULL, NULL, writeFuncStr, NULL, this );
    if (m_data_ref == 0)
        throw DataRefNotPublishedException("");
    else
        m_is_registered = true;
}

template <>
void OwnedData<std::vector<float> >::registerWrite()
{
    m_data_ref = XPLMRegisterDataAccessor( m_data_ref_identifier.c_str(), xplmType_FloatArray, 1,
                                                 NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                                 NULL, writeFuncVF, NULL, NULL, NULL, this );
    if (m_data_ref == 0)
        throw DataRefNotPublishedException("");
    else
        m_is_registered = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


template <>
void OwnedData<int>::registerReadWrite()
{
    m_data_ref = XPLMRegisterDataAccessor( m_data_ref_identifier.c_str(), xplmType_Int, 1,
                                                 readFunc<int>, writeFunc<int>, NULL, NULL, NULL, NULL, NULL, NULL,
                                                 NULL, NULL, NULL, NULL, this, this );
    if (m_data_ref == 0)
        throw DataRefNotPublishedException("");
    else
        m_is_registered = true;
}

template <>
void OwnedData<float>::registerReadWrite()
{
    m_data_ref = XPLMRegisterDataAccessor( m_data_ref_identifier.c_str(), xplmType_Float, 1,
                                                 NULL, NULL, readFunc<float>, writeFunc<float>, NULL, NULL, NULL, NULL,
                                                 NULL, NULL, NULL, NULL, this, this );
    if (m_data_ref == 0)
        throw DataRefNotPublishedException("");
    else
        m_is_registered = true;
}

template <>
void OwnedData<double>::registerReadWrite()
{
    m_data_ref = XPLMRegisterDataAccessor( m_data_ref_identifier.c_str(), xplmType_Double, 1,
                                                 NULL, NULL, NULL, NULL, readFunc<double>, writeFunc<double>, NULL, NULL,
                                                 NULL, NULL, NULL, NULL, this, this );
    if (m_data_ref == 0)
        throw DataRefNotPublishedException("");
    else
        m_is_registered = true;
}

template <>
void OwnedData<std::string>::registerReadWrite()
{
    m_data_ref = XPLMRegisterDataAccessor( m_data_ref_identifier.c_str(), xplmType_Data, 1,
                                                 NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                                 NULL, NULL, readFuncStr, writeFuncStr, this, this );
    if (m_data_ref == 0)
        throw DataRefNotPublishedException("");
    else
        m_is_registered = true;
}

template <>
void OwnedData<std::vector<float> >::registerReadWrite()
{
    m_data_ref = XPLMRegisterDataAccessor( m_data_ref_identifier.c_str(), xplmType_FloatArray, 1,
                                                 NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                                 readFuncVF, writeFuncVF, NULL, NULL, this, this );
    if (m_data_ref == 0)
        throw DataRefNotPublishedException("");
    else
        m_is_registered = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


long PPL::readFuncStr(void* inRefCon, void* outValue, int inOffset, long inMaxLength)
{
    OwnedData<std::string>* p_owned_data = static_cast<OwnedData<std::string>*>(inRefCon);
    long length = p_owned_data->value().length();
    if (outValue == NULL)
        return length;
    long maxlen = (inMaxLength < length)?inMaxLength:length;
    strcpy(static_cast<char*>(outValue), p_owned_data->value().substr(inOffset, maxlen).c_str());
    return inMaxLength;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void PPL::writeFuncStr(void* inRefCon, void* inValue, int inOffset, long inMaxLength)
{
    OwnedData<std::string>* p_owned_data = static_cast<OwnedData<std::string>*>(inRefCon);
    char* str = static_cast<char*>(inValue);
    p_owned_data->setValue(std::string(str).substr(inOffset,inMaxLength));
}

long PPL::readFuncVF(void* inRefCon, float* outValues, int inOffset, int inMaxLength)
{
    OwnedData<std::vector<float> >* p_owned_data = static_cast<OwnedData<std::vector<float> >*>(inRefCon);
    if (outValues == NULL)
        return p_owned_data->value().size();
    int end = (p_owned_data->value().size() < static_cast<unsigned int>(inMaxLength))?(p_owned_data->value().size()):inMaxLength;
    memcpy(outValues, &p_owned_data->value()[inOffset], sizeof(float)*end);
    return end;
}

void PPL::writeFuncVF(void* inRefCon, float* inValues, int inOffset, int inMaxLength)
{
    OwnedData<std::vector<float> >* p_owned_data = static_cast<OwnedData<std::vector<float> >*>(inRefCon);
    p_owned_data->setValue(std::vector<float>(inValues+inOffset, inValues+inOffset+inMaxLength));
}
