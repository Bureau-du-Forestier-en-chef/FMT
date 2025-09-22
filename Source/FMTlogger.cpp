/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>
#include <sstream>
#include "FMTlogger.h"
#if defined FMTWITHPYTHON
	#include <boost/python.hpp>
#endif // defined FMTWITHPYTHON
#if defined FMTWITHR
	#include <Rcpp.h>
#endif
#include <iostream>
#include "FMTversion.h"
#if defined FMTWITHOSI
#include "FMTsolverlogger.h"
#include "CoinMessageHandler.hpp"
#include "FMTerror.h"
#include "FMTtask.h"
#endif


namespace Logging
{

	template<class Archive>
	void FMTlogger::save(Archive& ar, const unsigned int version) const
	{
	#ifdef FMTWITHOSI
		const int logl = solverref->logLevel();
		ar & BOOST_SERIALIZATION_NVP(logl);
	#endif
		ar & filepath;
	}

	template<class Archive>
	void FMTlogger::load(Archive& ar, const unsigned int version)
	{
		#ifdef FMTWITHOSI
		int coinloglevel = 0;
		ar & BOOST_SERIALIZATION_NVP(coinloglevel);
		solverref->setLogLevel(coinloglevel);
		#endif
		ar & filepath;
		if (!filepath.empty())
		{
			settofile(filepath);
		}
	}

	template<class Archive>
	void FMTlogger::serialize(Archive &ar, const unsigned int file_version)
	{
		boost::serialization::split_member(ar, *this, file_version);
	}



	void FMTlogger::settofile(const std::string& filename) const
		{
		if (filestream!=nullptr)
			{
			filestream->close();
			delete filestream;
			}
		if (!filename.empty())
			{
			filestream = new std::ofstream(filename);
			}
		
		}

	FMTlogger::FMTlogger() : 
#if defined FMTWITHOSI
		solverref(new FMTsolverlogger(*this)),
#endif
		filepath(),filestream(), mtx(),flushstream(false)
		{
		

		}

	void FMTlogger::redirectofile(const std::string& filename, bool logstamp)
	{
		boost::lock_guard<boost::recursive_mutex> guard(mtx);
		filepath = filename;
		settofile(filepath);
		if (filestream && filestream->is_open() && logstamp)
		{
			this->logstamp();
			this->logtime();
		}
	}

	FMTlogger::FMTlogger(const FMTlogger& rhs):filepath(), filestream(), mtx(), flushstream(false)
		{
		boost::lock_guard<boost::recursive_mutex> lock(rhs.mtx);
		filepath=rhs.filepath;
		settofile(filepath);
		flushstream=rhs.flushstream;
		#if defined FMTWITHOSI
			solverref.reset(new FMTsolverlogger(*this));
			if (rhs.solverref)
				{
				solverref->setLogLevel(rhs.solverref->logLevel());
				}
			
		#endif
		}

	FMTlogger& FMTlogger::operator = (const FMTlogger& rhs)
		{
		if (this!=&rhs)
			{
			//std::lock(mtx, rhs.mtx);
			boost::lock(mtx, rhs.mtx);
			boost::lock_guard<boost::recursive_mutex> self_lock(mtx,boost::adopt_lock /*std::adopt_lock*/);
			boost::lock_guard<boost::recursive_mutex> other_lock(rhs.mtx,boost::adopt_lock /*std::adopt_lock*/);
			#if defined FMTWITHOSI
				solverref.reset(new FMTsolverlogger(*rhs.solverref));
				if (rhs.solverref)
					{
					solverref->setLogLevel(rhs.solverref->logLevel());
					}
			#endif
			filepath = rhs.filepath;
			settofile(filepath);
			flushstream = rhs.flushstream;
			}
		return *this;
		}

	void FMTlogger::closefilestream()
		{
		boost::lock_guard<boost::recursive_mutex> guard(mtx);
		if (filestream && filestream->is_open())
			{
			this->logtime();
			filestream->close();
			filepath.clear();
			delete filestream;
			}
		}

	FMTlogger::~FMTlogger()
		{
		//this->logstamp();
		//this->logtime();
		if (filestream && filestream->is_open())
			{
			this->logtime();
			filestream->close();
			delete filestream;
			}
		}

	std::string FMTlogger::getlogstamp() const
	{
		const std::string message = "FMT " + Version::FMTversion().getversion() +
			", build: " + Version::FMTversion().getbuilddate();
		return message;
	}

	void FMTlogger::logstamp()
		{
		//boost::lock_guard<boost::recursive_mutex> guard(mtx);
		*this<< FMTlogger::getlogstamp() << "\n";
		}

	void FMTlogger::logtime()
		{
		//boost::lock_guard<boost::recursive_mutex> guard(mtx);
		const std::string message = Version::FMTversion().getdatenow();
		*this << (message);
		}

