/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTlogger_H_INCLUDED
#define FMTlogger_H_INCLUDED
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/lexical_cast.hpp>
#ifdef FMTWITHOSI
	#include <CoinMessageHandler.hpp>
#endif
#include <fstream>



namespace Logging
{
	// DocString: FMTlogger 
	/**
	FMTlogger is a base class who handle the level of stuff printed with FMT.
	If FMT is compiled with Osisolverinterface then this class is going to be derived from
	the Coinmessagehandler class to help handling the log level of the solvers.
	*/
	class FMTlogger 
		#ifdef FMTWITHOSI
			: public CoinMessageHandler
		#endif
		{
		
		friend class boost::serialization::access;
		// DocString: FMTlogger::save
		/**
		Save function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		template<class Archive>
		void save(Archive& ar, const unsigned int version) const
		{
			#ifdef FMTWITHOSI
				const int logl = this->logLevel();
				ar & BOOST_SERIALIZATION_NVP(logl);
			#endif
		}
		// DocString: FMTlogger::load
		/**
		Load function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		template<class Archive>
		void load(Archive& ar, const unsigned int version)
		{
			#ifdef FMTWITHOSI
				int coinloglevel = 0;
				ar & BOOST_SERIALIZATION_NVP(coinloglevel);
				this->setLogLevel(coinloglevel);
			#endif
		}
		BOOST_SERIALIZATION_SPLIT_MEMBER()
		protected:
			// DocString: FMTlogger::filestream
			///Pointer to a stream if the logger redirect the logging into somesort of file.
			std::ofstream* filestream;
			// DocString: FMTlogger::cout
			/**
			cout function of the logger sometimes on Windows if using boost::python the std::cout needs
			a little help to print directly into the python window.
			*/
			void cout(const char* message) const;
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
				int print() override;
				// DocString: FMTlogger::checkSeverity
				/**
				FMTlogger check the severity of the message to be print by the coinmessagehandler base class.
				*/
				void checkSeverity() override;
				// DocString: FMTlogger::clone
				/**
				Clone function needed for the usage of abstract coingmessagehandler class.
				*/
				CoinMessageHandler * clone() const override;
			#endif
			// DocString: FMTlogger::logstamp
			/**
			The logstramp function log information about the version of FMT and it's buildate.
			*/
			void logstamp();
			// DocString: FMTlogger::logtime
			/**
			The logtime function log the actual time at which the function is called.
			*/
			void logtime();
			// DocString: FMTlogger::settostream
			/**
			This function set a ofstream to the filestream object.
			*/
			void settostream(std::ofstream& stream);
			// DocString: FMTlogger::operator<<
			/**
			This function is the main function used for sending elements to print to the FMTlogger.
			*/
			template<class T>
			FMTlogger& operator<<(const T &msg)
				{
				const std::string value = boost::lexical_cast<std::string>(msg);
				this->cout(value.c_str());
				return *this;
				}
			// DocString: FMTlogger::logwithlevel
			/**
			Log a message with a given message level if message level is greater of equal to the logger level
			then it will be printed
			*/
			template<class T>
			void logwithlevel(const T &msg,const int& messagelevel)
				{
				#ifdef FMTWITHOSI
				if (this->logLevel() < messagelevel)
					{
					return;
					}
				#endif
				const std::string value = boost::lexical_cast<std::string>(msg);
				this->cout(value.c_str());
				}

		};

}
#endif
