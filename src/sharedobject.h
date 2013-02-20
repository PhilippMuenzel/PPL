// Copyright (c) 2013, Philipp Muenzel mail@philippmuenzel.de
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// The views and conclusions contained in the software and documentation are those
// of the authors and should not be interpreted as representing official policies,
// either expressed or implied, of the FreeBSD Project.

#ifndef SHAREDOBJECT_H
#define SHAREDOBJECT_H

#include "dataref.h"
#include <boost/serialization/serialization.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include "namespaces.h"

namespace PPLNAMESPACE {

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
            std::istringstream in_stream(dataref_);
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
