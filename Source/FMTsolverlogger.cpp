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

FMTsolverlogger::FMTsolverlogger(FMTlogger& baselogger):
	baselogger(&baselogger),
	ownthelogger(false)
	{


	}
	
FMTsolverlogger::FMTsolverlogger():
	baselogger(nullptr),
	ownthelogger(true)
	{

	}

void FMTsolverlogger::cleanup()
{
	if (baselogger && ownthelogger)
	{
		delete baselogger;
	}
}


void FMTsolverlogger::copy(const FMTsolverlogger& rhs)
{
	cleanup();
	baselogger = rhs.baselogger->clone();
	ownthelogger = true;
}

FMTsolverlogger::~FMTsolverlogger()
	{
	cleanup();
	}

/*FMTsolverlogger::FMTsolverlogger(const FMTsolverlogger& rhs) :
	baselogger(nullptr),
	ownthelogger(true)
	{
	copy(rhs);
	}*/

void FMTsolverlogger::copyFrom(const FMTsolverlogger& rhs)
{
	baselogger = nullptr;
	ownthelogger = true;
	copy(rhs);
}

/*FMTsolverlogger& FMTsolverlogger::operator = (const FMTsolverlogger& rhs)
	{
	if (this!=&rhs)
		{
		copy(rhs);
		}
	return *this;
	}*/

int FMTsolverlogger::print()
	{
	return baselogger->print();
	}

void FMTsolverlogger::checkSeverity()
	{
	baselogger->checkSeverity();
	}

void FMTsolverlogger::checkcoinSeverity()
	{
	CoinMessageHandler::checkSeverity();
	}

CoinMessageHandler* FMTsolverlogger::clone() const
	{
	FMTsolverlogger* solverLogger = new FMTsolverlogger();
	solverLogger->copyFrom(*this);
	return solverLogger;
	}

}
#endif

