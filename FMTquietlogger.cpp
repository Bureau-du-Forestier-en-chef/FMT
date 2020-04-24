#include "FMTquietlogger.h"

namespace Logging
{

FMTquietlogger::FMTquietlogger()
	{
	this->setLogLevel(0);
	}

int FMTquietlogger::print()
	{
	return FMTlogger::print();
	}

void FMTquietlogger::checkSeverity()
	{
	FMTlogger::checkSeverity();
	}

CoinMessageHandler * FMTquietlogger::clone() const
	{
	return new FMTquietlogger(*this);
	}
}

