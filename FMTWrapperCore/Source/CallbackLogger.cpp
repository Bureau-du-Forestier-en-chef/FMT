#include "CallbackLogger.h"
#include "FMTLogger.h"
#include <memory>

#define LOGLEVEL 1
#define TASKLOGLEVEL 0

using namespace Logging;

std::unique_ptr <FMTLogger> FMTWrapperCore::CallbackLogger::Clone() const
{
	return std::unique_ptr<FMTLogger>(new FMTWrapperCore::CallbackLogger(*this));
}

FMTLogger* FMTWrapperCore::CallbackLogger::clone() const
{
	return new FMTWrapperCore::CallbackLogger(*this);
}

FMTWrapperCore::CallbackLogger::CallbackLogger(
	const std::string& nomFichierLogger, logfunc feed)
	: FMTLogger(), keepprint(false), m_isMainInstance(true),
	lastprint(), sendfeedback(feed)
{
	redirectToFile(nomFichierLogger, false);
	_setLoggingLevel(LOGLEVEL);
	setStreamFlush(true);
}

FMTWrapperCore::CallbackLogger::CallbackLogger(const CallbackLogger& rhs)
	: FMTLogger(rhs), 
	keepprint(rhs.keepprint),
	m_isMainInstance(false),
	lastprint(rhs.lastprint), 
	sendfeedback(rhs.sendfeedback)
{
	//m_FileStream.reset();
	//filepath.clear();
}

void FMTWrapperCore::CallbackLogger::logTime()
{
	// Intentional no-op: sendfeedback cannot be called from a native thread.
	// The log file is opened once in the constructor through redirectToFile().
	// Timestamps are written explicitly through *logger << logStamp
}

FMTWrapperCore::CallbackLogger::~CallbackLogger()
{
	// Closes and releases the file stream BEFORE the base destructor
	// FMTLogger::~FMTLogger() runs. Otherwise, virtual dispatch during destruction
	// calls FMTLogger::logTime() (the derived part being already destroyed, the no-op
	// override is no longer reached), which inserts a stray timestamp line
	// "Thread(id) <date>" in the log each time a clone of the logger is destroyed.
	closeFile();
}

void FMTWrapperCore::CallbackLogger::closeFile()
{
	if (m_FileStream)
	{
		if (m_FileStream->is_open())
		{
			m_FileStream->close();
		}
		m_FileStream.reset();
	}
}

void FMTWrapperCore::CallbackLogger::dokeepprint()
{
	keepprint = true;
}

void FMTWrapperCore::CallbackLogger::resetkeepprint()
{
	keepprint = false;
	lastprint.clear();
}

std::string FMTWrapperCore::CallbackLogger::getlastprint() const
{
	return lastprint;
}


void FMTWrapperCore::CallbackLogger::_cout(const char * message) const
{
	if (m_FileStream && m_FileStream->is_open())
	{
		(*m_FileStream) << message;
		m_FileStream->flush();
	}
	if (keepprint)
	{
		lastprint += message;
	}
	else {
		sendfeedback(message);
	}
}

void FMTWrapperCore::CallbackLogger::settasklogginglevel(int taskLogLevel)
{
	_setLoggingLevel(taskLogLevel);
}

void FMTWrapperCore::CallbackLogger::setdefaultlogginglevel()
{
	_setLoggingLevel(LOGLEVEL);
}
