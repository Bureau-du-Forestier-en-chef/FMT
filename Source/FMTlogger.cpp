/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTlogger.hpp"
#if defined FMTWITHPYTHON
	#include <boost/python.hpp>
#endif // defined FMTWITHPYTHON
#include <iostream>
#include "FMTversion.hpp"


namespace Logging
{



	void FMTlogger::settofile(const std::string& filename) const
		{
		if (filestream!=nullptr)
			{
			filestream->close();
			delete filestream;
			}
		filestream = new std::ofstream(filename);
		}

	FMTlogger::FMTlogger() : 
		#ifdef FMTWITHOSI
			CoinMessageHandler(),
		#endif
		filepath(),filestream(), mtx(),flushstream(false)
		{

		}

	void FMTlogger::redirectofile(const std::string& filename)
		{
		boost::lock_guard<boost::recursive_mutex> guard(mtx);
		filepath = filename;
		settofile(filepath);
		if (filestream && filestream->is_open())
			{
			this->logstamp();
			this->logtime();
			}
		}

	FMTlogger::FMTlogger(const FMTlogger& rhs)
		{
		boost::lock_guard<boost::recursive_mutex> lock(rhs.mtx);
		#ifdef FMTWITHOSI
			CoinMessageHandler::operator=(rhs),
		#endif
		filepath=rhs.filepath;
		settofile(filepath);
		flushstream=rhs.flushstream;
		}

	FMTlogger& FMTlogger::operator = (const FMTlogger& rhs)
		{
		if (this!=&rhs)
			{
			//std::lock(mtx, rhs.mtx);
			boost::lock(mtx, rhs.mtx);
			boost::lock_guard<boost::recursive_mutex> self_lock(mtx,boost::adopt_lock /*std::adopt_lock*/);
			boost::lock_guard<boost::recursive_mutex> other_lock(rhs.mtx,boost::adopt_lock /*std::adopt_lock*/);
			#ifdef FMTWITHOSI
				CoinMessageHandler::operator=(rhs),
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

	void FMTlogger::logstamp()
		{
		boost::lock_guard<boost::recursive_mutex> guard(mtx);
		const std::string message = "FMT " + Version::FMTversion().getversion() +
			", build: "+ Version::FMTversion().getbuilddate();
		*this<<(message)<< "\n";
		}

	void FMTlogger::logtime()
		{
		boost::lock_guard<boost::recursive_mutex> guard(mtx);
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

	bool FMTlogger::logwithlevel(const std::string &msg, const int& messagelevel) const
	{
		#ifdef FMTWITHOSI
			if (this->logLevel() < messagelevel)
				{
				return false;
				}
		#endif
	this->cout(msg.c_str());
	return true;
	}


	#ifdef FMTWITHOSI
		void FMTlogger::checkSeverity()
			{
			boost::lock_guard<boost::recursive_mutex> guard(mtx);
			CoinMessageHandler::checkSeverity();
			}
	
		CoinMessageHandler* FMTlogger::clone() const
			{
			boost::lock_guard<boost::recursive_mutex> guard(mtx);
			return new FMTlogger(*this);
			}

		CoinMessageHandler * FMTlogger::getpointer() const
			{
			boost::lock_guard<boost::recursive_mutex> guard(mtx);
			return dynamic_cast<CoinMessageHandler*>(const_cast<FMTlogger*>(this));
			}

	#endif

	void FMTlogger::cout(const char* message) const
		{
		boost::lock_guard<boost::recursive_mutex> guard(mtx);
		if (filestream && filestream->is_open())
			{
			(*filestream)<<(message);
			if (flushstream)
				{
				filestream->flush();
				}
		}else {
			#if defined(FMTWITHPYTHON)
					PySys_FormatStdout(message);
			#else
					std::cout << message << std::flush;
			#endif
			}
		}
	#ifdef FMTWITHOSI
		int FMTlogger::print()
			{
			boost::lock_guard<boost::recursive_mutex> guard(mtx);
			if (messageOut_ > messageBuffer_)
				{
				char buffer[COIN_MESSAGE_HANDLER_MAX_BUFFER_SIZE];
				snprintf(buffer, sizeof(buffer), "%s\n", this->messageBuffer_);
				this->cout(buffer);
				if (currentMessage_.severity_ == 'S')
					{
					fprintf(fp_, "Stopping due to previous errors.\n");
					//Should do walkback
					abort();
					}
				}
			return 0;
			}
	#endif


}

BOOST_CLASS_EXPORT_IMPLEMENT(Logging::FMTlogger)