	void FMTlogger::setstreamflush(bool flush)
		{
		boost::lock_guard<boost::recursive_mutex> guard(mtx);
		flushstream = flush;
		}


	FMTlogger& FMTlogger::operator<<(const std::string& msg)
		{
		this->cout(msg.c_str());
		return *this;
		}

	FMTlogger& FMTlogger::operator<<(const int& msg)
	{
		const std::string value = std::to_string(msg);
		this->cout(value.c_str());
		return *this;
	}

	FMTlogger& FMTlogger::operator<<(const double& msg)
	{
		const std::string value = std::to_string(msg);
		this->cout(value.c_str());
		return *this;
	}
	FMTlogger& FMTlogger::operator<<(const float& msg)
	{
		const std::string value = std::to_string(msg);
		this->cout(value.c_str());
		return *this;
	}
	FMTlogger& FMTlogger::operator<<(const std::time_t& msg)
	{
		const std::string value = std::to_string(msg);
		this->cout(value.c_str());
		return *this;
	}
	FMTlogger& FMTlogger::operator<<(const size_t& msg)
	{
		const std::string value = std::to_string(msg);
		this->cout(value.c_str());
		return *this;
	}
	FMTlogger& FMTlogger::operator<<(const unsigned int& msg)
	{
		const std::string value = std::to_string(msg);
		this->cout(value.c_str());
		return *this;
	}

	FMTlogger& FMTlogger::operator<<(const void*& msg)
	{
		std::ostringstream ons;
		ons << std::hex << msg;
		const std::string value = ons.str();
		this->cout(value.c_str());
		return *this;
	}

	bool FMTlogger::logwithlevel(const std::string& p_msg, const int& p_messageLevel) const
	{
		#ifdef FMTWITHOSI
		if (solverref->logLevel() < p_messageLevel)
				{
				return false;
				}
		#endif
		
	this->cout(p_msg.c_str());
	return true;
	}


	#ifdef FMTWITHOSI
		void FMTlogger::checkSeverity()
			{
			#ifdef FMTWITHOSI
			if (solverref->logLevel() == 0)//
				{
				return;
				}
			#endif
			boost::lock_guard<boost::recursive_mutex> guard(mtx);
			solverref->checkcoinSeverity();
			}
	
		/*FMTlogger* FMTlogger::clone() const
			{
			boost::lock_guard<boost::recursive_mutex> guard(mtx);
			return new FMTlogger(*this);
			}*/

	#endif

	void FMTlogger::cout(const char* message) const
		{
		boost::lock_guard<boost::recursive_mutex> guard(mtx);
		std::string thread = "Thread("+Parallel::FMTtask::getThreadId()+") ";
		if (message && message[0]=='\n')
			{
			thread.clear();
			}
		if (filestream && filestream->is_open())
			{
			(*filestream)<< thread << (message);
			if (flushstream)
				{
				filestream->flush();
				}
		}else {
			#if defined(FMTWITHPYTHON)
					
			#if PY_MAJOR_VERSION < 3
			//old way maybe for python 2 ???? 
				PySys_FormatStdout(message);
			#else
				std::cout << thread << message << std::flush;
			#endif
			#elif defined(FMTWITHR)
					Rcpp::Rcout << thread << message << std::flush;
			#else
					std::cout << thread << message << std::flush;
			#endif
			}
		}
	#ifdef FMTWITHOSI
		int FMTlogger::print()
			{
			#ifdef FMTWITHOSI
			if (solverref->logLevel()  == 0)//
				{
				return 0;
				}
			#endif
			boost::lock_guard<boost::recursive_mutex> guard(mtx);
			if (solverref->messageOut_ > solverref->messageBuffer())
				{
				char buffer[COIN_MESSAGE_HANDLER_MAX_BUFFER_SIZE];
				snprintf(buffer, sizeof(buffer), "%s\n", solverref->messageBuffer());
				this->cout(buffer);
				if (solverref->currentMessage().severity_ == 'S')
					{
					fprintf(solverref->filePointer(), "Stopping due to previous errors.\n");
					//Should do walkback
					//abort();
					std::throw_with_nested(Exception::FMTerror(
						Exception::FMTexception(Exception::FMTexc::FMTcoinerror,Core::FMTsection::Empty,
									"Stopping due to previous errors.\n","FMTlogger::print",__FILE__, __LINE__)));
					}
				}
			return 0;
			}

		FMTsolverlogger* FMTlogger::getsolverlogger()
			{
			return solverref.get();
			}
		
		void FMTlogger::setlogginglevel(const int& level)
		{
			solverref->setLogLevel(level);
		}

	#endif


}

BOOST_CLASS_EXPORT_IMPLEMENT(Logging::FMTlogger)


