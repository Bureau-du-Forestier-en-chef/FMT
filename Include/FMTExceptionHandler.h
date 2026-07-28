/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTexceptionhandler_Hm_included
#define FMTexceptionhandler_Hm_included


#include "FMTLogger.h"
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
	@brief Base class used to handle errors thrown in FMT.
	@details Keeps count of the warnings and errors thrown and holds a shared pointer to an abstract FMTLogger. Used in the FMTObject class.
	*/
	class FMTEXPORT FMTExceptionHandler
	{
	public:
		// DocString: FMTExceptionHandler::_specificwarningcount
		///Keeps count of the number of each type of warning thrown.
		std::unordered_map<int, size_t> _specificwarningcount;
		// DocString: checkSignals()
		/**
		@brief Check signals in R and Python if on the main thread.
		*/
		void checkSignals() const;
		// DocString: FMTExceptionHandler()
		/**
		@brief Default constructor for FMTExceptionHandler.
		*/
		FMTExceptionHandler();
		// DocString: ~FMTExceptionHandler()
		/**
		@brief Default virtual destructor for FMTExceptionHandler.
		*/
		virtual ~FMTExceptionHandler() = default;
		// DocString: FMTExceptionHandler(const FMTExceptionHandler&)
		/**
		@brief Copy constructor for FMTExceptionHandler.
		@param[in] rhs the FMTExceptionHandler to copy.
		*/
		FMTExceptionHandler(const FMTExceptionHandler& rhs);
		// DocString: FMTExceptionHandler(const std::unique_ptr<Logging::FMTLogger>&)
		/**
		@brief Construct a FMTExceptionHandler with a logger.
		@param[in] logger the logger to use.
		*/
		FMTExceptionHandler(const std::unique_ptr<Logging::FMTLogger>& logger);
		// DocString: FMTExceptionHandler::passInLogger
		/**
		@brief Pass a logger to the shared pointer of the handler for sharing.
		@param[in] logger the logger to pass in.
		*/
		void passInLogger(const std::unique_ptr<Logging::FMTLogger>& logger);

		// DocString: FMTExceptionHandler::getCPLdata
		/**
		@brief Return a copy of the abstract handler for use in the FMTCPLErrorHandler function when using GDAL.
		@return a pointer to the exception handler.
		*/
		virtual FMTExceptionHandler* getCPLdata();
		#if defined FMTWITHGDAL
				// DocString: FMTExceptionHandler::handelCPLerror
				/**
				@brief Callback called by GDAL for handling GDAL errors thrown.
				@param[in] eErrClass the error class.
				@param[in] nError the error number.
				@param[in] pszErrorMsg the error message.
				*/
				virtual void handelCPLerror(int eErrClass, int nError, const char* pszErrorMsg);
		#endif
		// DocString: FMTExceptionHandler::operator=
		/**
		@brief Default copy assignment operator for FMTExceptionHandler.
		@param[in] rhs the FMTExceptionHandler to copy.
		@return a reference to this FMTExceptionHandler.
		*/
		FMTExceptionHandler& operator = (const FMTExceptionHandler& rhs);
		// DocString: FMTExceptionHandler::throwNested
		/**
		@brief Throw a nested exception.
		@param[in] texception the exception to nest.
		@param[in,out] level the nesting level.
		@param[in] rethrow if true rethrows the exception.
		*/
		void throwNested(const std::exception& texception, int& level, bool rethrow = true);
		// DocString: FMTExceptionHandler::printExceptions
		/**
		@brief Print all nested exceptions starting with the first provided.
		@param[in] text the message of the exception.
		@param[in] method the method where the exception occurred.
		@param[in] line the line where the exception occurred.
		@param[in] fil the file where the exception occurred.
		@param[in] lsection the section in which the exception occurred.
		*/
		virtual void printExceptions(std::string text,
			const std::string& method, const int& line, const std::string& fil,
			Core::FMTsection lsection = Core::FMTsection::Empty);
		// DocString: FMTExceptionHandler::raise
		/**
		@brief Throw a FMTException based on the exception type, section, message and source location. Also used for warnings.
		@param[in] lexception the exception type.
		@param[in] text the message of the exception.
		@param[in] method the method where the exception occurred.
		@param[in] line the line where the exception occurred.
		@param[in] file the file where the exception occurred.
		@param[in] lsection the section in which the exception occurred.
		@param[in] throwit if true throws the exception.
		@return the raised FMTException.
		*/
		virtual FMTException raise(FMTexc lexception, std::string text,
			const std::string& method, const int& line, const std::string& file,
			Core::FMTsection lsection = Core::FMTsection::Empty, bool throwit = true);
		// DocString: FMTExceptionHandler::raiseFromCatch
		/**
		@brief Raise an exception from a catch body, determining if the exception is unhandled.
		@param[in] text the message of the exception.
		@param[in] method the method where the exception occurred.
		@param[in] line the line where the exception occurred.
		@param[in] file the file where the exception occurred.
		@param[in] lsection the section in which the exception occurred.
		@return the raised FMTException.
		*/
		virtual FMTException raiseFromCatch(std::string text,
			const std::string& method, const int& line, const std::string& file,
			Core::FMTsection lsection = Core::FMTsection::Empty);
		// DocString: FMTExceptionHandler::raiseFromThreadCatch
		/**
		@brief Raise an exception from a catch body on a worker thread, catching all exceptions and printing them without throwing.
		@param[in] text the message of the exception.
		@param[in] method the method where the exception occurred.
		@param[in] line the line where the exception occurred.
		@param[in] file the file where the exception occurred.
		@param[in] lsection the section in which the exception occurred.
		*/
		void raiseFromThreadCatch(std::string text,
			const std::string& method, const int& line, const std::string& file,
			Core::FMTsection lsection = Core::FMTsection::Empty);
		// DocString: FMTExceptionHandler::reRaiseIfThreadCrash
		/**
		@brief Re-raise a multithread error if an exception was raised by any thread.
		*/
		void reRaiseIfThreadCrash();
		// DocString: FMTExceptionHandler::enableNestedExceptions
		/**
		@brief Enable the throwing of nested exceptions.
		*/
		void enableNestedExceptions();
		// DocString: FMTExceptionHandler::disableNestedExceptions
		/**
		@brief Disable the throwing of nested exceptions.
		*/
		void disableNestedExceptions();
		// DocString: setErrorsToWarnings()
		/**
		@brief Set a list of errors to be cast to warnings.
		@param[in] errors the errors to treat as warnings.
		*/
		void setErrorsToWarnings(const std::vector<Exception::FMTexc>& errors);
		// DocString: setMaxWarningsBeforeSilenced()
		/**
		@brief Setter for the maximum number of warnings before they are silenced.
		@param[in] maxwarningcount the maximum warning count.
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
		@brief Translate a Win32 structural exception to a C++ exception.
		@param[in] p_u the structural exception id.
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
		// DocString: FMTExceptionHandler::m_maxwarningsbeforesilenced
		///Number of time that the handler throw a certain warning before stop throwing it.
		size_t m_maxwarningsbeforesilenced;
		// DocString: FMTExceptionHandler::_logger
		///pointer to the logger used to print the warning / error.
		Logging::FMTLogger* _logger;
		// DocString: FMTExceptionHandler::m_usenestedexceptions
		///If usenested = true then the handler will throw nested exceptions
		bool m_usenestedexceptions;
		// DocString: FMTExceptionHandler::m_mtx
		///Mutex for multi-threading.
		//mutable std::recursive_mutex mtx;
		mutable boost::recursive_mutex m_mtx;
		// DocString: FMTExceptionHandler::m_errorstowarnings
		///If an error is in this list it's going to processed like a warning.
		std::vector<Exception::FMTexc>m_errorstowarnings;
		// DocString: FMTExceptionHandler::registred_threads
		///This is the level of the last FMTException thrown by the FMTExceptionHandler.
		std::set<boost::thread::id> m_registered_threads;
		// DocString: FMTExceptionHandler::m_mainthreadid
		///Main thread id
		static boost::thread::id m_mainthreadid;
		// DocString: FMTExceptionHandler::m_crashedthreadid
		///The thread id of the crashed thread
		static boost::thread::id m_crashedthreadid;
		// DocString: FMTExceptionHandler::threadcrashexceptions
		///The exception throwed by the faulty thread...
		std::exception_ptr m_threadcrashexception;
		// DocString: FMTExceptionHandler::_updateStatus
		/**
		@brief Update the status of the handler, adding to the warning or error counts based on the exception type.
		@param[in] lexception the exception type.
		@param[in] message the message of the exception.
		@return the updated status message.
		*/
		std::string _updateStatus(const FMTexc lexception, const std::string message);
		// DocString: FMTExceptionHandler::_needToRethrow
		/**
		@brief Return true if nested exceptions are used and the exception is a function error that must be rethrown.
		@return true if the exception must be rethrown else false.
		*/
		bool _needToRethrow() const;
		// DocString: FMTExceptionHandler::_isMainThread()
		/**
		@brief Return true if we are on the main thread.
		@return true if on the main thread else false.
		*/
		bool _isMainThread() const;
		// DocString: FMTExceptionHandler::_isThrowedOnThread()
		/**
		@brief Return true if an exception has been thrown on a thread.
		@return true if an exception has been thrown on a thread else false.
		*/
		bool _isThrowedOnThread() const;
		// DocString: FMTExceptionHandler::_isThisThreadThrowed()
		/**
		@brief Return true if this thread has thrown.
		@return true if this thread has thrown else false.
		*/
		bool _isThisThreadThrowed() const;
		// DocString: FMTExceptionHandler::registerthread
		/**
		@brief Register a worker thread so the handler knows it is dealing with a slave thread and not the main one.
		*/
		void _registerWorkerThread();
		// DocString: FMTExceptionHandler::isregistered
		/**
		@brief Return true if the thread is registered.
		@return true if the thread is registered else false.
		*/
		bool _isThreadRegistered() const;
		// DocString: FMTExceptionHandler::_gutsOfPrintExceptions
		/**
		@brief The guts of printExceptions.
		@param[in] text the message of the exception.
		@param[in] method the method where the exception occurred.
		@param[in] line the line where the exception occurred.
		@param[in] fil the file where the exception occurred.
		@param[in,out] levelreference the nesting level reference.
		@param[in] lsection the section in which the exception occurred.
		@param[in] logfirstlevel if true logs the first level.
		*/
		void _gutsOfPrintExceptions(std::string text,
			const std::string& method, const int& line, const std::string& fil,
			int& levelreference,Core::FMTsection lsection = Core::FMTsection::Empty,bool logfirstlevel = true);
		// DocString: FMTExceptionHandler::_gutsOfExceptionLog
		/**
		@brief The guts of logging exceptions.
		@param[in] texception the exception to log.
		@param[in] level the nesting level.
		*/
		void _gutsOfExceptionLog(const std::exception& texception,const int& level);
		// DocString: FMTExceptionHandler::_getLevel
		/**
		@brief Get the level of an exception.
		@param[in] p_exception the exception enum.
		@return the exception level.
		*/
		FMTlev _getLevel(const FMTexc p_exception) const;
	private:
		// DocString: FMTExceptionHandler::serialize
		/**
		@brief Serialize function used for serialization to do multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to serialize to or from.
		@param[in] version the serialization version.
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& BOOST_SERIALIZATION_NVP(_exception);
			ar& BOOST_SERIALIZATION_NVP(_logger);
			ar& BOOST_SERIALIZATION_NVP(m_usenestedexceptions);
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
