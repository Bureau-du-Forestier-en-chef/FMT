/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTexceptionhandler_H_INCLUDED
#define FMTexceptionhandler_H_INCLUDED

#include "FMTexception.h"
#include "FMTerror.h"
#include "FMTwarning.h"
#include "FMTlogger.h"
#if defined FMTWITHGDAL
	#include "cpl_error.h"
#endif
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include <memory>

namespace Exception
{
#if defined FMTWITHGDAL
	// DocString: FMTCPLErrorHandler()
	/**
	This function is used by has a callback in GDAL for handling GDAL errors.
	*/
	void CPL_STDCALL FMTCPLErrorHandler(CPLErr eErrClass, CPLErrorNum nError, const char * pszErrorMsg);
#endif
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
		// DocString: FMTexceptionhandler::_logger
		///pointer to the logger used to print the warning / error.
		std::shared_ptr<Logging::FMTlogger>_logger;
		// DocString: FMTexceptionhandler::usenestedexceptions
		///If usenested = true then the handler will throw nested exceptions
		bool usenestedexceptions;
		// DocString: FMTexceptionhandler::cplhandlerpushed
		///If CPL handler pushed true else false
		bool cplhandlerpushed;
		// DocString: FMTexceptionhandler::mtx
		///Mutex for multi-threading.
		//mutable std::recursive_mutex mtx;
		mutable boost::recursive_mutex mtx;
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
	public:
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
			virtual void handelCPLerror(CPLErr eErrClass, CPLErrorNum nError, const char * pszErrorMsg);
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
		void throw_nested(const std::exception& texception, int level = 0,bool rethrow=true);

		// DocString: FMTexceptionhandler::printexceptions
		/**
		Print all nested exception starting with the first provided by the parameters.
		*/
		void printexceptions(std::string text,
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
		FMTexception raisefromcatch(std::string text,
			const std::string& method, const int& line, const std::string& file,
			Core::FMTsection lsection = Core::FMTsection::Empty);

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
		
		#if defined FMTWITHGDAL
		// DocString: FMTexceptionhandler::isCPLpushed
		/**
		Check if CPL handler been pushed.
		*/
		inline bool isCPLpushed() const
			{
			return cplhandlerpushed;
			}
		// DocString: FMTexceptionhandler::setCPLpushed
		/**
		Set CPL pushed to true.
		*/
		void setCPLpushed();
		#endif
		
	};

}

BOOST_CLASS_EXPORT_KEY(Exception::FMTexceptionhandler)

#endif
