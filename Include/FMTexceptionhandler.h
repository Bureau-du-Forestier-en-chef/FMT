/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTexceptionhandler_Hm_included
#define FMTexceptionhandler_Hm_included


#include "FMTlogger.h"
#include <unordered_map>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include <memory>
#include <set>
#include <boost/thread.hpp>
#if defined _MSC_VER
	#include "FMTScopedSeTranslator.h"
#endif

namespace Exception
{
	class FMTException;
	// DocString: FMTExceptionHandler
	/**
	The FMTExceptionHandler is a base class used to handel error thrown in FMT. It keeps count of the
	number of warning thrown and error thrown it also has a shared pointer to an abstract FMTLogger.
	This class is used in the FMTObject class.
	*/
	class FMTEXPORT FMTExceptionHandler
	{
	public:
		// DocString: FMTExceptionHandler::_specificwarningcount
		///Keeps count of the number of each type of warning thrown.
		std::unordered_map<int, size_t> _specificwarningcount;
		// DocString: checkSignals()
		/**
		Check signals in R and Python, if we are on the main thread.
		*/
		void checkSignals() const;
		// DocString: FMTExceptionHandler()
		/**
		Default constructor for FMTExceptionHandler.
		*/
		FMTExceptionHandler();
		// DocString: ~FMTExceptionHandler()
		/**
		Default virtual destructor for FMTExceptionHandler.
		*/
		virtual ~FMTExceptionHandler() = default;
		// DocString: FMTExceptionHandler(const FMTExceptionHandler&)
		/**
		Copy constructor for FMTExceptionHandler.
		*/
		FMTExceptionHandler(const FMTExceptionHandler& rhs);
		// DocString: FMTExceptionHandler(const std::unique_ptr<Logging::FMTLogger>&)
		/**
		Constructor with logger.
		*/
		FMTExceptionHandler(const std::unique_ptr<Logging::FMTLogger>& logger);
		// DocString: FMTExceptionHandler::passInLogger
		/**
		Pass a logger to the shared pointer of the FMTExceptionHandler class for sharing.
		*/
		void passInLogger(const std::unique_ptr<Logging::FMTLogger>& logger);

