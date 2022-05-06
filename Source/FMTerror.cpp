/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTerror.hpp"
#if defined FMTWITHOSI
	#include <CoinError.hpp>
#endif



namespace Exception
{
	FMTerror::FMTerror() : FMTexception() {}
	FMTerror::FMTerror(const FMTexception& rhs) : FMTexception(rhs) {}
	FMTerror::FMTerror(const FMTexc lexception, const std::string message) : FMTexception(lexception, message) {}
	FMTerror::FMTerror(const FMTexc lexception, const Core::FMTsection lsection, const std::string message) :
		FMTexception(lexception, lsection, message) {}
	FMTerror::FMTerror(const FMTexc lexception, const Core::FMTsection lsection, const std::string message,
		const std::string& lmethod, const std::string& lfile, const int& lline) : 
		FMTexception(lexception, lsection, message, lmethod, lfile, lline) {}
	FMTerror::FMTerror(const FMTexc lexception, const std::string message,
		const std::string& lmethod, const std::string& lfile, const int& lline) :
		FMTexception(lexception, message, lmethod, lfile, lline)
		{

		}
	#if defined FMTWITHOSI
		FMTerror::FMTerror(const CoinError& coinexception):
			FMTexception(FMTexc::FMTcoinerror,
				"Coin Function failed: " + coinexception.message(), coinexception.className() + "::" + coinexception.methodName(),
				coinexception.fileName(), coinexception.lineNumber())
			{

			}
	#endif

	FMTerror::FMTerror(const boost::bad_graph& boostexception):
		FMTexception(FMTexc::FMTboostgrapherror, "Boost Graph Function failed: "+ std::string(boostexception.what()))
		{

		}


}
