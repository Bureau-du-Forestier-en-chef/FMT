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

Wrapper::FMTFormLogger::FMTFormLogger(
	const std::string& nomFichierLogger, logfunc feed)
	: FMTlogger(), keepprint(false), m_isMainInstance(true),
	lastprint(), sendfeedback(feed)
{
	redirectofile(nomFichierLogger, false);
	setlogginglevel(LOGLEVEL);
	setstreamflush(true);
}

Wrapper::FMTFormLogger::FMTFormLogger(const FMTFormLogger& rhs)
	: FMTlogger(rhs), keepprint(rhs.keepprint),
	m_isMainInstance(false), // les clones ne sont jamais l'instance principale
	lastprint(rhs.lastprint), sendfeedback(rhs.sendfeedback)
{
}

void Wrapper::FMTFormLogger::logtime()
{
	if (m_isMainInstance)
	{
		Logging::FMTlogger::logtime();
	}
}

void Wrapper::FMTFormLogger::dokeepprint()
{
	keepprint = true;
}

void Wrapper::FMTFormLogger::resetkeepprint()
{
	keepprint = false;
	lastprint.clear();
}

std::string Wrapper::FMTFormLogger::getlastprint() const
{
	return lastprint;
}


void Wrapper::FMTFormLogger::cout(const char * message) const
{
	if (m_FileStream && m_FileStream->is_open())
	{
		Logging::FMTlogger::cout(message);
	}
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
