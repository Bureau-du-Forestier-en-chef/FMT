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

#ifndef FMTwarning_H_INCLUDED
#define FMTwarning_H_INCLUDED

#include "FMTexception.h"
#include <string>
#include "FMTlogger.h"
#include <memory>

namespace Exception
{
	// DocString: FMTwarning
	/**
	FMTwarning is derived from the FMTexception base class.
	See FMTexception class for more information about the member data.
	In FMT warning are never thrown but only logged depending of the FMTlogger and FMTexceptionhandler used.
	*/
	class FMTwarning : public FMTexception
	{
	// DocString: FMTwarning::serialize
	/**
	Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
		{
			ar &  boost::serialization::make_nvp("parent_exception", boost::serialization::base_object<FMTexception>(*this));
		}
	public:
		// DocString: FMTwarning()
		/**
		Default constructor for FMTwarning.
		*/
		FMTwarning();
		// DocString: FMTwarning(const FMTexception&)
		/**
		Default copy constructor for FMTwarning.
		*/
		FMTwarning(const FMTexception& rhs);
		// DocString: FMTwarning(const FMTexc,const std::string)
		/**
		Constructor for FMTwarning taking a exception type and message string as arguments.
		*/
		FMTwarning(const FMTexc lexception, const std::string message);
		// DocString: FMTwarning(const FMTexc,const std::string)
		/**
		Constructor for FMTwarning taking a exception type, section in which the warning occur and message string as arguments.
		*/
		FMTwarning(const FMTexc lexception, const Core::FMTsection lsection, const std::string message);
		// DocString: FMTwarning::warn
		/**
		Using a given logger the function log the warning with the logger. 
		*/
		void warn(const std::shared_ptr<Logging::FMTlogger>logger) const;
	};
}
#endif