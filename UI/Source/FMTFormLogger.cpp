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
	: FMTlogger(rhs), 
	keepprint(rhs.keepprint),
	m_isMainInstance(false),
	lastprint(rhs.lastprint), 
	sendfeedback(rhs.sendfeedback)
{
	//m_FileStream.reset();
	//filepath.clear();
}

void Wrapper::FMTFormLogger::logtime()
{
	// No-op intentionnel : sendfeedback ne peut pas être appelé depuis un thread natif.
	// Le fichier log est ouvert une fois dans le constructeur via redirectofile().
	// Les timestamps sont écrits explicitement via *logger << logstamp
}

Wrapper::FMTFormLogger::~FMTFormLogger()
{
	// Ferme et libere le flux fichier AVANT l'execution du destructeur de base
	// FMTlogger::~FMTlogger(). Sinon, la répartition virtuelle pendant la destruction
	// appelle FMTlogger::logtime() (la partie dérivée étant déjà détruite, l'override
	// no-op n'est plus atteint), ce qui insère une ligne timestamp parasite
	// "Thread(id) <date>" dans le log à chaque destruction d'un clone du logger.
	if (m_FileStream)
	{
		if (m_FileStream->is_open())
		{
			m_FileStream->close();
		}
		m_FileStream.reset();
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

void Wrapper::FMTFormLogger::settasklogginglevel(int taskLogLevel)
{
	setlogginglevel(taskLogLevel);
}

void Wrapper::FMTFormLogger::setdefaultlogginglevel()
{
	setlogginglevel(LOGLEVEL);
}
