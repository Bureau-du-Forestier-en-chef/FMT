/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTlogger_Hm_included
#define FMTlogger_Hm_included
#include <boost/serialization/export.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <fstream>
#include <string>
#include <memory>
#include "FMTException.h"

namespace boost
{
	namespace serialization
	{
		class access;
	}
}


/// Namespace for the log management, provides different log handlers.
namespace Logging
{
	class FMTSolverLogger;
	//class CoinMessageHandler;
	// DocString: FMTLogger 
	/**
	FMTLogger is a base class who handle the level of stuff printed with FMT.
	If FMT is compiled with Osisolverinterface then this class is going to be derived from
	the Coinmessagehandler class to help handling the log level of the solvers.
	*/
	class FMTEXPORT FMTLogger
		{
		public:
			// DocString: FMTLogger()
			/**
			FMTLogger default constructor.
			*/
			FMTLogger();
			// DocString: ~FMTLogger()
			/**
			FMTLogger default destructor.
			*/
			virtual ~FMTLogger();
			// DocString: FMTLogger(const FMTLogger&)
			/**
			FMTLogger copy constructor.
			*/
			FMTLogger(const FMTLogger& rhs);
			// DocString: FMTLogger::redirectToFile
			/**
			Redirect the log information to a file.
			*/
			void redirectToFile(const std::string& filename, bool logStamp = true);
			// DocString: FMTLogger::closeFileStream
			/**
			Close the file stream if error occured.
			*/
			void closeFileStream();
			// DocString: FMTLogger::operator=
			/**
			FMTLogger copy assignment operator.
			*/
			FMTLogger& operator = (const FMTLogger& rhs);
			#ifdef FMTWITHOSI
				// DocString: FMTLogger::print
				/**
				FMTLogger print function if we are using Osisolverinterface the coinmessagehandler
				print function needs to be overloaded.
				*/
				virtual int print();
				// DocString: FMTLogger::checkSeverity
				/**
				FMTLogger check the severity of the message to be print by the coinmessagehandler base class.
				*/
				virtual void checkSeverity();
				// DocString: FMTLogger::clone
				/**
				Clone function needed for the usage of abstract 
				*/
				virtual FMTLogger* clone() const=0;
			#endif
			// DocString: FMTLogger::getLogStamp
			/**
			Return the basic logstamp has a string.
			*/
			virtual std::string getLogStamp() const;
			// DocString: FMTLogger::logStamp
			/**
			The logstramp function log information about the version of FMT and it's buildate.
			*/
			virtual void logStamp();
			// DocString: FMTLogger::logTime
			/**
			The logtime function log the actual time at which the function is called.
			*/
			virtual void logTime();
			// DocString: FMTLogger::setStreamFlush
			/**
			Force the flushing on file stream
			*/
			void setStreamFlush(bool flush);
			// DocString: FMTLogger::operator<<(const std::string& msg)
			/**
			This function is the main function to add up string.
			*/
			virtual FMTLogger& operator<<(const std::string& msg);
			// DocString: FMTLogger::operator<<(const int& msg)
			/**
			This function is the main function to add up int.
			*/
			virtual FMTLogger& operator<<(const int& msg);
			// DocString: FMTLogger::operator<<(const double& msg)
			/**
			This function is the main function to add up double.
			*/
			virtual FMTLogger& operator<<(const double& msg);
			// DocString: FMTLogger::operator<<(const float& msg)
			/**
			This function is the main function to add up float.
			*/
			virtual FMTLogger& operator<<(const float& msg);
			// DocString: FMTLogger::operator<<(const std::time_t& msg)
			/**
			This function is the main function to add up std::time_t.
			*/
			virtual FMTLogger& operator<<(const std::time_t& msg);
			// DocString: FMTLogger::operator<<(const size_t& msg)
			/**
			This function is the main function to add up size_t.
			*/
			virtual FMTLogger& operator<<(const size_t& msg);
			// DocString: FMTLogger::operator<<(const unsigned int& msg)
			/**
			This function is the main function to add up unsigned int.
			*/
			virtual FMTLogger& operator<<(const unsigned int& msg);
			// DocString: FMTLogger::operator<<(const void*& msg)
			/**
			This function is the main function to add up a void pointer to get its address
			*/
			virtual FMTLogger& operator<<(const void*& msg);
			// DocString: FMTLogger::logWithLevel
			/**
			@brief Log a message with a given message level if message level is greater of equal to the logger level
			then it will be printed
			@param[in] p_msg the message
			@param[in] p_messageLevel message level
			@return true if printed
			*/
			virtual bool logWithLevel(const std::string& p_msg, const int& p_messageLevel) const;
			#ifdef FMTWITHOSI
			// DocString: FMTLogger::getSolverLogger
			/**
			Return the ABSTRACT logger used by osisolverinterface.
			*/
			virtual FMTSolverLogger* getSolverLogger();
			#endif
			// DocString: FMTLogger::Clone
			/**
			@brief clone the FMTLogger
			@return a valid cloned FMTLogger
			*/
			virtual std::unique_ptr <FMTLogger> Clone() const = 0;
		protected:
			#if defined FMTWITHOSI
			std::unique_ptr<FMTSolverLogger>solverref;
			#endif
			// DocString: FMTLogger::pathtostream
			///string path the the potential filestream
			std::string filepath;
			// DocString: FMTLogger::m_FileStream
			///stream if the logger redirect the logging into somesort of file.
			mutable std::unique_ptr<std::ofstream>m_FileStream;
			// DocString: FMTLogger::mtx
			///Mutex for multi-threading.
			//mutable std::recursive_mutex mtx;
			mutable boost::recursive_mutex mtx;
			// DocString: FMTLogger::flushstream
			///If true will flush stream at each write
			bool flushstream;
			// DocString: FMTLogger::cout
			/**
			cout function of the logger sometimes on Windows if using boost::python the std::cout needs
			a little help to print directly into the python window.
			*/
			virtual void cout(const char* message) const;
			#ifdef FMTWITHOSI
			// DocString: FMTLogger::setLoggingLevel
			/**
			Set the solverlogger logging level
			*/
			virtual void setLoggingLevel(const int& level);
			#endif // FMTWITHOSI
		private:
			friend class boost::serialization::access;
			// DocString: FMTLogger::save
			/**
			Save function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
			*/
			template<class Archive>
			void save(Archive& ar, const unsigned int version) const;
			// DocString: FMTLogger::load
			/**
			Load function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
			*/
			template<class Archive>
			void load(Archive& ar, const unsigned int version);
			// DocString: FMTLogger::serialize
			/**
			Load function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
			*/
			template<class Archive>
			void serialize(Archive& ar, const unsigned int file_version);
			// DocString: FMTLogger::setToFile
			/**
			Redirect the log information to a file.
			*/
			void setToFile(const std::string& filename) const;
			
		};

}

BOOST_CLASS_EXPORT_KEY(Logging::FMTLogger)

#endif
