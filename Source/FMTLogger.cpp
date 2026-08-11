/*
Copyright (c) 2019 Gouvernement du Qu�bec

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
#include "FMTCoinError.h"
#include "FMTTask.h"
#endif


namespace Logging
{

	template<class Archive>
	void FMTLogger::save(Archive& ar, const unsigned int version) const
	{
	#ifdef FMTWITHOSI
		const int logl = m_solverref->logLevel();
		ar & BOOST_SERIALIZATION_NVP(logl);
	#endif
		ar & m_filepath;
	}

	template<class Archive>
	void FMTLogger::load(Archive& ar, const unsigned int version)
	{
		#ifdef FMTWITHOSI
		int coinloglevel = 0;
		ar & BOOST_SERIALIZATION_NVP(coinloglevel);
		m_solverref->setLogLevel(coinloglevel);
		#endif
		ar & m_filepath;
		if (!m_filepath.empty())
		{
			_setToFile(m_filepath);
		}
	}

	template<class Archive>
	void FMTLogger::serialize(Archive &ar, const unsigned int file_version)
	{
		boost::serialization::split_member(ar, *this, file_version);
	}



	void FMTLogger::_setToFile(const std::string& filename) const
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
		m_solverref(new FMTSolverLogger(*this)),
#endif
		m_filepath(), m_FileStream(), m_mtx(),m_flushstream(false)
		{
		

		}

	void FMTLogger::redirectToFile(const std::string& filename, bool logStamp)
	{
		boost::lock_guard<boost::recursive_mutex> guard(m_mtx);
		m_filepath = filename;
		_setToFile(m_filepath);
		if (m_FileStream && m_FileStream->is_open() && logStamp)
		{
			this->logStamp();
			this->logTime();
		}
	}

	FMTLogger::FMTLogger(const FMTLogger& rhs):m_filepath(), m_FileStream(), m_mtx(), m_flushstream(false)
		{
		boost::lock_guard<boost::recursive_mutex> lock(rhs.m_mtx);
		m_filepath=rhs.m_filepath;
		_setToFile(m_filepath);
		m_flushstream=rhs.m_flushstream;
		#if defined FMTWITHOSI
			m_solverref.reset(new FMTSolverLogger(*this));
			if (rhs.m_solverref)
				{
				m_solverref->setLogLevel(rhs.m_solverref->logLevel());
				}
			
		#endif
		}

	FMTLogger& FMTLogger::operator = (const FMTLogger& rhs)
		{
		if (this!=&rhs)
			{
			//std::lock(mtx, rhs.mtx);
			boost::lock(m_mtx, rhs.m_mtx);
			boost::lock_guard<boost::recursive_mutex> self_lock(m_mtx,boost::adopt_lock /*std::adopt_lock*/);
			boost::lock_guard<boost::recursive_mutex> other_lock(rhs.m_mtx,boost::adopt_lock /*std::adopt_lock*/);
			#if defined FMTWITHOSI
				m_solverref.reset(new FMTSolverLogger(*rhs.m_solverref));
				if (rhs.m_solverref)
					{
					m_solverref->setLogLevel(rhs.m_solverref->logLevel());
					}
			#endif
			m_filepath = rhs.m_filepath;
			_setToFile(m_filepath);
			m_flushstream = rhs.m_flushstream;
			}
		return *this;
		}

	void FMTLogger::closeFileStream()
		{
		boost::lock_guard<boost::recursive_mutex> guard(m_mtx);
		if (m_FileStream && m_FileStream->is_open())
			{
			this->logTime();
			m_FileStream->close();
			m_filepath.clear();
			}
		}

	FMTLogger::~FMTLogger()
		{
		boost::lock_guard<boost::recursive_mutex> guard(m_mtx);
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
		const std::string message = Version::FMTVersion().getDateNow();
		*this << (message);
		}

	void FMTLogger::setStreamFlush(bool flush)
		{
		boost::lock_guard<boost::recursive_mutex> guard(m_mtx);
		m_flushstream = flush;
		}


	FMTLogger& FMTLogger::operator<<(const std::string& msg)
		{
		this->_cout(msg.c_str());
		return *this;
		}

	FMTLogger& FMTLogger::operator<<(const int& msg)
	{
		const std::string value = std::to_string(msg);
		this->_cout(value.c_str());
		return *this;
	}

	FMTLogger& FMTLogger::operator<<(const double& msg)
	{
		const std::string value = std::to_string(msg);
		this->_cout(value.c_str());
		return *this;
	}
	FMTLogger& FMTLogger::operator<<(const float& msg)
	{
		const std::string value = std::to_string(msg);
		this->_cout(value.c_str());
		return *this;
	}
	FMTLogger& FMTLogger::operator<<(const std::time_t& msg)
	{
		const std::string value = std::to_string(msg);
		this->_cout(value.c_str());
		return *this;
	}
	FMTLogger& FMTLogger::operator<<(const size_t& msg)
	{
		const std::string value = std::to_string(msg);
		this->_cout(value.c_str());
		return *this;
	}
	FMTLogger& FMTLogger::operator<<(const unsigned int& msg)
	{
		const std::string value = std::to_string(msg);
		this->_cout(value.c_str());
		return *this;
	}

	FMTLogger& FMTLogger::operator<<(const void*& msg)
	{
		std::ostringstream ons;
		ons << std::hex << msg;
		const std::string value = ons.str();
		this->_cout(value.c_str());
		return *this;
	}

	bool FMTLogger::logWithLevel(const std::string& p_msg, const int& p_messageLevel) const
	{
		#ifdef FMTWITHOSI
		if (m_solverref->logLevel() < p_messageLevel)
				{
				return false;
				}
		#endif
		
	this->_cout(p_msg.c_str());
	return true;
	}


	#ifdef FMTWITHOSI
		void FMTLogger::checkSeverity()
			{
			#ifdef FMTWITHOSI
			if (m_solverref->logLevel() == 0)//
				{
				return;
				}
			#endif
			boost::lock_guard<boost::recursive_mutex> guard(m_mtx);
			m_solverref->checkcoinSeverity();
			}
	
		/*FMTLogger* FMTLogger::clone() const
			{
			boost::lock_guard<boost::recursive_mutex> guard(m_mtx);
			return new FMTLogger(*this);
			}*/

	#endif

	void FMTLogger::_cout(const char* message) const
		{
		boost::lock_guard<boost::recursive_mutex> guard(m_mtx);
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
			if (m_solverref->logLevel()  == 0)//
				{
				return 0;
				}
			#endif
			boost::lock_guard<boost::recursive_mutex> guard(m_mtx);
			if (m_solverref->messageOut_ > m_solverref->messageBuffer())
				{
				char buffer[COIN_MESSAGE_HANDLER_MAX_BUFFER_SIZE];
				snprintf(buffer, sizeof(buffer), "%s\n", m_solverref->messageBuffer());
				this->_cout(buffer);
				if (m_solverref->currentMessage().severity_ == 'S')
					{
					fprintf(m_solverref->filePointer(), "Stopping due to previous errors.\n");
					//Should do walkback
					//abort();
					std::throw_with_nested(Exception::FMTCoinError(Core::FMTsection::Empty,
									"Stopping due to previous errors.\n","FMTLogger::print",__FILE__, __LINE__));
					}
				}
			return 0;
			}

		FMTSolverLogger* FMTLogger::getSolverLogger()
			{
			return m_solverref.get();
			}
		
		void FMTLogger::_setLoggingLevel(const int& level)
		{
			m_solverref->setLogLevel(level);
		}

	#endif


}

BOOST_CLASS_EXPORT_IMPLEMENT(Logging::FMTLogger)


