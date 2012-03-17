// Copyright (C) 2008-2011 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, Inc.

#ifndef DATAREF_H
#define DATAREF_H

#include <string>
#include <vector>
#include <stdexcept>

#include "XPLMDataAccess.h"

#include "log.h"
#include "namespaces.h"

namespace PPLNAMESPACE {

/**
  * @brief RWType distinguishes the three types of acess to datarefs.
  *
  * For simdata, it specifies if incoming data may be written to X-Plane
  * and if data should be readable.
  * For owned data, it specifies if the data is readable or writebale
  * to OTHER plugins (or anything that accesses datarefs e.g. panels).
  * @todo Should there be two enums for simdata and owned data?
  */
enum RWType {
    ReadOnly    = 1,
    WriteOnly   = 2,
    ReadWrite   = WriteOnly | ReadOnly
            };

/**
  * A generic base exception that is thrown when anything
  * with looking up an X-Plane dataref and binding it goes wrong
  */
class LookupException : public std::runtime_error {
public:
    LookupException(const std::string& msg):
            runtime_error(msg)
    {}
};

/**
  * The dataref identifier could not be found in the plugin system's table
  * of datarefs, neither X-Plane nor a plugin publishes it.
  */
class NotFoundException : public LookupException {
public:
    NotFoundException(const std::string& msg):
            LookupException(msg)
    {}
};

/**
  * The requested data type does not match the type specified in the
  * plugin system's table of datarefs
  */
class IncompatibleTypeException : public LookupException {
public:
    IncompatibleTypeException(const std::string& msg):
            LookupException(msg)
    {}
};

/**
 * Indicates that a write-acess (write-only or read/write) to a
  * read-only dataref was requested
  */
class NotWriteableException : public LookupException {
public:
    NotWriteableException(const std::string& msg):
            LookupException(msg)
    {}
};

template <typename T>
struct basic_trait {
    typedef T Basic;
};

template<>
struct basic_trait<std::vector<float> > {
    typedef float Basic;
};

template<>
struct basic_trait<std::vector<int> > {
    typedef int Basic;
};

template<>
struct basic_trait<std::string> {
    typedef char Basic;
};

/**
 * @brief Wrapper for access to datarefs published by XP itself or by other plugins.
 *
 * It wraps the lookup of datarefs, type-safe getting and setting of data, and
 * obeying to X-Planes writeability restrictions.
 * By creating a DataRef instance, it is bound to a specific dataref
 * specified in the constructor.
 * @author (c) 2009-2011 by Philipp Muenzel
 * @version 2.0
 */
template <typename SimType>
class DataRef
{
public:

    /**
      * Sets up the dataref connection.
      * looks up the datarefs and stores handler locally, also checks for correct
      * type of data (sim-type) and correct read-/writeability
      * @param identifier The identifier as in datarefs.txt as std::string
      * @param writeability the writeability as defined by RWType
      * @param share treat the dataref as a shared dataref, i.e. register the user as a shared dataref participant
      * @exception LookupException is thrown if one of the following happens
      * a) DataRef can not be found
      * b) Data type is invalid (trying to access an int DataRef through float functions
      * c) data was requested to be writeable, but X-Plane says it is read-only
      */
    DataRef(const std::string& identifier, RWType writeability = ReadOnly, bool share = false);

    /**
      * read the current value from X-Plane's plugin system
      */
    operator SimType() const;

    /**
      * write value to X-Plane
      */
    const DataRef& operator=(const SimType&);

    bool hasChanged() const;

    void save();

    void undo();

    /**
      * read the current value from X-Plane and access element in vector data
      * @note is the same as operator SimType() for non-vector data
      * @todo is there a more elegant way to do this?
      */
    typename basic_trait<SimType>::Basic operator[](std::size_t index) const;


private:

    void shareDataRef(const std::string& identifier);

    void lookUp(const std::string& identifier);

    void checkWriteabilityIsValid();

    void checkDataType();

