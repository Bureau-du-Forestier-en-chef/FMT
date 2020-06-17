/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTPYTHONPICKLE_H_INCLUDED
#define FMTPYTHONPICKLE_H_INCLUDED
	#include <boost/serialization/serialization.hpp>
	#include <boost/serialization/export.hpp>
	#include <boost/iostreams/stream.hpp>
	#include <boost/iostreams/filtering_streambuf.hpp>
	#include <boost/iostreams/filter/zlib.hpp>
	#include <boost/iostreams/copy.hpp>
	#include "FMTmodel.h"
	#include "FMTlpmodel.h"
	#include "FMTsesmodel.h"
	#include "FMToperatingareaheuristic.h"
	#include <typeinfo>

#if defined FMTWITHPYTHON

namespace Python {


	template<typename T>
	class FMT_pickle_suite : public boost::python::pickle_suite
	{
	public:
		static const char* getname()
			{
			return typeid(T).name();
			}
		static boost::python::tuple getinitargs(const T&)
		{
			return(boost::python::make_tuple());
		}
		static boost::python::object getstate(const T& target)
		{
			std::stringstream os;
				{
				boost::archive::binary_oarchive oa(os);
				oa << boost::serialization::make_nvp(getname(), target);
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
				ia >> boost::serialization::make_nvp(getname(), target);
				}
		}
	};

	#endif


	/*BOOST_CLASS_EXPORT(FMTmask)
	BOOST_CLASS_EXPORT(FMTtheme)
	BOOST_CLASS_EXPORT(FMTdevelopment)
	BOOST_CLASS_EXPORT(FMTactualdevelopment)
	BOOST_CLASS_EXPORT(FMTfuturdevelopment)
	BOOST_CLASS_EXPORT(FMTaction)
	BOOST_CLASS_EXPORT(FMTtransition)
	BOOST_CLASS_EXPORT(FMTyields)
	BOOST_CLASS_EXPORT(FMToutput)
	BOOST_CLASS_EXPORT(FMTconstraint)
	BOOST_CLASS_EXPORT(FMTschedule)
	BOOST_CLASS_EXPORT(FMTmodel)
	BOOST_CLASS_EXPORT(FMTlpmodel)
	BOOST_CLASS_EXPORT(FMTsesmodel)*/

}

#endif
