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

#ifndef FMTdefaultexceptionhandler_H_INCLUDED
#define FMTdefaultexceptionhandler_H_INCLUDED

#include "FMTexceptionhandler.h"
#include "FMTexception.h"
#include <string>

namespace Exception
{
// DocString: FMTdefaultexceptionhandler
/**
The FMTdefaultexceptionhandler is derived from the FMTexceptionhandler base class and is
the default exception handler used in FMT.
*/
class FMTdefaultexceptionhandler final : public FMTexceptionhandler
	{
	// DocString: FMTdefaultexceptionhandler::serialize
	/**
	Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar &  boost::serialization::make_nvp("parent_handler", boost::serialization::base_object<FMTexceptionhandler>(*this));
	}
	public:
		// DocString: FMTdefaultexceptionhandler()
		/**
		Default constructor for FMTdefaultexceptionhandler.
		*/
		FMTdefaultexceptionhandler();
		// DocString: ~FMTdefaultexceptionhandler()
		/**
		Default destructor for FMTdefaultexceptionhandler.
		*/
		~FMTdefaultexceptionhandler() = default;
		// DocString: FMTdefaultexceptionhandler::raise
		/**
		The function overide the base class raise function for a default implementation.
		See raise function of FMTexceptionhandler class.
		*/
		FMTlev raise(FMTexc lexception, Core::FMTwssect lsection, std::string text,
			const int& line, const std::string& file) override;
#ifdef FMTWITHGDAL
		// DocString: FMTdefaultexceptionhandler::getCPLdata
		/**
		Used in the handelCPLerror called back by GDAL function reutnr a abstract copy of itselft.
		See getCPLdata of FMTexceptionhandler class.
		*/
		FMTexceptionhandler* getCPLdata() override;
		// DocString: FMTdefaultexceptionhandler::handelCPLerror
		/**
		Used has call back in gdal.
		See handelCPLerror of FMTexceptionhandler class.
		*/
		void handelCPLerror(CPLErr eErrClass, CPLErrorNum nError, const char * pszErrorMsg) override;
#endif
	};
}

#endif

