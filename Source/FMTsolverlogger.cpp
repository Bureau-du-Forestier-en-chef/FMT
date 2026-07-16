/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI
#include "FMTsolverlogger.h"
#include "FMTlogger.h"

/// Namespace for the log management, provides different log handlers.
namespace Logging
{

FMTSolverLogger::FMTSolverLogger(FMTLogger& baselogger):
	baselogger(&baselogger),
	ownthelogger(false)
	{


	}
	
FMTSolverLogger::FMTSolverLogger():
	baselogger(nullptr),
	ownthelogger(true)
	{

	}

void FMTSolverLogger::cleanUp()
{
	if (baselogger && ownthelogger)
	{
		delete baselogger;
	}
}


void FMTSolverLogger::copy(const FMTSolverLogger& rhs)
{
	cleanUp();
	baselogger = rhs.baselogger->clone();
	ownthelogger = true;
}

FMTSolverLogger::~FMTSolverLogger()
	{
	cleanUp();
	}

/*FMTSolverLogger::FMTSolverLogger(const FMTSolverLogger& rhs) :
	baselogger(nullptr),
	ownthelogger(true)
	{
	copy(rhs);
	}*/

void FMTSolverLogger::copyFrom(const FMTSolverLogger& rhs)
{
	baselogger = nullptr;
	ownthelogger = true;
	copy(rhs);
}

/*FMTSolverLogger& FMTSolverLogger::operator = (const FMTSolverLogger& rhs)
	{
	if (this!=&rhs)
		{
		copy(rhs);
		}
	return *this;
	}*/

int FMTSolverLogger::print()
	{
	return baselogger->print();
	}

void FMTSolverLogger::checkSeverity()
	{
	baselogger->checkSeverity();
	}

void FMTSolverLogger::checkcoinSeverity()
	{
	CoinMessageHandler::checkSeverity();
	}

CoinMessageHandler* FMTSolverLogger::clone() const
	{
	FMTSolverLogger* solverLogger = new FMTSolverLogger();
	solverLogger->copyFrom(*this);
	return solverLogger;
	}

}
#endif

