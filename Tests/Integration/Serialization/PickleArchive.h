/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PICKLEARCHIVE_H_INCLUDED
#define PICKLEARCHIVE_H_INCLUDED

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/serialization/nvp.hpp>

#include <sstream>
#include <string>
#include <typeinfo>

// The archive that a pickle of Python carries, as Python::FMT_pickle_suite (Include/FMTpythonpickle.h) writes and
// reads it: a binary archive of the object, named after its type, compressed by zlib. The suite is compiled only in
// FMT.pyd (FMTWITHPYTHON), and hands the state to Python: these functions repeat its C++ part, so that a test reaches
// the archive of a pickle without Python (Documentation/Architecture.md, Known Architectural Debt). Like FMT.pyd, the
// test executable writes and reads the archive in one module. The archive carries objects between the processes of one
// build: it depends on the platform, has no version, and is not kept from one version of FMT to the next.
namespace Testing
{
    // The state that FMT_pickle_suite<T>::getstate hands to Python, before its conversion to a Python object.
    template<typename T>
    std::string getPickleState(const T& p_object)
    {
        std::stringstream archived;
        {
            boost::archive::binary_oarchive archive(archived);
            archive << boost::serialization::make_nvp(typeid(T).name(), p_object);
        }
        std::stringstream compressed;
        boost::iostreams::filtering_streambuf<boost::iostreams::input> compressor;
        compressor.push(boost::iostreams::zlib_compressor());
        compressor.push(archived);
        boost::iostreams::copy(compressor, compressed);
        return compressed.str();
    }

    // Restores an object from a state, as FMT_pickle_suite<T>::setstate does.
    template<typename T>
    void setPickleState(T& p_object, const std::string& p_state)
    {
        std::stringstream compressed(p_state);
        std::stringstream archived;
        boost::iostreams::filtering_streambuf<boost::iostreams::input> decompressor;
        decompressor.push(boost::iostreams::zlib_decompressor());
        decompressor.push(compressed);
        boost::iostreams::copy(decompressor, archived);
        boost::archive::binary_iarchive archive(archived);
        archive >> boost::serialization::make_nvp(typeid(T).name(), p_object);
    }

    // pickle.dumps then pickle.loads: the state of an object, restored in an object constructed without argument, as
    // Python constructs it before setstate (FMT_pickle_suite<T>::getinitargs gives an empty tuple).
    template<typename T>
    T pickleAndUnpickle(const T& p_object)
    {
        const std::string state = getPickleState(p_object);
        T unpickled;
        setPickleState(unpickled, state);
        return unpickled;
    }
}

#endif
