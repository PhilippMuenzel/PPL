// Copyright (C) 2008-2011 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, Inc.

#ifndef OWNEDDATA_H
#define OWNEDDATA_H

#include <string>
#include <vector>
#include <stdexcept>

#include "XPLMDataAccess.h"
#include "XPLMPlugin.h"

#include "dataref.h"
#include "namespaces.h"

namespace PPLNAMESPACE {

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

const long DRE_MSG_ADD_DATAREF = 0x01000000;
const char* const DRE_PLUGIN_SINATURE = "xplanesdk.examples.DataRefEditor";


/**
  * Data owned by the plugin and published as a XPLM DataRef.
  * create an instance and call a register function to make data accessible to other plugins
  * @author (c) 2009-2011 by Philipp Muenzel
  * @version 0.6
  */
template <typename T>
class OwnedData{
public:

    typedef void (*DataCallback_f)(const T&);

    /**
      * create owned date for sharing.
      * the identifier provided is the string identifier later used for the dataref.
      * the dataref is then registered in X-Plane's plugin system
      * Please consult the XP SDK naming conventions on how to name things right.
      * @param std::string& identifier
      * @param RWType set the dataref writeable to OTHER plugins
      * @param publish_in_dre by default, a message is sent to the DataRefEditor plugin that exposes the dataref
      * @param callback optional callback function pointer to be called when other plugins write to the dataref
      * @exception throws DataRefNotPublishedException if publishing in X-Plane fails
      */
    OwnedData(const std::string& identifier,
              RWType read_write = ReadOnly,
              bool publish_in_dre = true,
              DataCallback_f callback = 0):
        m_data_ref_identifier(identifier),
        m_data_ref(0),
        m_value(T()),
        m_callback(callback)
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
        if (publish_in_dre)
        {
            XPLMPluginID PluginID = XPLMFindPluginBySignature(DRE_PLUGIN_SINATURE);
            if (PluginID != XPLM_NO_PLUGIN_ID)
                XPLMSendMessageToPlugin(PluginID, DRE_MSG_ADD_DATAREF, (void*)m_data_ref_identifier.c_str());
        }
    }

    /**
      * upon destruction, the dataref is de-registered from X-Plane
      */
    ~OwnedData() { unregister(); }

    /**
      * acces the currently stored value
      * @return value
      */
    T value() const { return m_value; }

    /**
      * acces the currently stored value
      * @return value
      */
    operator T() const { return m_value; }

    /**
      * set the value so all other monitors of the dataref get it
      * @param val
      */
    const OwnedData& operator=(const T& val) { m_value = val; return *this; }

    /**
      * set the value so all other monitors of the dataref get it
      * @param val
      */
    void setValue(const T& val)
    {
        m_value = val;
        if (m_callback)
            m_callback(val);
    }


private:

    void registerRead();

    void registerWrite();

    void registerReadWrite();

    void unregister()
    {
        if (m_data_ref)
        {
            XPLMUnregisterDataAccessor(m_data_ref);
            m_data_ref = 0;
        }
    }

private:

    std::string m_data_ref_identifier;
    XPLMDataRef m_data_ref;
    T m_value;
    DataCallback_f m_callback;
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
