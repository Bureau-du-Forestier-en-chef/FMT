/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTlogger_H_INCLUDED
#define FMTlogger_H_INCLUDED
#include <boost/serialization/export.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <fstream>
#include <string>
#include "FMTexception.h"

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
	class FMTsolverlogger;
	//class CoinMessageHandler;
	// DocString: FMTlogger 
	/**
	FMTlogger is a base class who handle the level of stuff printed with FMT.
	If FMT is compiled with Osisolverinterface then this class is going to be derived from
	the Coinmessagehandler class to help handling the log level of the solvers.
	*/
	class FMTEXPORT FMTlogger
		{
		friend class boost::serialization::access;
		// DocString: FMTlogger::save
		/**
		Save function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		template<class Archive>
		void save(Archive& ar, const unsigned int version) const;
		// DocString: FMTlogger::load
		/**
		Load function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		template<class Archive>
		void load(Archive& ar, const unsigned int version);
		// DocString: FMTlogger::serialize
		/**
		Load function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		template<class Archive>
		void serialize(Archive &ar, const unsigned int file_version);
		// DocString: FMTlogger::settofile
		/**
		Redirect the log information to a file.
		*/
		void settofile(const std::string& filename) const;
		protected:
		#if defined FMTWITHOSI
			std::unique_ptr<FMTsolverlogger>solverref;
		#endif
			// DocString: FMTlogger::pathtostream
			///string path the the potential filestream
			std::string filepath;
			// DocString: FMTlogger::filestream
			///stream if the logger redirect the logging into somesort of file.
			mutable std::ofstream* filestream;
			// DocString: FMTlogger::mtx
			///Mutex for multi-threading.
			//mutable std::recursive_mutex mtx;
			mutable boost::recursive_mutex mtx;
			// DocString: FMTlogger::flushstream
			///If true will flush stream at each write
			bool flushstream;
			// DocString: FMTlogger::cout
			/**
			cout function of the logger sometimes on Windows if using boost::python the std::cout needs
			a little help to print directly into the python window.
			*/
			virtual void cout(const char* message) const;
			#ifdef FMTWITHOSI
				// DocString: FMTlogger::setlogginglevel
				/**
				Set the solverlogger logging level
				*/
				virtual void setlogginglevel(const int& level);
			#endif // FMTWITHOSI
		public:
			// DocString: FMTlogger()
			/**
			FMTlogger default constructor.
			*/
			FMTlogger();
			// DocString: ~FMTlogger()
			/**
			FMTlogger default destructor.
			*/
			virtual ~FMTlogger();
			// DocString: FMTlogger(const FMTlogger&)
			/**
			FMTlogger copy constructor.
			*/
			FMTlogger(const FMTlogger& rhs);
			// DocString: FMTlogger::redirectofile
			/**
			Redirect the log information to a file.
			*/
			void redirectofile(const std::string& filename);
			// DocString: FMTlogger::closefilestream
			/**
			Close the file stream if error occured.
			*/
			void closefilestream();
			// DocString: FMTlogger::operator=
			/**
			FMTlogger copy assignment operator.
			*/
			FMTlogger& operator = (const FMTlogger& rhs);
			#ifdef FMTWITHOSI
				// DocString: FMTlogger::print
				/**
				FMTlogger print function if we are using Osisolverinterface the coinmessagehandler
				print function needs to be overloaded.
				*/
				virtual int print();
				// DocString: FMTlogger::checkSeverity
				/**
				FMTlogger check the severity of the message to be print by the coinmessagehandler base class.
				*/
				virtual void checkSeverity();
				// DocString: FMTlogger::clone
				/**
				Clone function needed for the usage of abstract 
				*/
				virtual FMTlogger* clone() const=0;
			#endif
			// DocString: FMTlogger::getlogstamp
			/**
			Return the basic logstamp has a string.
			*/
			virtual std::string getlogstamp() const;
			// DocString: FMTlogger::logstamp
			/**
			The logstramp function log information about the version of FMT and it's buildate.
			*/
			virtual void logstamp();
			// DocString: FMTlogger::logtime
			/**
			The logtime function log the actual time at which the function is called.
			*/
			virtual void logtime();
			// DocString: FMTlogger::setstreamflush
			/**
			Force the flushing on file stream
			*/
			void setstreamflush(bool flush);
			// DocString: FMTlogger::operator<<(const std::string& msg)
			/**
			This function is the main function to add up string.
			*/
			virtual FMTlogger& operator<<(const std::string& msg);
			// DocString: FMTlogger::operator<<(const int& msg)
			/**
			This function is the main function to add up int.
			*/
			virtual FMTlogger& operator<<(const int& msg);
			// DocString: FMTlogger::operator<<(const double& msg)
			/**
			This function is the main function to add up double.
			*/
			virtual FMTlogger& operator<<(const double& msg);
			// DocString: FMTlogger::operator<<(const float& msg)
			/**
			This function is the main function to add up float.
			*/
			virtual FMTlogger& operator<<(const float& msg);
			// DocString: FMTlogger::operator<<(const std::time_t& msg)
			/**
			This function is the main function to add up std::time_t.
			*/
			virtual FMTlogger& operator<<(const std::time_t& msg);
			// DocString: FMTlogger::operator<<(const size_t& msg)
			/**
			This function is the main function to add up size_t.
			*/
			virtual FMTlogger& operator<<(const size_t& msg);
			// DocString: FMTlogger::operator<<(const unsigned int& msg)
			/**
			This function is the main function to add up unsigned int.
			*/
			virtual FMTlogger& operator<<(const unsigned int& msg);
			// DocString: FMTlogger::operator<<(const void*& msg)
			/**
			This function is the main function to add up a void pointer to get its address
			*/
			virtual FMTlogger& operator<<(const void*& msg);
			// DocString: FMTlogger::logwithlevel
			/**
			Log a message with a given message level if message level is greater of equal to the logger level
			then it will be printed
			*/
			virtual bool logwithlevel(const std::string &msg, const int& messagelevel) const;
			#ifdef FMTWITHOSI
			// DocString: FMTlogger::getsolverlogger
			/**
			Return the ABSTRACT logger used by osisolverinterface.
			*/
			virtual FMTsolverlogger* getsolverlogger();
			#endif
			// DocString: FMTlogger::Clone
			/**
			@brief clone the FMTlogger
			@return a valid cloned FMTlogger
			*/
			virtual std::unique_ptr <FMTlogger> Clone() const = 0;
			
		};

}

BOOST_CLASS_EXPORT_KEY(Logging::FMTlogger)

#endif
