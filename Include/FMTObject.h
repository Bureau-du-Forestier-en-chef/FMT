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
#include "FMTException.h"
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
@brief Base class of multiple FMT classes holding a shared exception handler pointer and logger.
@details Plays a major role in exception handling and ctrl-c signals for boost::python, and provides utility functions for mask validation and runtime location.
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
		@brief Return the location of the FMT shared library.
		@return the runtime location of the FMT shared library.
		*/
		static std::string getRuntimeLocation();
		// DocString: FMTObject::getAvailableMemory
		/**
		@brief Return the available memory in bytes.
		@return the available memory in bytes.
		*/
		static unsigned long long getAvailableMemory();
		// DocString: FMTObject()
		/**
		@brief Default constructor for FMTObject.
		*/
		FMTObject();
		// DocString: ~FMTObject()
		/**
		@brief Default virtual destructor for FMTObject.
		*/
		virtual ~FMTObject();
		// DocString: FMTObject(const std::unique_ptr<Exception::FMTExceptionHandler>)
		/**
		@brief Construct a FMTObject passing in the exception handler of another FMTObject.
		@param[in] exhandler the exception handler to pass in.
		*/
		FMTObject(const std::unique_ptr<Exception::FMTExceptionHandler> exhandler);
		// DocString: FMTObject(const FMTObject&)
		/**
		@brief Copy constructor for FMTObject.
		@param[in] rhs the FMTObject to copy.
		*/
		FMTObject(const FMTObject& rhs);
		// DocString: FMTObject::operator=
		/**
		@brief Copy assignment operator for FMTObject.
		@param[in] rhs the FMTObject to copy.
		@return a reference to this FMTObject.
		*/
		FMTObject& operator = (const FMTObject& rhs);
		// DocString: FMTObject::passInLogger
		/**
		@brief Pass in the logger of another FMTObject.
		@param[in] logger the logger to pass in.
		*/
		virtual void passInLogger(const std::unique_ptr<Logging::FMTLogger>& logger);
		// DocString: FMTObject::passInExceptionHandler
		/**
		@brief Pass in the exception handler of another FMTObject.
		@param[in] exhandler the exception handler to pass in.
		*/
		void passInExceptionHandler(const std::unique_ptr<Exception::FMTExceptionHandler>& exhandler);
		// DocString: FMTObject::redirectLogToFile
		/**
		@brief Redirect the log to a specific file, appending to it.
		@param[in] location the file to redirect the log to.
		*/
		void redirectLogToFile(const std::string& location);
		// DocString: FMTObject::setDefaultLogger
		/**
		@brief Create and set a default logger to the FMTObject.
		*/
		virtual void setDefaultLogger();
		// DocString: FMTObject::setQuietLogger
		/**
		@brief Create and set a quiet logger to the FMTObject.
		*/
		virtual void setQuietLogger();
		// DocString: FMTObject::setTaskLogger
		/**
		@brief Create and set a task logger to the FMTObject.
		*/
		virtual void setTaskLogger();
		// DocString: FMTObject::setDebugLogger
		/**
		@brief Create and set a debug logger to the FMTObject.
		*/
		virtual void setDebugLogger();
		// DocString: FMTObject::setDefaultExceptionHandler
		/**
		@brief Create and set a default exception handler to the FMTObject.
		*/
		void setDefaultExceptionHandler();
		// DocString: FMTObject::setQuietExceptionHandler
		/**
		@brief Create and set a quiet exception handler to the FMTObject.
		*/
		void setQuietExceptionHandler();
		// DocString: FMTObject::setDebugExceptionHandler
		/**
		@brief Create and set a debug exception handler to the FMTObject.
		*/
		void setDebugExceptionHandler();
		// DocString: FMTObject::setFreeExceptionHandler
		/**
		@brief Create and set a free exception handler to the FMTObject.
		*/
		void setFreeExceptionHandler();
		// DocString: FMTObject::disableNestedExceptions
		/**
		@brief Disable the nested exception throw of the exception handler.
		*/
		void disableNestedExceptions();
		// DocString: FMTObject::enableNestedExceptions
		/**
		@brief Enable the nested exception throw of the exception handler.
		*/
		void enableNestedExceptions();
		// DocString: FMTObject::setErrorsToWarnings
		/**
		@brief Set a list of errors to be cast to warnings on the exception handler.
		@param[in] errors the errors to treat as warnings.
		*/
		void setErrorsToWarnings(const std::vector<Exception::FMTexc>& errors);
		// DocString: FMTObject::setMaxWarningsBeforeSilenced
		/**
		@brief Set the number of warnings raised before being silenced.
		@param[in] maxwarningcount the maximum warning count.
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
		@brief Serialize the FMTObject for multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to serialize to or from.
		@param[in] version the serialization version.
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
		@brief Force the serialization to save useful information, which the default FMTObject serialization does not.
		@tparam Archive the archive type.
		@param[in,out] ar the archive to save to.
		@param[in] version the serialization version.
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
		@brief Force the serialization to load useful information, which the default FMTObject serialization does not.
		@tparam Archive the archive type.
		@param[in,out] ar the archive to load from.
		@param[in] version the serialization version.
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
		@brief Check if the user has sent a ctrl-c signal using boost::python to FMT.
		*/
		void checkSignals() const;
		// DocString: FMTObject::setCPLhandler
		/**
		@brief Pass the FMT exception handler to the GDAL exception handler (GDAL only).
		*/
		void setCPLhandler();
		// DocString: FMTObject::getClock
		/**
		@brief Return a clock of the current time.
		@return a clock of the current time.
		*/
		static std::chrono::time_point<std::chrono::high_resolution_clock> getClock();
		// DocString: FMTObject::getDuration
		/**
		@brief Return the time elapsed since a start clock as a double.
		@tparam chrono the duration type.
		@param[in] startclock the start clock.
		@return the elapsed time.
		*/
		template<class chrono>
		static double getDuration(const std::chrono::time_point<std::chrono::high_resolution_clock>& startclock);
		// DocString: FMTObject::getDurationInSeconds
		/**
		@brief Return the time elapsed since a start clock in seconds as a string.
		@param[in] startclock the start clock.
		@return the elapsed time in seconds.
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
		@brief Raise an error with the boost stacktrace on abort.
		@param[in] p_signal the signal for abort.
		*/
		static void _abort(int p_signal);
		

	};
}
BOOST_SERIALIZATION_ASSUME_ABSTRACT(Core::FMTObject)
BOOST_CLASS_TRACKING(Core::FMTObject, boost::serialization::track_always)
BOOST_CLASS_EXPORT_KEY(Core::FMTObject)

#endif

