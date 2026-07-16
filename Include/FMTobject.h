/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTobject_Hm_included
#define FMTobject_Hm_included

#ifndef NOMINMAX
	#define NOMINMAX
#endif // !NOMINMAX

#include <boost/serialization/unique_ptr.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include "FMTexception.h"
#include <memory>
#include <chrono>
#include <vector>

#if defined _MSC_VER || __MINGW64__ || __CYGWIN__

#else
    #include <boost/dll/runtime_symbol_info.h>
#endif

namespace Exception
{
	class FMTExceptionHandler;
}

namespace Logging
{
	class FMTLogger;
}



namespace Core
{
// DocString: FMTObject
/**
FMTObject is the base class of multiple class it hold a shared exception handler pointer and logger.
FMTObject plays a big role into exception handling and for ctrl-c signals for boost python.
It also contains some usefull functions for mask validation and runtimelocation etc...
*/
class FMTEXPORT FMTObject
	{
	public:
		// DocString: FMTObject::getLogger
		/**
		@brief get a pointer to the actual logger.
		@return the valid FMTLogger
		*/
		static Logging::FMTLogger* getLogger();
		// DocString: FMTObject::getExceptionHandler
		/**
		@brief get a pointer to the actual exception handler.
		@return the valid FMTExceptionHandler;
		*/
		static Exception::FMTExceptionHandler* getExceptionHandler();
		// DocString: FMTObject::getRuntimeLocation
		/**
		This function return the location of the FMT shared library location.
		*/
		static std::string getRuntimeLocation();
		// DocString: FMTObject::getAvailableMemory
		/**
		Get the available memory in bytes
		*/
		static unsigned long long getAvailableMemory();
		// DocString: FMTObject()
		/**
		FMTObject default constructor.
		*/
		FMTObject();
		// DocString: ~FMTObject()
		/**
		FMTObject default virutal destructor.
		*/
		virtual ~FMTObject();
		// DocString: FMTObject(const std::unique_ptr<Exception::FMTExceptionHandler>)
		/**
		When constructing a new FMTObject it's sometime usefull to passin the exception handler of an
		other FMTObject.
		*/
		FMTObject(const std::unique_ptr<Exception::FMTExceptionHandler> exhandler);
		// DocString: FMTObject(const FMTObject&)
		/**
		FMTObject default copy constructor.
		*/
		FMTObject(const FMTObject& rhs);
		// DocString: FMTObject::operator=
		/**
		FMTObject default copy assignment.
		*/
		FMTObject& operator = (const FMTObject& rhs);
		// DocString: FMTObject::passInLogger
		/**
		It's sometime usefull to pass in the logger of an other FMTObject.
		*/
		virtual void passInLogger(const std::unique_ptr<Logging::FMTLogger>& logger);
		// DocString: FMTObject::passInExceptionHandler
		/**
		It's sometime usefull to pass in the exception handler of an other FMTObject.
		*/
		void passInExceptionHandler(const std::unique_ptr<Exception::FMTExceptionHandler>& exhandler);
		// DocString: FMTObject::redirectLogToFile
		/**
		redict the log to a specific file (will append to it)
		*/
		void redirectLogToFile(const std::string& location);
		// DocString: FMTObject::setDefaultLogger
		/**
		Create and set a default logger to the FMTObject.
		*/
		virtual void setDefaultLogger();
		// DocString: FMTObject::setQuietLogger
		/**
		Create and set a quiet logger to the FMTObject.
		*/
		virtual void setQuietLogger();
		// DocString: FMTObject::setTaskLogger
		/**
		Create and set a quiet logger to the FMTObject.
		*/
		virtual void setTaskLogger();
		// DocString: FMTObject::setDebugLogger
		/**
		Create and set a debug logger to the FMTObject.
		*/
		virtual void setDebugLogger();
		// DocString: FMTObject::setDefaultExceptionHandler
		/**
		Create and set a default exception handler to the FMTObject.
		*/
		void setDefaultExceptionHandler();
		// DocString: FMTObject::setQuietExceptionHandler
		/**
		Create and set a quiet exception handler to the FMTObject.
		*/
		void setQuietExceptionHandler();
		// DocString: FMTObject::setDebugExceptionHandler
		/**
		Create and set a debug exception handler to the FMTObject.
		*/
		void setDebugExceptionHandler();
		// DocString: FMTObject::setFreeExceptionHandler
		/**
		Create and set a free exception handler to the FMTObject.
		*/
		void setFreeExceptionHandler();
		// DocString: FMTObject::disableNestedExceptions
		/**
		Disable nested exception throw of the Exceptionhandler by default all handlers
		do nested exception throw.
		*/
		void disableNestedExceptions();
		// DocString: FMTObject::enableNestedExceptions
		/**
		Enable nested exception throw of the Exceptionhandler by default all handlers
		do nested exception throw.
		*/
		void enableNestedExceptions();
		// DocString: FMTObject::setErrorsToWarnings
		/**
		Very hazardous function if you want to live dangerously you can
		set a vector of error to be cast to warnings to the exception handler...
		*/
		void setErrorsToWarnings(const std::vector<Exception::FMTexc>& errors);
		// DocString: FMTObject::setMaxWarningsBeforeSilenced
		/**
		Change the number of warning raise before silenced.
		*/
		void setMaxWarningsBeforeSilenced(const size_t& maxwarningcount);
		// DocString: FMTExceptionHandler::setTerminateStack
		/**
		@brief will write the stack in the log when terminate called and raise a function failed.
		*/
		static void setTerminateStack();
		// DocString: FMTExceptionHandler::setAbortStack
		/**
		@brief will write the stack in the log when abort called with SIGABRT and raise a function failed.
		*/
		static void setAbortStack();
		// DocString: FMTObject::serialize
		/**
		Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			//ar & BOOST_SERIALIZATION_NVP(_exhandler);
			//ar & BOOST_SERIALIZATION_NVP(_logger);
			//ar & BOOST_SERIALIZATION_NVP(m_section);
		}
	protected:
		// DocString: FMTObject::_exhandler
		///A shared pointer to the exception handler.
		static std::unique_ptr<Exception::FMTExceptionHandler> _exhandler;
		// DocString: FMTObject::_logger
		///A shared pointer to the logger.
		static std::unique_ptr<Logging::FMTLogger> _logger;
		// DocString: FMTObject:: forceSave
		/**
		By Default the serialization of a FMTObject does nothing if you want to get some usefull information use this function.
		*/
		template<class Archive>
		void forceSave(Archive& ar, const unsigned int version) const
		{
			//ar & BOOST_SERIALIZATION_NVP(_exhandler);
			//ar & BOOST_SERIALIZATION_NVP(_logger);
			//ar & BOOST_SERIALIZATION_NVP(m_section);
		}
		// DocString: FMTObject:: forceLoad
		/**
		By Default the serialization of a FMTObject does nothing if you want to get some usefull information use this function.
		*/
		template<class Archive>
		void forceLoad(Archive& ar, const unsigned int version)
		{
			//ar & BOOST_SERIALIZATION_NVP(_exhandler);
			//ar & BOOST_SERIALIZATION_NVP(_logger);
			setDefaultLogger();
			setDefaultExceptionHandler();
			//ar & BOOST_SERIALIZATION_NVP(m_section);
		}
		// DocString: FMTObject::checkSignals
		/**
		This function only check if the user has sent a ctrl-c signal using boost::python to FMT.
		*/
		void checkSignals() const;
		// DocString: FMTObject::setCPLhandler
		/**
		This function is for gdal only it pass the FMT exception handler to gdal exception handler.
		*/
		void setCPLhandler();
		// DocString: FMTObject::getClock
		/**
		Will return a clock of "now" time.
		*/
		static std::chrono::time_point<std::chrono::high_resolution_clock> getClock();
		// DocString: FMTObject::getDuration
		/**
		With the high resolution clock you can get the time it took has a double.
		*/
		template<class chrono>
		static double getDuration(const std::chrono::time_point<std::chrono::high_resolution_clock>& startclock);
		// DocString: FMTObject::getDurationInSeconds
		/**
		With the clock time calculate time spent in second and return a string.
		*/
		static std::string getDurationInSeconds(const std::chrono::time_point<std::chrono::high_resolution_clock>& startclock);
		// DocString: FMTExceptionHandler::_logStack
		/**
		@brief Log the stack trace...
		*/
		static void _logStack();
		// DocString: FMTExceptionHandler::_terminate
		/**
		@brief Raise an error with the boost stacktrace.
		*/
		static void _terminate();
		// DocString: FMTExceptionHandler::_abort
		/**
		@brief Raise an error with the boost stacktrace.
		@param[in] the signal for abort
		*/
		static void _abort(int p_signal);
		

	};
}
BOOST_SERIALIZATION_ASSUME_ABSTRACT(Core::FMTObject)
BOOST_CLASS_TRACKING(Core::FMTObject, boost::serialization::track_always)
BOOST_CLASS_EXPORT_KEY(Core::FMTObject)

#endif

