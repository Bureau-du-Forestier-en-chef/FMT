/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTError.h"
#if defined FMTWITHOSI
	#include <CoinError.hpp>
#endif



namespace Exception
{
	FMTError::FMTError() : FMTException() {}
	FMTError::FMTError(const FMTException& rhs) : FMTException(rhs) {}
	FMTError::FMTError(const FMTexc lexception, const std::string message) : FMTException(lexception, message) {}
	FMTError::FMTError(const FMTexc lexception, const Core::FMTsection lsection, const std::string message) :
		FMTException(lexception, lsection, message) {}
	FMTError::FMTError(const FMTexc lexception, const Core::FMTsection lsection, const std::string message,
		const std::string& lmethod, const std::string& lfile, const int& lline) : 
		FMTException(lexception, lsection, message, lmethod, lfile, lline) {}
	FMTError::FMTError(const FMTexc lexception, const std::string message,
		const std::string& lmethod, const std::string& lfile, const int& lline) :
		FMTException(lexception, message, lmethod, lfile, lline)
		{

		}
	#if defined FMTWITHOSI
		FMTError::FMTError(const CoinError& coinexception):
			FMTException(FMTexc::FMTcoinerror,
				"Coin Function failed: " + coinexception.message(), coinexception.className() + "::" + coinexception.methodName(),
				coinexception.fileName(), coinexception.lineNumber())
			{

			}
	#endif

	FMTError::FMTError(const boost::bad_graph& boostexception):
		FMTException(FMTexc::FMTboostgrapherror, "Boost Graph Function failed: "+ std::string(boostexception.what()))
		{

		}


}
