/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTobject_H_INCLUDED
#define FMTobject_H_INCLUDED

#include "FMTexceptionhandler.hpp"
#include "FMTlogger.hpp"
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include <memory>
#include <chrono>
#include <vector>

#if defined _MSC_VER || __MINGW64__ || __CYGWIN__

#else
    #include <boost/dll/runtime_symbol_info.hpp>
#endif



namespace Core
{
// DocString: FMTobject
/**
FMTobject is the base class of multiple class it hold a shared exception handler pointer and logger.
FMTobject plays a big role into exception handling and for ctrl-c signals for boost python.
It also contains some usefull functions for mask validation and runtimelocation etc...
*/
class FMTEXPORT FMTobject
	{
	// DocString: FMTobject::serialize
	/**
	Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		//ar & BOOST_SERIALIZATION_NVP(_exhandler);
		//ar & BOOST_SERIALIZATION_NVP(_logger);
		//ar & BOOST_SERIALIZATION_NVP(_section);
	}
	protected:
		// DocString: FMTobject::_exhandler
		///A shared pointer to the exception handler.
		static std::shared_ptr<Exception::FMTexceptionhandler> _exhandler;
		// DocString: FMTobject::_logger
		///A shared pointer to the logger.
		static std::shared_ptr<Logging::FMTlogger> _logger;
		// DocString: FMTobject:: forcesave
		/**
		By Default the serialization of a FMTobject does nothing if you want to get some usefull information use this function.
		*/
		template<class Archive>
		void forcesave(Archive& ar, const unsigned int version) const
		{
			//ar & BOOST_SERIALIZATION_NVP(_exhandler);
			//ar & BOOST_SERIALIZATION_NVP(_logger);
			//ar & BOOST_SERIALIZATION_NVP(_section);
		}
		// DocString: FMTobject:: forceload
		/**
		By Default the serialization of a FMTobject does nothing if you want to get some usefull information use this function.
		*/
		template<class Archive>
		void forceload(Archive& ar, const unsigned int version)
		{
			//ar & BOOST_SERIALIZATION_NVP(_exhandler);
			//ar & BOOST_SERIALIZATION_NVP(_logger);
			setdefaultlogger();
			setdefaultexceptionhandler();
			//ar & BOOST_SERIALIZATION_NVP(_section);
		}
		// DocString: FMTobject::checksignals
		/**
		This function only check if the user has sent a ctrl-c signal using boost::python to FMT.
		*/
		void checksignals() const;
		// DocString: FMTobject::getruntimelocation
		/**
		This function return the location of the FMT shared library location.
		*/
		static std::string getruntimelocation();
		// DocString: FMTobject::setCPLhandler
		/**
		This function is for gdal only it pass the FMT exception handler to gdal exception handler.
		*/
		void setCPLhandler();
		// DocString: FMTobject::getclock
		/**
		Will return a clock of "now" time.
		*/
		static std::chrono::time_point<std::chrono::high_resolution_clock> getclock();
		// DocString: FMTobject::getduration
		/**
		With the high resolution clock you can get the time it took has a double.
		*/
		template<class chrono>
		static double getduration(const std::chrono::time_point<std::chrono::high_resolution_clock>& startclock);
		// DocString: FMTobject::getdurationinseconds
		/**
		With the clock time calculate time spent in second and return a string.
		*/
		static std::string getdurationinseconds(const std::chrono::time_point<std::chrono::high_resolution_clock>& startclock);
	public:
		// DocString: FMTobject::getavailablememory
		/**
		Get the available memory in bytes
		*/
		static unsigned long long getavailablememory();
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
		virtual void passinlogger(const std::shared_ptr<Logging::FMTlogger>& logger);
		// DocString: FMTobject::passinexceptionhandler
		/**
		It's sometime usefull to pass in the exception handler of an other FMTobject.
		*/
		void passinexceptionhandler(const std::shared_ptr<Exception::FMTexceptionhandler>& exhandler);
		// DocString: FMTobject::redirectlogtofile
		/**
		redict the log to a specific file (will append to it)
		*/
		void redirectlogtofile(const std::string& location);
		// DocString: FMTobject::setdefaultlogger
		/**
		Create and set a default logger to the FMTobject.
		*/
		virtual void setdefaultlogger();
		// DocString: FMTobject::setquietlogger
		/**
		Create and set a quiet logger to the FMTobject.
		*/
		virtual void setquietlogger();
		// DocString: FMTobject::setdebuglogger
		/**
		Create and set a debug logger to the FMTobject.
		*/
		virtual void setdebuglogger();
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
		// DocString: FMTobject::disablenestedexceptions
		/**
		Disable nested exception throw of the Exceptionhandler by default all handlers
		do nested exception throw.
		*/
		void disablenestedexceptions();
		// DocString: FMTobject::enablenestedexceptions
		/**
		Enable nested exception throw of the Exceptionhandler by default all handlers
		do nested exception throw.
		*/
		void enablenestedexceptions();
		// DocString: FMTobject::seterrorstowarnings
		/**
		Very hazardous function if you want to live dangerously you can
		set a vector of error to be cast to warnings to the exception handler...
		*/
		void seterrorstowarnings(const std::vector<Exception::FMTexc>& errors);

	};
}
BOOST_SERIALIZATION_ASSUME_ABSTRACT(Core::FMTobject)
BOOST_CLASS_TRACKING(Core::FMTobject, boost::serialization::track_always)
BOOST_CLASS_EXPORT_KEY(Core::FMTobject)

#endif

