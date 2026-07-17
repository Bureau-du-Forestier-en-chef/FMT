/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTPYTHONPICKLE_Hm_included
#define FMTPYTHONPICKLE_Hm_included
	#include <boost/serialization/serialization.hpp>
	#include <boost/serialization/export.hpp>
	#include <boost/iostreams/stream.hpp>
	#include <boost/iostreams/filtering_streambuf.hpp>
	#include <boost/iostreams/filter/zlib.hpp>
	#include <boost/iostreams/copy.hpp>
	#include "FMTModel.h"
	#include "FMTLpModel.h"
	#include "FMTSesModel.h"
	#include "FMTOperatingAreaScheduler.h"
	#include <typeinfo>

#if defined FMTWITHPYTHON

	#include <boost/python.hpp>

namespace Python {


	template<typename T>
	class FMT_pickle_suite : public boost::python::pickle_suite
	{
	public:
		static const char* getName()
			{
			return typeid(T).name();
			}
		// Noms imposés par Boost.Python (protocole pickle) : NE PAS passer en camelCase,
		// sinon boost::python::pickle_suite ne les détecte plus (voir pickle_support.hpp).
		static boost::python::tuple getinitargs(const T&)
		{
			return(boost::python::make_tuple());
		}
		static boost::python::object getstate(const T& target)
		{
			std::stringstream os;
				{
				boost::archive::binary_oarchive oa(os);
				oa << boost::serialization::make_nvp(getName(), target);
				}
			std::stringstream compressed;
			boost::iostreams::filtering_streambuf<boost::iostreams::input> out;
			out.push(boost::iostreams::zlib_compressor());
			out.push(os);
			boost::iostreams::copy(out, compressed);
			return boost::python::make_tuple(compressed.str());
		}
		static void setstate(T& target, boost::python::tuple state)
		{
			boost::python::extract<std::string> input(state[0]);
			std::stringstream compressed(input);
			std::stringstream decompressed;
			boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
			in.push(boost::iostreams::zlib_decompressor());
			in.push(compressed);
			boost::iostreams::copy(in, decompressed);
				{
				boost::archive::binary_iarchive ia(decompressed);
				ia >> boost::serialization::make_nvp(getName(), target);
				}
		}
	};

	#endif


	/*BOOST_CLASS_EXPORT(FMTMask)
	BOOST_CLASS_EXPORT(FMTTheme)
	BOOST_CLASS_EXPORT(FMTDevelopment)
	BOOST_CLASS_EXPORT(FMTActualDevelopment)
	BOOST_CLASS_EXPORT(FMTFuturDevelopment)
	BOOST_CLASS_EXPORT(FMTAction)
	BOOST_CLASS_EXPORT(FMTTransition)
	BOOST_CLASS_EXPORT(FMTYields)
	BOOST_CLASS_EXPORT(FMTOutput)
	BOOST_CLASS_EXPORT(FMTConstraint)
	BOOST_CLASS_EXPORT(FMTSchedule)
	BOOST_CLASS_EXPORT(FMTModel)
	BOOST_CLASS_EXPORT(FMTLpModel)
	BOOST_CLASS_EXPORT(FMTSesModel)*/

}

#endif
