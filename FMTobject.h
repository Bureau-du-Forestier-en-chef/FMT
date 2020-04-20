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

#ifndef FMTobject_H_INCLUDED
#define FMTobject_H_INCLUDED

#include "FMTexceptionhandler.h"
#include "FMTlogger.h"
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/nvp.hpp>
#include <memory>


#if defined _MSC_VER || __MINGW64__//_MSC_VER || __CYGWIN__

#else
    #include <boost/dll/runtime_symbol_info.hpp>
#endif



#include "FMTtheme.h"
#include <vector>

namespace Core
{
// DocString: FMTobject
/**
FMTobject is the base class of multiple class it hold a shared exception handler pointer and logger.
This way has soon as a object is create from this base class you can passin the exception handler and the logger
to the newly ceated class. FMTobject plays a big role into exception handling and for ctrl-c signals for boost python.
It also contains some usefull functions for mask validation and runtimelocation etc...
*/
class FMTobject
	{
	// DocString: FMTobject::serialize
	/**
	Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & BOOST_SERIALIZATION_NVP(_exhandler);
		ar & BOOST_SERIALIZATION_NVP(_logger);
	}
	protected:
		// DocString: FMTobject::_exhandler
		///A shared pointer to the exception handler.
		mutable std::shared_ptr<Exception::FMTexceptionhandler> _exhandler;
		// DocString: FMTobject::_logger
		///A shared pointer to the logger.
		std::shared_ptr<Logging::FMTlogger> _logger;
		// DocString: FMTobject::FMTwssect
		///The section in which the child class is in
		mutable FMTwssect _section;
		// DocString: FMTobject::checksignals
		/**
		This function only check if the user has sent a ctrl-c signal using boost::python to FMT.
		*/
		void checksignals() const;
		// DocString: FMTobject::getruntimelocation
		/**
		This function return the location of the FMT shared library location.
		*/
		std::string getruntimelocation();
		// DocString: FMTobject::setCPLhandler
		/**
		This function is for gdal only it pass the FMT exception handler to gdal exception handler.
		*/
		void setCPLhandler();
		// DocString: FMTobject::validate
		/**
		The funciton validate a the construction of a valid FMTmask using the mask string based on the
		themes. If their's less themes that the number present in the string mask then the string mask is 
		going to be trim for the good number of FMTthemes.
		*/
		bool validate(const std::vector<Core::FMTtheme>& themes,
			std::string& mask,std::string otherinformation = std::string()) const;
		// DocString: FMTobject::checkmask
		/**
		This function validate the mask string for a given vector of themes and throw exception if 
		something is not right. It'S called by the validate function.
		*/
		bool checkmask(const std::vector<Core::FMTtheme>& themes, 
			const std::vector<std::string>& values, std::string& mask,
			const std::string& otherinformation) const;
	public:
		// DocString: FMTobject()
		/**
		FMTobject default constructor.
		*/
		FMTobject();
		// DocString: ~FMTobject()
		/**
		FMTobject default virutal destructor.
		*/
		virtual ~FMTobject();
		// DocString: FMTobject(const std::shared_ptr<Exception::FMTexceptionhandler>)
		/**
		When constructing a new FMTobject it's sometime usefull to passin the exception handler of an
		other FMTobject.
		*/
		FMTobject(const std::shared_ptr<Exception::FMTexceptionhandler> exhandler);
		// DocString: FMTobject(const FMTobject&)
		/**
		FMTobject default copy constructor.
		*/
		FMTobject(const FMTobject& rhs);
		// DocString: FMTobject::operator=
		/**
		FMTobject default copy assignment.
		*/
		FMTobject& operator = (const FMTobject& rhs);
		// DocString: FMTobject::passinlogger
		/**
		It's sometime usefull to pass in the logger of an other FMTobject.
		*/
		void passinlogger(const std::shared_ptr<Logging::FMTlogger>& logger);
		// DocString: FMTobject::passinexceptionhandler
		/**
		It's sometime usefull to pass in the exception handler of an other FMTobject.
		*/
		void passinexceptionhandler(const std::shared_ptr<Exception::FMTexceptionhandler>& exhandler);
		// DocString: FMTobject::passinobject
		/**
		It's sometime usefull to pass in the exception handler and the logger  of an other FMTobject to
		a FMTobject.
		*/
		void passinobject(const FMTobject& rhs);
		// DocString: FMTobject::setsection
		/**
		It sets the section member of the FMTobject.
		*/
		void setsection(const FMTwssect& section) const;
		// DocString: FMTobject::setdefaultlogger
		/**
		Create and set a default logger to the FMTobject.
		*/
		void setdefaultlogger();
		// DocString: FMTobject::setdebuglogger
		/**
		Create and set a debug logger to the FMTobject.
		*/
		void setdebuglogger();
		// DocString: FMTobject::setdefaultexceptionhandler
		/**
		Create and set a default exception handler to the FMTobject.
		*/
		void setdefaultexceptionhandler();
		// DocString: FMTobject::setquietexceptionhandler
		/**
		Create and set a quiet exception handler to the FMTobject.
		*/
		void setquietexceptionhandler();
		// DocString: FMTobject::setdebugexceptionhandler
		/**
		Create and set a debug exception handler to the FMTobject.
		*/
		void setdebugexceptionhandler();
		// DocString: FMTobject::setfreeexceptionhandle
		/**
		Create and set a free exception handler to the FMTobject.
		*/
		void setfreeexceptionhandler();
	};
}

#endif

