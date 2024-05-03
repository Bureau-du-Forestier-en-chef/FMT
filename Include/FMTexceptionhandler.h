/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTexceptionhandler_H_INCLUDED
#define FMTexceptionhandler_H_INCLUDED


#include "FMTlogger.h"
#include <unordered_map>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include <memory>
#include <set>

#include <boost/thread.hpp>

namespace Exception
{
	class FMTexception;
	// DocString: FMTexceptionhandler
	/**
	The FMTexceptionhandler is a base class used to handel error thrown in FMT. It keeps count of the
	number of warning thrown and error thrown it also has a shared pointer to an abstract FMTlogger.
	This class is used in the FMTobject class.
	*/
	class FMTEXPORT FMTexceptionhandler
	{
	// DocString: FMTexceptionhandler::serialize
	/**
	Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
		{
			ar & BOOST_SERIALIZATION_NVP(_level);
			ar & BOOST_SERIALIZATION_NVP(_exception);
			ar & BOOST_SERIALIZATION_NVP(_errorcount);
			ar & BOOST_SERIALIZATION_NVP(_warningcount);
			ar & BOOST_SERIALIZATION_NVP(_logger);
			ar & BOOST_SERIALIZATION_NVP(usenestedexceptions);
		}
	protected:
		// DocString: FMTexceptionhandler::_level
		///This is the level of the last FMTexception thrown by the FMTexceptionhandler.
		FMTlev _level;
		// DocString: FMTexceptionhandler::_exception
		///This is the type of the last FMTexception thrown by the FMTexceptionhandler.
		FMTexc _exception;
		// DocString: FMTexceptionhandler::_errorcount
		///Keeps count of the number of error thrown.
		int _errorcount;
		// DocString: FMTexceptionhandler::_warningcount
		///Keeps count of the number of warning thrown.
		int _warningcount;
		// DocString: FMTexceptionhandler::maxwarningsbeforesilenced
		///Number of time that the handler throw a certain warning before stop throwing it.
		size_t maxwarningsbeforesilenced;
		// DocString: FMTexceptionhandler::_logger
		///pointer to the logger used to print the warning / error.
		std::shared_ptr<Logging::FMTlogger>_logger;
		// DocString: FMTexceptionhandler::usenestedexceptions
		///If usenested = true then the handler will throw nested exceptions
		bool usenestedexceptions;
		// DocString: FMTexceptionhandler::mtx
		///Mutex for multi-threading.
		//mutable std::recursive_mutex mtx;
		mutable boost::recursive_mutex mtx;
		// DocString: FMTexceptionhandler::errorstowarnings
		///If an error is in this list it's going to processed like a warning.
		std::vector<Exception::FMTexc>errorstowarnings;
		// DocString: FMTexceptionhandler::registred_threads
		///This is the level of the last FMTexception thrown by the FMTexceptionhandler.
		std::set<boost::thread::id> registered_threads;
		// DocString: FMTexceptionhandler::mainthreadid
		///Main thread id
		static boost::thread::id mainthreadid;
		// DocString: FMTexceptionhandler::crashedthreadid
		///The thread id of the crashed thread
		static boost::thread::id crashedthreadid;
		// DocString: FMTexceptionhandler::threadcrashexceptions
		///The exception throwed by the faulty thread...
		std::exception_ptr threadcrashexception;
		// DocString: FMTexceptionhandler::updatestatus
		/**
		This functions updates the status of the handler adding up to the warning or the error counts.
		base on the exception type (lexception).
		*/
		std::string updatestatus(const FMTexc lexception, const std::string message);
		// DocString: FMTexceptionhandler::needtorethrow
		/**
		If usenested exceptions and exception == function error then it will be true and the exception will be rethrown.
		*/
		bool needtorethrow() const;
		// DocString: FMTexceptionhandler::ismainthread()
		/**
		Return true if we are on the main thread.
		*/
		bool ismainthread() const;
		// DocString: FMTexceptionhandler::isthrowedonthread()
		/**
		Return true if an exception been thrown on a thread.
		*/
		bool isthrowedonthread() const;
		// DocString: FMTexceptionhandler::isthisthreadthrowed()
		/**
		Return true if this thread throwed
		*/
		bool isthisthreadthrowed() const;
		// DocString: FMTexceptionhandler::registerthread
		/**
		Register a thread so that way the exceptionhandler knows that he is dealing with
		a slave thread that is not the main one.
		*/
		void registerworkerthread();
		// DocString: FMTexceptionhandler::isregistered
		/**
		Return true if the thread is registered.
		*/
		bool isthreadregistered() const;
		// DocString: FMTexceptionhandler::gutsofprintexceptions
		/**
		The guts of printexceptions.
		*/
		void gutsofprintexceptions(std::string text,
			const std::string& method, const int& line, const std::string& fil,
			int& levelreference,Core::FMTsection lsection = Core::FMTsection::Empty,bool logfirstlevel = true);
		// DocString: FMTexceptionhandler::gutsofexceptionlog
		/**
		The guts of logging exceptions...
		*/
		void gutsofexceptionlog(const std::exception& texception,const int& level);
	public:
		// DocString: FMTexceptionhandler::_specificwarningcount
		///Keeps count of the number of each type of warning thrown.
		std::unordered_map<int,size_t> _specificwarningcount;
		// DocString: checksignals()
		/**
		Check signals in R and Python, if we are on the main thread.
		*/
		void checksignals() const;
		// DocString: FMTexceptionhandler()
		/**
		Default constructor for FMTexceptionhandler.
		*/
		FMTexceptionhandler();
		// DocString: ~FMTexceptionhandler()
		/**
		Default virtual destructor for FMTexceptionhandler.
		*/
		virtual ~FMTexceptionhandler()=default;
		// DocString: FMTexceptionhandler(const FMTexceptionhandler&)
		/**
		Copy constructor for FMTexceptionhandler.
		*/
		FMTexceptionhandler(const FMTexceptionhandler& rhs);
		// DocString: FMTexceptionhandler(const std::shared_ptr<Logging::FMTlogger>&)
		/**
		Constructor with logger.
		*/
		FMTexceptionhandler(const std::shared_ptr<Logging::FMTlogger>& logger);
		// DocString: FMTexceptionhandler::passinlogger
		/**
		Pass a logger to the shared pointer of the FMTexceptionhandler class for sharing.
		*/
		void passinlogger(const std::shared_ptr<Logging::FMTlogger>& logger);
		
