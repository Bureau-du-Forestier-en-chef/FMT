/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTError.h"




namespace Exception
{
	FMTError::FMTError() : FMTException() {}
	FMTError::FMTError(FMTexc p_exception, FMTlev p_level, Core::FMTsection p_section, const std::string& p_message,
		const std::string& p_method, const std::string& p_file, const int& p_line,
		const std::string& p_FrenchDescription, const std::string& p_EnglishDescription) :
		FMTException(p_exception, p_level, p_section, p_message,
			p_method, p_file, p_line, p_FrenchDescription, p_EnglishDescription) {}


}
