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
	@brief Base class handling the level of information printed by FMT.
	@details When FMT is compiled with the OSI solver interface this class derives from CoinMessageHandler to help handle the log level of the solvers.
	*/
	class FMTEXPORT FMTLogger
		{
		public:
			// DocString: FMTLogger()
			/**
			@brief Default constructor for FMTLogger.
			*/
			FMTLogger();
			// DocString: ~FMTLogger()
			/**
			@brief Default destructor for FMTLogger.
			*/
			virtual ~FMTLogger();
			// DocString: FMTLogger(const FMTLogger&)
			/**
			@brief Copy constructor for FMTLogger.
			@param[in] rhs the FMTLogger to copy.
			*/
			FMTLogger(const FMTLogger& rhs);
			// DocString: FMTLogger::redirectToFile
			/**
			@brief Redirect the log information to a file.
			@param[in] filename the file to redirect the log to.
			@param[in] logStamp if true writes a log stamp.
			*/
			void redirectToFile(const std::string& filename, bool logStamp = true);
			// DocString: FMTLogger::closeFileStream
			/**
			@brief Close the file stream.
			*/
			void closeFileStream();
			// DocString: FMTLogger::operator=
			/**
			@brief Copy assignment operator for FMTLogger.
			@param[in] rhs the FMTLogger to copy.
			@return a reference to this FMTLogger.
			*/
			FMTLogger& operator = (const FMTLogger& rhs);
			#ifdef FMTWITHOSI
				// DocString: FMTLogger::print
				/**
				@brief Print function overloaded from CoinMessageHandler when using the OSI solver interface.
				@return the value returned by the print function.
				*/
				virtual int print();
				// DocString: FMTLogger::checkSeverity
				/**
				@brief Check the severity of the message to be printed by the CoinMessageHandler base class.
				*/
				virtual void checkSeverity();
				// DocString: FMTLogger::clone
				/**
				@brief Clone the logger, needed for the abstract CoinMessageHandler usage.
				@return a pointer to the cloned logger.
				*/
				virtual FMTLogger* clone() const=0;
			#endif
			// DocString: FMTLogger::getLogStamp
			/**
			@brief Return the basic log stamp as a string.
			@return the log stamp.
			*/
			virtual std::string getLogStamp() const;
			// DocString: FMTLogger::logStamp
			/**
			@brief Log information about the version of FMT and its build date.
			*/
			virtual void logStamp();
			// DocString: FMTLogger::logTime
			/**
			@brief Log the actual time at which the function is called.
			*/
			virtual void logTime();
			// DocString: FMTLogger::setStreamFlush
			/**
			@brief Force the flushing of the file stream.
			@param[in] flush if true flushes the stream at each write.
			*/
			void setStreamFlush(bool flush);
			// DocString: FMTLogger::operator<<(const std::string& msg)
			/**
			@brief Append a string to the log.
			@param[in] msg the a string to append.
			@return a reference to this FMTLogger.
			*/
			virtual FMTLogger& operator<<(const std::string& msg);
			// DocString: FMTLogger::operator<<(const int& msg)
			/**
			@brief Append an int to the log.
			@param[in] msg the an int to append.
			@return a reference to this FMTLogger.
			*/
			virtual FMTLogger& operator<<(const int& msg);
			// DocString: FMTLogger::operator<<(const double& msg)
			/**
			@brief Append a double to the log.
			@param[in] msg the a double to append.
			@return a reference to this FMTLogger.
			*/
			virtual FMTLogger& operator<<(const double& msg);
			// DocString: FMTLogger::operator<<(const float& msg)
			/**
			@brief Append a float to the log.
			@param[in] msg the a float to append.
			@return a reference to this FMTLogger.
			*/
			virtual FMTLogger& operator<<(const float& msg);
			// DocString: FMTLogger::operator<<(const std::time_t& msg)
			/**
			@brief Append a std::time_t to the log.
			@param[in] msg the a std::time_t to append.
			@return a reference to this FMTLogger.
			*/
			virtual FMTLogger& operator<<(const std::time_t& msg);
			// DocString: FMTLogger::operator<<(const size_t& msg)
			/**
			@brief Append a size_t to the log.
			@param[in] msg the a size_t to append.
			@return a reference to this FMTLogger.
			*/
			virtual FMTLogger& operator<<(const size_t& msg);
			// DocString: FMTLogger::operator<<(const unsigned int& msg)
			/**
			@brief Append an unsigned int to the log.
			@param[in] msg the an unsigned int to append.
			@return a reference to this FMTLogger.
			*/
			virtual FMTLogger& operator<<(const unsigned int& msg);
			// DocString: FMTLogger::operator<<(const void*& msg)
			/**
			@brief Append the address of a void pointer to the log.
			@param[in] msg the void pointer whose address is appended.
			@return a reference to this FMTLogger.
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
			@brief Return the abstract logger used by the OSI solver interface.
			@return a pointer to the solver logger.
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
			std::unique_ptr<FMTSolverLogger>m_solverref;
			#endif
			// DocString: FMTLogger::pathtostream
			///string path the the potential filestream
			std::string m_filepath;
			// DocString: FMTLogger::m_FileStream
			///stream if the logger redirect the logging into somesort of file.
			mutable std::unique_ptr<std::ofstream>m_FileStream;
			// DocString: FMTLogger::m_mtx
			///Mutex for multi-threading.
			//mutable std::recursive_mutex mtx;
			mutable boost::recursive_mutex m_mtx;
			// DocString: FMTLogger::m_flushstream
			///If true will flush stream at each write
			bool m_flushstream;
			// DocString: FMTLogger::_cout
			/**
			@brief Output a message, sometimes needed on Windows with boost::python to print directly into the Python window.
			@param[in] message the message to output.
			*/
			virtual void _cout(const char* message) const;
			#ifdef FMTWITHOSI
			// DocString: FMTLogger::_setLoggingLevel
			/**
			@brief Set the solver logger logging level.
			@param[in] level the logging level to set.
			*/
			virtual void _setLoggingLevel(const int& level);
			#endif // FMTWITHOSI
		private:
			friend class boost::serialization::access;
			// DocString: FMTLogger::save
			/**
			@brief Save function used for serialization to do multiprocessing across multiple cpus (pickle in Python).
			@tparam Archive the archive type.
			@param[in,out] ar the archive to save to.
			@param[in] version the serialization version.
			*/
			template<class Archive>
			void save(Archive& ar, const unsigned int version) const;
			// DocString: FMTLogger::load
			/**
			@brief Load function used for serialization to do multiprocessing across multiple cpus (pickle in Python).
			@tparam Archive the archive type.
			@param[in,out] ar the archive to load from.
			@param[in] version the serialization version.
			*/
			template<class Archive>
			void load(Archive& ar, const unsigned int version);
			// DocString: FMTLogger::serialize
			/**
			@brief Serialize function used for serialization to do multiprocessing across multiple cpus (pickle in Python).
			@tparam Archive the archive type.
			@param[in,out] ar the archive to serialize to or from.
			@param[in] file_version the serialization version.
			*/
			template<class Archive>
			void serialize(Archive& ar, const unsigned int file_version);
			// DocString: FMTLogger::_setToFile
			/**
			@brief Redirect the log information to a file.
			@param[in] filename the file to redirect the log to.
			*/
			void _setToFile(const std::string& filename) const;
			
		};

}

BOOST_CLASS_EXPORT_KEY(Logging::FMTLogger)

#endif