			// DocString: FMTexceptionhandler::getCPLdata
			/**
			When using GDAL you need this function for abstract usage in the FMTCPLErrorHandler function.
			it returns a copy of the abstract FMTexceptionhandler.
			*/
			virtual FMTexceptionhandler* getCPLdata();
		#if defined FMTWITHGDAL
			// DocString: FMTexceptionhandler::handelCPLerror
			/**
			Function called back by gdal for handling GDAL error thrown.
			*/
			virtual void handelCPLerror(int eErrClass, int nError, const char * pszErrorMsg);
		#endif
		// DocString: FMTexceptionhandler::operator=
		/**
		Default assignment operator for FMTexceptionhandler.
		*/
		FMTexceptionhandler& operator = (const FMTexceptionhandler& rhs);
		// DocString: FMTexceptionhandler::throw_nested
		/**
		This function is not used by FMT seems to be usefull for nested exception thrown.
		*/
		void throw_nested(const std::exception& texception, int& level,bool rethrow=true);
		// DocString: FMTexceptionhandler::printexceptions
		/**
		Print all nested exception starting with the first provided by the parameters.
		*/
		virtual void printexceptions(std::string text,
			const std::string& method, const int& line, const std::string& fil,
			Core::FMTsection lsection = Core::FMTsection::Empty);
		// DocString: FMTexceptionhandler::raise
		/**
		This function throw an FMTexception based on the exception type,section,text to write, line in the source code
		and file in the source code. Use also this function for warnings.
		*/
		virtual FMTexception raise(FMTexc lexception,std::string text,
			const std::string& method, const int& line, const std::string& file,
			Core::FMTsection lsection = Core::FMTsection::Empty ,bool throwit = true);
		// DocString: FMTexceptionhandler::raisefromcatch
		/**
		Raise an exception from the catch body it will determine if the exception is unenhdled.
		*/
		virtual FMTexception raisefromcatch(std::string text,
			const std::string& method, const int& line, const std::string& file,
			Core::FMTsection lsection = Core::FMTsection::Empty);
		// DocString: FMTexceptionhandler::raisefromthreadcatch
		/**
		Raise an exception from the catch body it will determine if the exception is unenhdled.
		Also catch all the exception to make sure their's no exceptions alive in the thread if not on the main thread.
		Then printalltheexceptions and return without throwing.
		*/
		void raisefromthreadcatch(std::string text,
			const std::string& method, const int& line, const std::string& file,
			Core::FMTsection lsection = Core::FMTsection::Empty);
		// DocString: FMTexceptionhandler::reraiseifthreadcrash
		/**
		If you have used threads make sure to use this to validate that there's no exception raised by any thread...
		if there's one it will raise a multithread error.
		*/
		void reraiseifthreadcrash();
		// DocString: FMTexceptionhandler::enablenestedexceptions
		/**
		Enable the nested exception throw.
		*/
		void enablenestedexceptions();
		// DocString: FMTexceptionhandler::disablenestedexceptions
		/**
		Disable the nested exception throw.
		*/
		void disablenestedexceptions();
		// DocString: seterrorstowarnings()
		/**
		Very hazardous function if you want to live dangerously you can
		set a vector of error to be cast to warnings...
		*/
		void seterrorstowarnings(const std::vector<Exception::FMTexc>& errors);
		// DocString: setmaxwarningsbeforesilenced()
		/**
		Settter for maxwarningsbeforesilenced.
		*/
		void setmaxwarningsbeforesilenced(const size_t& maxwarningcount);
	};

}

BOOST_CLASS_EXPORT_KEY(Exception::FMTexceptionhandler)

#endif