		// DocString: FMTExceptionHandler::getCPLdata
		/**
		When using GDAL you need this function for abstract usage in the FMTCPLErrorHandler function.
		it returns a copy of the abstract FMTExceptionHandler.
		*/
		virtual FMTExceptionHandler* getCPLdata();
		#if defined FMTWITHGDAL
				// DocString: FMTExceptionHandler::handelCPLerror
				/**
				Function called back by gdal for handling GDAL error thrown.
				*/
				virtual void handelCPLerror(int eErrClass, int nError, const char* pszErrorMsg);
		#endif
		// DocString: FMTExceptionHandler::operator=
		/**
		Default assignment operator for FMTExceptionHandler.
		*/
		FMTExceptionHandler& operator = (const FMTExceptionHandler& rhs);
		// DocString: FMTExceptionHandler::throwNested
		/**
		This function is not used by FMT seems to be usefull for nested exception thrown.
		*/
		void throwNested(const std::exception& texception, int& level, bool rethrow = true);
		// DocString: FMTExceptionHandler::printExceptions
		/**
		Print all nested exception starting with the first provided by the parameters.
		*/
		virtual void printExceptions(std::string text,
			const std::string& method, const int& line, const std::string& fil,
			Core::FMTsection lsection = Core::FMTsection::Empty);
		// DocString: FMTExceptionHandler::raise
		/**
		This function throw an FMTException based on the exception type,section,text to write, line in the source code
		and file in the source code. Use also this function for warnings.
		*/
		virtual FMTException raise(FMTexc lexception, std::string text,
			const std::string& method, const int& line, const std::string& file,
			Core::FMTsection lsection = Core::FMTsection::Empty, bool throwit = true);
		// DocString: FMTExceptionHandler::raiseFromCatch
		/**
		Raise an exception from the catch body it will determine if the exception is unenhdled.
		*/
		virtual FMTException raiseFromCatch(std::string text,
			const std::string& method, const int& line, const std::string& file,
			Core::FMTsection lsection = Core::FMTsection::Empty);
		// DocString: FMTExceptionHandler::raiseFromThreadCatch
		/**
		Raise an exception from the catch body it will determine if the exception is unenhdled.
		Also catch all the exception to make sure their's no exceptions alive in the thread if not on the main thread.
		Then printalltheexceptions and return without throwing.
		*/
		void raiseFromThreadCatch(std::string text,
			const std::string& method, const int& line, const std::string& file,
			Core::FMTsection lsection = Core::FMTsection::Empty);
		// DocString: FMTExceptionHandler::reRaiseIfThreadCrash
		/**
		If you have used threads make sure to use this to validate that there's no exception raised by any thread...
		if there's one it will raise a multithread error.
		*/
		void reRaiseIfThreadCrash();
		// DocString: FMTExceptionHandler::enableNestedExceptions
		/**
		Enable the nested exception throw.
		*/
		void enableNestedExceptions();
		// DocString: FMTExceptionHandler::disableNestedExceptions
		/**
		Disable the nested exception throw.
		*/
		void disableNestedExceptions();
		// DocString: setErrorsToWarnings()
		/**
		Very hazardous function if you want to live dangerously you can
		set a vector of error to be cast to warnings...
		*/
		void setErrorsToWarnings(const std::vector<Exception::FMTexc>& errors);
		// DocString: setMaxWarningsBeforeSilenced()
		/**
		Settter for maxwarningsbeforesilenced.
		*/
		void setMaxWarningsBeforeSilenced(const size_t& maxwarningcount);
		// DocString: FMTExceptionHandler::Clone
		/**
		@brief clone the FMTExceptionHandler
		@return a valid cloned FMTExceptionHandler
		*/
		virtual std::unique_ptr <FMTExceptionHandler> Clone() const = 0;
		#if defined _MSC_VER
		// DocString: FMTExceptionHandler::translateStructuralWIN32Exceptions
		/**
		@brief translate win32 structural exception to c++ exception
		@param[in] exception id
		@param[in] exception_pointer
		*/
		static void translateStructuralWIN32Exceptions(unsigned int p_u, EXCEPTION_POINTERS*);
		#endif
	protected:
		// DocString: FMTExceptionHandler::_exception
		///This is the type of the last FMTException thrown by the FMTExceptionHandler.
		FMTexc _exception;
		// DocString: FMTExceptionHandler::_errorcount
		///Keeps count of the number of error thrown.
		int _errorcount;
		// DocString: FMTExceptionHandler::maxwarningsbeforesilenced
		///Number of time that the handler throw a certain warning before stop throwing it.
		size_t maxwarningsbeforesilenced;
		// DocString: FMTExceptionHandler::_logger
		///pointer to the logger used to print the warning / error.
		Logging::FMTLogger* _logger;
		// DocString: FMTExceptionHandler::usenestedexceptions
		///If usenested = true then the handler will throw nested exceptions
		bool usenestedexceptions;
		// DocString: FMTExceptionHandler::mtx
		///Mutex for multi-threading.
		//mutable std::recursive_mutex mtx;
		mutable boost::recursive_mutex mtx;
		// DocString: FMTExceptionHandler::errorstowarnings
		///If an error is in this list it's going to processed like a warning.
		std::vector<Exception::FMTexc>errorstowarnings;
		// DocString: FMTExceptionHandler::registred_threads
		///This is the level of the last FMTException thrown by the FMTExceptionHandler.
		std::set<boost::thread::id> registered_threads;
		// DocString: FMTExceptionHandler::mainthreadid
		///Main thread id
		static boost::thread::id mainthreadid;
		// DocString: FMTExceptionHandler::crashedthreadid
		///The thread id of the crashed thread
		static boost::thread::id crashedthreadid;
		// DocString: FMTExceptionHandler::threadcrashexceptions
		///The exception throwed by the faulty thread...
		std::exception_ptr threadcrashexception;
		// DocString: FMTExceptionHandler::updateStatus
		/**
		This functions updates the status of the handler adding up to the warning or the error counts.
		base on the exception type (lexception).
		*/
		std::string updateStatus(const FMTexc lexception, const std::string message);
		// DocString: FMTExceptionHandler::needToRethrow
		/**
		If usenested exceptions and exception == function error then it will be true and the exception will be rethrown.
		*/
		bool needToRethrow() const;
		// DocString: FMTExceptionHandler::isMainThread()
		/**
		Return true if we are on the main thread.
		*/
		bool isMainThread() const;
		// DocString: FMTExceptionHandler::isThrowedOnThread()
		/**
		Return true if an exception been thrown on a thread.
		*/
		bool isThrowedOnThread() const;
		// DocString: FMTExceptionHandler::isThisThreadThrowed()
		/**
		Return true if this thread throwed
		*/
		bool isThisThreadThrowed() const;
		// DocString: FMTExceptionHandler::registerthread
		/**
		Register a thread so that way the exceptionhandler knows that he is dealing with
		a slave thread that is not the main one.
		*/
		void registerWorkerThread();
		// DocString: FMTExceptionHandler::isregistered
		/**
		Return true if the thread is registered.
		*/
		bool isThreadRegistered() const;
		// DocString: FMTExceptionHandler::gutsOfPrintExceptions
		/**
		The guts of printexceptions.
		*/
		void gutsOfPrintExceptions(std::string text,
			const std::string& method, const int& line, const std::string& fil,
			int& levelreference,Core::FMTsection lsection = Core::FMTsection::Empty,bool logfirstlevel = true);
		// DocString: FMTExceptionHandler::gutsOfExceptionLog
		/**
		The guts of logging exceptions...
		*/
		void gutsOfExceptionLog(const std::exception& texception,const int& level);
		// DocString: FMTExceptionHandler::getLevel
		/**
		@brief get the level of an exception.
		@param[in] FMTexc exception enum
		@return the FMTlev
		*/
		FMTlev getLevel(const FMTexc p_exception) const;
	private:
		// DocString: FMTExceptionHandler::serialize
		/**
		Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& BOOST_SERIALIZATION_NVP(_exception);
			ar& BOOST_SERIALIZATION_NVP(_logger);
			ar& BOOST_SERIALIZATION_NVP(usenestedexceptions);
		}
		#if defined _MSC_VER
		// DocString: FMTScopedSeTranslator::m_SeTranslator
		///The structural exceptions win32 translator
		static FMTScopedSeTranslator m_SeTranslator;
		#endif
	};

}

BOOST_CLASS_EXPORT_KEY(Exception::FMTExceptionHandler)

#endif
