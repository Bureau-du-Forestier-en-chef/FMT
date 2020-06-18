/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

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

