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
#include "FMTLogger.h"
#if defined FMTWITHPYTHON
	#include <boost/python.hpp>
#endif // defined FMTWITHPYTHON
#if defined FMTWITHR
	#include <Rcpp.h>
#endif
#include <iostream>
#include "FMTVersion.h"
#if defined FMTWITHOSI
#include "FMTSolverLogger.h"
#include "CoinMessageHandler.hpp"
#include "FMTError.h"
#include "FMTTask.h"
#endif


namespace Logging
{

	template<class Archive>
	void FMTLogger::save(Archive& ar, const unsigned int version) const
	{
	#ifdef FMTWITHOSI
		const int logl = solverref->logLevel();
		ar & BOOST_SERIALIZATION_NVP(logl);
	#endif
		ar & filepath;
	}

	template<class Archive>
	void FMTLogger::load(Archive& ar, const unsigned int version)
	{
		#ifdef FMTWITHOSI
		int coinloglevel = 0;
		ar & BOOST_SERIALIZATION_NVP(coinloglevel);
		solverref->setLogLevel(coinloglevel);
		#endif
		ar & filepath;
		if (!filepath.empty())
		{
			setToFile(filepath);
		}
	}

	template<class Archive>
	void FMTLogger::serialize(Archive &ar, const unsigned int file_version)
	{
		boost::serialization::split_member(ar, *this, file_version);
	}



	void FMTLogger::setToFile(const std::string& filename) const
		{
		if (m_FileStream !=nullptr)
			{
			m_FileStream->close();
			}
		if (!filename.empty())
			{
			m_FileStream = std::unique_ptr<std::ofstream>(new std::ofstream(filename,std::ios_base::app));
			}
		
		}

	FMTLogger::FMTLogger() : 
#if defined FMTWITHOSI
		solverref(new FMTSolverLogger(*this)),
#endif
		filepath(), m_FileStream(), mtx(),flushstream(false)
		{
		

		}

	void FMTLogger::redirectToFile(const std::string& filename, bool logStamp)
	{
		boost::lock_guard<boost::recursive_mutex> guard(mtx);
		filepath = filename;
		setToFile(filepath);
		if (m_FileStream && m_FileStream->is_open() && logStamp)
		{
			this->logStamp();
			this->logTime();
		}
	}

	FMTLogger::FMTLogger(const FMTLogger& rhs):filepath(), m_FileStream(), mtx(), flushstream(false)
		{
		boost::lock_guard<boost::recursive_mutex> lock(rhs.mtx);
		filepath=rhs.filepath;
		setToFile(filepath);
		flushstream=rhs.flushstream;
		#if defined FMTWITHOSI
			solverref.reset(new FMTSolverLogger(*this));
			if (rhs.solverref)
				{
				solverref->setLogLevel(rhs.solverref->logLevel());
				}
			
		#endif
		}

	FMTLogger& FMTLogger::operator = (const FMTLogger& rhs)
		{
		if (this!=&rhs)
			{
			//std::lock(mtx, rhs.mtx);
			boost::lock(mtx, rhs.mtx);
			boost::lock_guard<boost::recursive_mutex> self_lock(mtx,boost::adopt_lock /*std::adopt_lock*/);
			boost::lock_guard<boost::recursive_mutex> other_lock(rhs.mtx,boost::adopt_lock /*std::adopt_lock*/);
			#if defined FMTWITHOSI
				solverref.reset(new FMTSolverLogger(*rhs.solverref));
				if (rhs.solverref)
					{
					solverref->setLogLevel(rhs.solverref->logLevel());
					}
			#endif
			filepath = rhs.filepath;
			setToFile(filepath);
			flushstream = rhs.flushstream;
			}
		return *this;
		}

	void FMTLogger::closeFileStream()
		{
		boost::lock_guard<boost::recursive_mutex> guard(mtx);
		if (m_FileStream && m_FileStream->is_open())
			{
			this->logTime();
			m_FileStream->close();
			filepath.clear();
			}
		}

