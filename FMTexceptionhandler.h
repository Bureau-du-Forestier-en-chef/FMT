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

#ifndef FMTexceptionhandler_H_INCLUDED
#define FMTexceptionhandler_H_INCLUDED

#include "FMTexception.h"
#include "FMTerror.h"
#include "FMTwarning.h"
#include "FMTlogger.h"
#ifdef FMTWITHGDAL
	#include "cpl_error.h"
#endif
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <memory>

namespace Exception
{
#ifdef FMTWITHGDAL
	void FMTCPLErrorHandler(CPLErr eErrClass, CPLErrorNum nError, const char * pszErrorMsg);
#endif

	class FMTexceptionhandler
	{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
		{
			ar & BOOST_SERIALIZATION_NVP(_level);
			ar & BOOST_SERIALIZATION_NVP(_exception);
			ar & BOOST_SERIALIZATION_NVP(_errorcount);
			ar & BOOST_SERIALIZATION_NVP(_warningcount);
			ar & BOOST_SERIALIZATION_NVP(_logger);
		}
	protected:
		FMTlev _level;
		FMTexc _exception;
		int _errorcount, _warningcount;
		std::shared_ptr<Logging::FMTlogger>_logger;
		std::string updatestatus(const FMTexc lexception, const std::string message);
	public:
		FMTexceptionhandler();
		virtual ~FMTexceptionhandler() = default;
		FMTexceptionhandler(const FMTexceptionhandler& rhs);
		void passinlogger(const std::shared_ptr<Logging::FMTlogger>& logger);
		#ifdef FMTWITHGDAL
			virtual FMTexceptionhandler* getCPLdata();
			virtual void handelCPLerror(CPLErr eErrClass, CPLErrorNum nError, const char * pszErrorMsg);
		#endif
		FMTexceptionhandler& operator = (const FMTexceptionhandler& rhs);
		void throw_nested(const FMTexception& texception, int level = 0);
		virtual FMTlev raise(FMTexc lexception, Core::FMTwssect lsection, std::string text,
			const int& line, const std::string& file);
	};

}

#endif
