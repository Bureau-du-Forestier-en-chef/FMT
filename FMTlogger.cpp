/*
MIT License

Copyright (c) [2019] [Bureau du forestier en chef]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "FMTlogger.h"
#include <boost/python.hpp>
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

	FMTlogger::FMTlogger() : filestream()
		{

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
		const std::string message = "FMT" + Version::FMTversion().getversion() +
			", build: "+ Version::FMTversion().getbuilddate();
		*this<<(message)<< "\n";
		}

	void FMTlogger::logtime()
		{
		const std::string message = Version::FMTversion().getdatenow();
		*this << (message) << "\n";
		}

	void FMTlogger::checkSeverity()
		{
		CoinMessageHandler::checkSeverity();
		}

	CoinMessageHandler* FMTlogger::clone() const
		{
		return new FMTlogger(*this);
		}

	void FMTlogger::cout(const char* message) const
		{
		#if defined(FMTPY)
				PySys_WriteStdout(message);
		#else
				std::cout << message << std::flush;
		#endif
		if (filestream && filestream->is_open())
			{
			filestream->operator<<(message);
			}
		}

	int FMTlogger::print()
		{
		if (messageOut_ > messageBuffer_) 
			{
			*messageOut_ = 0;
			//take off trailing spaces and commas
			messageOut_--;
			while (messageOut_ >= messageBuffer_)
				{
				if (*messageOut_ == ' ' || *messageOut_ == ',') 
					{
					*messageOut_ = 0;
					messageOut_--;
					}else {
						break;
						}	
				}
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

}


