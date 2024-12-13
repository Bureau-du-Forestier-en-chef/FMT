/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
	
#include "FMTseException.h"
#include <string>

namespace Exception
{

	FMTseException::FMTseException() noexcept :std::logic_error("Se exception"), m_nSE(0)
		{
	
		}

	FMTseException::FMTseException(unsigned int p_n) noexcept :std::logic_error(
		std::string("WIN32 Se exception number "+std::to_string(p_n)).c_str()), m_nSE(p_n)
		{
	
		}
	unsigned int FMTseException::getSeNumber() const noexcept
		{ 
		return m_nSE; 
		}

}