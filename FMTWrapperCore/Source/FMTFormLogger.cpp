#include "FMTFormLogger.h"
#include "FMTLogger.h"
#include <memory>

#define LOGLEVEL 1
#define TASKLOGLEVEL 0

using namespace Logging;

std::unique_ptr <FMTLogger> FMTWrapperCore::FMTFormLogger::Clone() const
{
	return std::unique_ptr<FMTLogger>(new FMTWrapperCore::FMTFormLogger(*this));
}

FMTLogger* FMTWrapperCore::FMTFormLogger::clone() const
{
	return new FMTWrapperCore::FMTFormLogger(*this);
}

FMTWrapperCore::FMTFormLogger::FMTFormLogger(
	const std::string& nomFichierLogger, logfunc feed)
	: FMTLogger(), keepprint(false), m_isMainInstance(true),
	lastprint(), sendfeedback(feed)
{
	redirectToFile(nomFichierLogger, false);
	_setLoggingLevel(LOGLEVEL);
	setStreamFlush(true);
}

FMTWrapperCore::FMTFormLogger::FMTFormLogger(const FMTFormLogger& rhs)
	: FMTLogger(rhs), 
	keepprint(rhs.keepprint),
	m_isMainInstance(false),
	lastprint(rhs.lastprint), 
	sendfeedback(rhs.sendfeedback)
{
	//m_FileStream.reset();
	//filepath.clear();
}

void FMTWrapperCore::FMTFormLogger::logTime()
{
	// No-op intentionnel : sendfeedback ne peut pas être appelé depuis un thread natif.
	// Le fichier log est ouvert une fois dans le constructeur via redirectToFile().
	// Les timestamps sont écrits explicitement via *logger << logStamp
}

FMTWrapperCore::FMTFormLogger::~FMTFormLogger()
{
	// Ferme et libere le flux fichier AVANT l'execution du destructeur de base
	// FMTLogger::~FMTLogger(). Sinon, la répartition virtuelle pendant la destruction
	// appelle FMTLogger::logTime() (la partie dérivée étant déjà détruite, l'override
	// no-op n'est plus atteint), ce qui insère une ligne timestamp parasite
	// "Thread(id) <date>" dans le log à chaque destruction d'un clone du logger.
	closeFile();
}

void FMTWrapperCore::FMTFormLogger::closeFile()
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

void FMTWrapperCore::FMTFormLogger::dokeepprint()
{
	keepprint = true;
}

void FMTWrapperCore::FMTFormLogger::resetkeepprint()
{
	keepprint = false;
	lastprint.clear();
}

std::string FMTWrapperCore::FMTFormLogger::getlastprint() const
{
	return lastprint;
}


void FMTWrapperCore::FMTFormLogger::_cout(const char * message) const
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

void FMTWrapperCore::FMTFormLogger::settasklogginglevel(int taskLogLevel)
{
	_setLoggingLevel(taskLogLevel);
}

void FMTWrapperCore::FMTFormLogger::setdefaultlogginglevel()
{
	_setLoggingLevel(LOGLEVEL);
}
