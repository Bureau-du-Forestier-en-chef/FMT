/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTlogger.h"
#if defined FMTWITHPYTHON
	#include <boost/python.hpp>
#endif // defined FMTWITHPYTHON
#include <iostream>
#include "FMTversion.h"


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
		std::lock_guard<std::recursive_mutex> guard(mtx);
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
		std::lock_guard<std::recursive_mutex> lock(rhs.mtx);
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
			std::lock(mtx, rhs.mtx);
			std::lock_guard<std::recursive_mutex> self_lock(mtx, std::adopt_lock);
			std::lock_guard<std::recursive_mutex> other_lock(rhs.mtx, std::adopt_lock);
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
		std::lock_guard<std::recursive_mutex> guard(mtx);
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
		std::lock_guard<std::recursive_mutex> guard(mtx);
		const std::string message = "FMT " + Version::FMTversion().getversion() +
			", build: "+ Version::FMTversion().getbuilddate();
		*this<<(message)<< "\n";
		}

	void FMTlogger::logtime()
		{
		std::lock_guard<std::recursive_mutex> guard(mtx);
		const std::string message = Version::FMTversion().getdatenow();
		*this << (message);
		}

	void FMTlogger::setstreamflush(bool flush)
		{
		std::lock_guard<std::recursive_mutex> guard(mtx);
		flushstream = flush;
		}


	#ifdef FMTWITHOSI
		void FMTlogger::checkSeverity()
			{
			std::lock_guard<std::recursive_mutex> guard(mtx);
			CoinMessageHandler::checkSeverity();
			}
	
		CoinMessageHandler* FMTlogger::clone() const
			{
			std::lock_guard<std::recursive_mutex> guard(mtx);
			return new FMTlogger(*this);
			}

		CoinMessageHandler * FMTlogger::getpointer() const
			{
			std::lock_guard<std::recursive_mutex> guard(mtx);
			return dynamic_cast<CoinMessageHandler*>(const_cast<FMTlogger*>(this));
			}

	#endif

	void FMTlogger::cout(const char* message) const
		{
		std::lock_guard<std::recursive_mutex> guard(mtx);
		if (filestream && filestream->is_open())
			{
			(*filestream)<<(message);
			if (flushstream)
				{
				filestream->flush();
				}
		}else {
			#if defined(FMTWITHPYTHON)
					PySys_WriteStdout(message);
			#else
					std::cout << message << std::flush;
			#endif
			}
		}
	#ifdef FMTWITHOSI
		int FMTlogger::print()
			{
			std::lock_guard<std::recursive_mutex> guard(mtx);
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