	FMTLogger::~FMTLogger()
		{
		boost::lock_guard<boost::recursive_mutex> guard(mtx);
		if (m_FileStream && m_FileStream->is_open())
			{
			this->logTime();
			m_FileStream->close();
			m_FileStream = std::unique_ptr<std::ofstream>(nullptr);
			}
		}

	std::string FMTLogger::getLogStamp() const
	{
		const std::string message = "FMT " + Version::FMTVersion().getVersion() +
			", build: " + Version::FMTVersion().getBuildDate();
		return message;
	}

	void FMTLogger::logStamp()
		{
		//boost::lock_guard<boost::recursive_mutex> guard(mtx);
		*this<< FMTLogger::getLogStamp() << "\n";
		}

	void FMTLogger::logTime()
		{
		//boost::lock_guard<boost::recursive_mutex> guard(mtx);
		const std::string message = Version::FMTVersion().getDatenow();
		*this << (message);
		}

	void FMTLogger::setStreamFlush(bool flush)
		{
		boost::lock_guard<boost::recursive_mutex> guard(mtx);
		flushstream = flush;
		}


	FMTLogger& FMTLogger::operator<<(const std::string& msg)
		{
		this->cout(msg.c_str());
		return *this;
		}

	FMTLogger& FMTLogger::operator<<(const int& msg)
	{
		const std::string value = std::to_string(msg);
		this->cout(value.c_str());
		return *this;
	}

	FMTLogger& FMTLogger::operator<<(const double& msg)
	{
		const std::string value = std::to_string(msg);
		this->cout(value.c_str());
		return *this;
	}
	FMTLogger& FMTLogger::operator<<(const float& msg)
	{
		const std::string value = std::to_string(msg);
		this->cout(value.c_str());
		return *this;
	}
	FMTLogger& FMTLogger::operator<<(const std::time_t& msg)
	{
		const std::string value = std::to_string(msg);
		this->cout(value.c_str());
		return *this;
	}
	FMTLogger& FMTLogger::operator<<(const size_t& msg)
	{
		const std::string value = std::to_string(msg);
		this->cout(value.c_str());
		return *this;
	}
	FMTLogger& FMTLogger::operator<<(const unsigned int& msg)
	{
		const std::string value = std::to_string(msg);
		this->cout(value.c_str());
		return *this;
	}

	FMTLogger& FMTLogger::operator<<(const void*& msg)
	{
		std::ostringstream ons;
		ons << std::hex << msg;
		const std::string value = ons.str();
		this->cout(value.c_str());
		return *this;
	}

	bool FMTLogger::logWithLevel(const std::string& p_msg, const int& p_messageLevel) const
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
		void FMTLogger::checkSeverity()
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
	
		/*FMTLogger* FMTLogger::clone() const
			{
			boost::lock_guard<boost::recursive_mutex> guard(mtx);
			return new FMTLogger(*this);
			}*/

	#endif

	void FMTLogger::cout(const char* message) const
		{
		boost::lock_guard<boost::recursive_mutex> guard(mtx);
		std::string thread = "Thread("+Parallel::FMTTask::getThreadId()+") ";
		if (message && message[0]=='\n')
			{
			thread.clear();
			}
		if (m_FileStream && m_FileStream->is_open())
			{
			(*m_FileStream)<< thread << (message);
			if (m_FileStream)
				{
				m_FileStream->flush();
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
		int FMTLogger::print()
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
					std::throw_with_nested(Exception::FMTError(
						Exception::FMTException(Exception::FMTexc::FMTcoinerror,Core::FMTsection::Empty,
									"Stopping due to previous errors.\n","FMTLogger::print",__FILE__, __LINE__)));
					}
				}
			return 0;
			}

		FMTSolverLogger* FMTLogger::getSolverLogger()
			{
			return solverref.get();
			}
		
		void FMTLogger::setLoggingLevel(const int& level)
		{
			solverref->setLogLevel(level);
		}

	#endif


}

BOOST_CLASS_EXPORT_IMPLEMENT(Logging::FMTLogger)