    void logErrorWithDataRef(const std::string& error_msg, const std::string& dataref_identifier);

private:
    XPLMDataRef m_data_ref; //!< opaque handle to X-Plane's data
    RWType m_read_write;    //!< is the data required to be write- or readable
    SimType m_history;      //!< stores the last value to detect changes
};


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


template <typename SimType>
DataRef<SimType>::DataRef(const std::string& identifier,
                          RWType writeability,
                          bool share):
    m_data_ref(0),
    m_read_write(writeability)
{
    try
    {
        if (share)
            shareDataRef(identifier);
        lookUp(identifier);
        checkDataType();
        checkWriteabilityIsValid();
    } catch (LookupException& ex)
    {
        logErrorWithDataRef(ex.what(), identifier);
        // We log only an error and re-throw the exception, so the
        // caller may decide if this is a fatal error that requires an X-Plane exit.
        throw;
    }
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template <typename SimType>
bool DataRef<SimType>::hasChanged() const
{
    return true;
}

template <>
bool DataRef<int>::hasChanged() const;

template <>
bool DataRef<float>::hasChanged() const;

template <>
bool DataRef<double>::hasChanged() const;

template <>
bool DataRef<std::vector<int> >::hasChanged() const;

template <>
bool DataRef<std::vector<float> >::hasChanged() const;

template <>
bool DataRef<std::string>::hasChanged() const;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template <typename SimType>
void DataRef<SimType>::save()
{
    m_history = operator SimType();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template <typename SimType>
void DataRef<SimType>::undo()
{
    operator=(m_history);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template <typename SimType>
void DataRef<SimType>::shareDataRef(const std::string&)
{
    throw IncompatibleTypeException("No type defined for sharing data");
}

template<>
void DataRef<int>::shareDataRef(const std::string&);
template<>
void DataRef<float>::shareDataRef(const std::string&);
template<>
void DataRef<double>::shareDataRef(const std::string&);
template<>
void DataRef<std::vector<int> >::shareDataRef(const std::string&);
template<>
void DataRef<std::vector<float> >::shareDataRef(const std::string&);
template<>
void DataRef<std::string>::shareDataRef(const std::string&);


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


template <typename SimType>
void DataRef<SimType>::lookUp(const std::string& identifier)
{
    m_data_ref = XPLMFindDataRef(identifier.c_str());
    if(!m_data_ref)
        throw NotFoundException("not found in X-Plane's available Datarefs.");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template <typename SimType>
void DataRef<SimType>::checkWriteabilityIsValid()
{
    if(m_read_write == WriteOnly || m_read_write == ReadWrite)
        if (!XPLMCanWriteDataRef(m_data_ref))
            throw NotWriteableException("Declared to be writeable, but X-Plane says it is read-only.");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template <typename SimType>
void DataRef<SimType>::checkDataType()
{
    throw IncompatibleTypeException("No type defined.");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template <typename SimType>
void DataRef<SimType>::logErrorWithDataRef(const std::string& error_msg, const std::string& dataref_identifier)
{
    Log() << Log::Error << "When setting up dataref " << dataref_identifier <<
                  " the following error occured: " << error_msg << Log::endl;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template <>
void DataRef<int>::checkDataType();

template <>
void DataRef<float>::checkDataType();

template <>
void DataRef<double>::checkDataType();

template <>
void DataRef<std::vector<float> >::checkDataType();

template <>
void DataRef<std::vector<int> >::checkDataType();

template <>
void DataRef<std::string>::checkDataType();

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template <>
DataRef<int>::operator int() const;

template <>
DataRef<float>::operator float() const;

template <>
DataRef<double>::operator double() const;

template <>
DataRef<std::vector<float> >::operator std::vector<float>() const;

template <>
DataRef<std::vector<int> >::operator std::vector<int>() const;

template <>
DataRef<std::string>::operator std::string() const;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template <>
const DataRef<int>& DataRef<int>::operator=(const int&);

template <>
const DataRef<float>& DataRef<float>::operator=(const float&);

template <>
const DataRef<double>& DataRef<double>::operator=(const double&);

template <>
const DataRef<std::vector<float> >& DataRef<std::vector<float> >::operator=(const std::vector<float>&);

template <>
const DataRef<std::vector<int> >& DataRef<std::vector<int> >::operator=(const std::vector<int>&);

template <>
const DataRef<std::string>& DataRef<std::string>::operator=(const std::string&);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template <typename SimType>
typename basic_trait<SimType>::Basic DataRef<SimType>::operator[](std::size_t) const
{
    return basic_trait<SimType>::Basic(*this);
}

template<>
basic_trait<std::vector<float> >::Basic DataRef<std::vector<float> >::operator[](std::size_t index) const;

template<>
basic_trait<std::vector<int> >::Basic DataRef<std::vector<int> >::operator[](std::size_t index) const;

}

#endif // DATAREF_H
