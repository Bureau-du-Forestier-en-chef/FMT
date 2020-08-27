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

	void FMTlogger::settostream(std::ofstream& stream)
		{
		filestream = &stream;
		if (filestream && filestream->is_open())
			{
			this->logstamp();
			this->logtime();
			}
		}

	FMTlogger::FMTlogger() : filestream(nullptr)
		{

		}

	FMTlogger::FMTlogger(const FMTlogger& rhs): filestream(rhs.filestream)
		{

		}

	FMTlogger& FMTlogger::operator = (const FMTlogger& rhs)
		{
		if (this!=&rhs)
			{
			filestream = rhs.filestream;
			}
		return *this;
		}

	FMTlogger::~FMTlogger()
		{
		if (filestream && filestream->is_open())
			{
			this->logtime();
			}
		}

	void FMTlogger::logstamp()
		{
		const std::string message = "FMT " + Version::FMTversion().getversion() +
			", build: "+ Version::FMTversion().getbuilddate();
		*this<<(message)<< "\n";
		}

	void FMTlogger::logtime()
		{
		const std::string message = Version::FMTversion().getdatenow();
		*this << (message) << "\n";
		}
	#ifdef FMTWITHOSI
		void FMTlogger::checkSeverity()
			{
			CoinMessageHandler::checkSeverity();
			}
	
		CoinMessageHandler* FMTlogger::clone() const
			{
			return new FMTlogger(*this);
			}
	#endif

	void FMTlogger::cout(const char* message) const
		{
		#if defined(FMTWITHPYTHON)
				PySys_WriteStdout(message);
		#else
				std::cout << message << std::flush;
		#endif
		/*if (filestream && filestream->is_open())
			{
			filestream->operator <<(message);
			}*/
		}
	#ifdef FMTWITHOSI
		int FMTlogger::print()
			{
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


