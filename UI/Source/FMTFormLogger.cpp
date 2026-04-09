#include "stdafx.h"
#include <msclr\marshal_cppstd.h>
#include "FMTFormLogger.h"
#include "FMTlogger.h"
#include <memory>

#define LOGLEVEL 1
#define TASKLOGLEVEL 0

using namespace Logging;

std::unique_ptr <FMTlogger> Wrapper::FMTFormLogger::Clone() const
{
	return std::unique_ptr<FMTlogger>(new Wrapper::FMTFormLogger(*this));
}

FMTlogger* Wrapper::FMTFormLogger::clone() const
{
	return new Wrapper::FMTFormLogger(*this);
}

Wrapper::FMTFormLogger::FMTFormLogger(const std::string& nomFichierLogger, logfunc feed) :FMTlogger(), keepprint(false), lastprint(), sendfeedback(feed)
{
	redirectofile(nomFichierLogger, false);
	setlogginglevel(LOGLEVEL);
	setstreamflush(true);
}

void Wrapper::FMTFormLogger::dokeepprint()
{
	keepprint = true;
}
std::string Wrapper::FMTFormLogger::getlastprint() const
{
	return lastprint;
}


void Wrapper::FMTFormLogger::cout(const char * message) const
{
	Logging::FMTlogger::cout(message);
	if (keepprint)
	{
		lastprint += message;
	}
	else {
		sendfeedback(message);
	}
}

void Wrapper::FMTFormLogger::settasklogginglevel(int taskLogLevel)
{
	setlogginglevel(taskLogLevel);
}

void Wrapper::FMTFormLogger::setdefaultlogginglevel()
{
	setlogginglevel(LOGLEVEL);
}
