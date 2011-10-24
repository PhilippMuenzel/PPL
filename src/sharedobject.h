#ifndef SHAREDOBJECT_H
#define SHAREDOBJECT_H

#include "dataref.h"
#include <boost/serialization/serialization.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

namespace PPL {

template <typename T>
class SharedObject
{
public:
    SharedObject(const std::string& name, T& object):
        dataref_(name, PPL::ReadWrite, true),
        object_(object)
    {
    }

    void save()
    {
        try {
            std::ostringstream out_stream;
            boost::archive::binary_oarchive out_archive(out_stream);
            out_archive << BOOST_SERIALIZATION_NVP(object_);
            dataref_ = out_stream.str();
        } catch (boost::archive::archive_exception&ex) {
            PPL::Log() << PPL::Log::Error << "Could not write object to dataref" << PPL::Log::endl;
        }
    }

    void load()
    {
        try {
            std::istringstream in_stream(std::string(dataref_));
            boost::archive::binary_iarchive in_archive(in_stream);
            in_archive >> BOOST_SERIALIZATION_NVP(object_);
        } catch (boost::archive::archive_exception&ex) {
            PPL::Log() << PPL::Log::Error << "Could not read object from dataref " << PPL::Log::endl;
        }

    }

private:
    DataRef<std::string> dataref_;
    T& object_;
};

}

#endif // SHAREDOBJECT_H
