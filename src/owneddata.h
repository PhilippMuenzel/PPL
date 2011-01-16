// Copyright (C) 2008-2011 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, Inc.

#ifndef OWNEDDATA_H
#define OWNEDDATA_H

#include <string>
#include <vector>
#include <stdexcept>

#include "XPLMDataAccess.h"

#include "dataref.h"

namespace PPL {

template <typename T>
T readFunc(void*);
template <typename T>
void writeFunc(void*, T);

long readFuncStr(void*, void*, int, long);
void writeFuncStr(void*, void*, int, long);

long readFuncVF(void*, float*, int, int);
void writeFuncVF(void*, float*, int, int);

class DataRefNotPublishedException : public std::runtime_error {
public:
    DataRefNotPublishedException(const std::string& msg):
        std::runtime_error(msg)
    {}
};


/**
  * Data owned by the plugin and published as a XPLM DataRef.
  * create an instance and call a register function to make data accessible to other plugins
  * @author (c) 2009-2011 by Philipp Muenzel
  * @version 0.6
  */
template <typename T>
class OwnedData{
public:

    /**
      * create owned date for sharing.
      * the identifier provided is the string identifier later used for the dataref.
      * the dataref is then registered in X-Plane's plugin system
      * Please consult the XP SDK naming conventions on how to name things right.
      * @param std::string& identifier
      * @param RWType set the dataref writeable to OTHER plugins
      * @exception throws DataRefNotPublishedException if publishing in X-Plane fails
      */
    OwnedData(const std::string& identifier,
              RWType read_write = ReadOnly):
        m_data_ref_identifier(identifier),
        m_is_registered(false),
        m_data_ref(0),
        m_value(T())
    {
        switch(read_write)
        {
        case ReadOnly :
            registerRead();
            break;
        case WriteOnly :
            registerWrite();
            break;
        case ReadWrite:
            registerReadWrite();
            break;
        }
    }

    /**
      * upon destruction, the dataref is de-registered from X-Plane
      */
    ~OwnedData() throw () { unregister(); }

    /**
      * check if setup process was correctly completed
      * @return TRUE if the dataref registered and accessible
      */
    bool isRegistered() const { return m_is_registered; }

    /**
      * acces the currently stored value
      * @return value
      */
    T value() const { return m_value; }

    /**
      * set the value so all other monitors of the dataref get it
      * @param value
      */
    void setValue(const T& val) { m_value = val; }


private:

    void registerRead();

    void registerWrite();

    void registerReadWrite();

    void unregister()
    {
        if (m_is_registered && m_data_ref != 0)
        {
            XPLMUnregisterDataAccessor(m_data_ref);
            m_data_ref = 0;
        }
        m_is_registered = false;
    }

private:

    std::string m_data_ref_identifier;
    bool m_is_registered;
    XPLMDataRef m_data_ref;
    T m_value;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template <typename T>
T readFunc(void* inRefCon)
{
    OwnedData<T>* p_owned_data = static_cast<OwnedData<T>*>(inRefCon);
    return p_owned_data->value();
}

template <typename T>
void writeFunc(void* inRefCon, T data)
{
    OwnedData<T>* p_owned_data = static_cast<OwnedData<T>*>(inRefCon);
    p_owned_data->setValue(data);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template <>
void OwnedData<int>::registerRead();
template <>
void OwnedData<float>::registerRead();
template <>
void OwnedData<double>::registerRead();
template <>
void OwnedData<std::string>::registerRead();
template <>
void OwnedData<std::vector<float> >::registerRead();

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template <>
void OwnedData<int>::registerWrite();
template <>
void OwnedData<float>::registerWrite();
template <>
void OwnedData<double>::registerWrite();
template <>
void OwnedData<std::string>::registerWrite();
template <>
void OwnedData<std::vector<float> >::registerWrite();

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template <>
void OwnedData<int>::registerReadWrite();
template <>
void OwnedData<float>::registerReadWrite();
template <>
void OwnedData<double>::registerReadWrite();
template <>
void OwnedData<std::string>::registerReadWrite();
template <>
void OwnedData<std::vector<float> >::registerReadWrite();

}

#endif // OWNEDDATA_H
