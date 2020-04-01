/*
MIT License

Copyright (c) [2019] [Bureau du forestier en chef]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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
