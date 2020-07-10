/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTversion.h"
#include <chrono>
#include <ctime>


namespace Version
{

FMTversion::FMTversion()
	{

	}

int FMTversion::getmajor()
	{
	return FMT_MAJOR;
	}
int FMTversion::getminor()
	{
	return FMT_MINOR;
	}
int FMTversion::getpatch()
	{
	return FMT_PATCH;
	}

std::string FMTversion::getdatenow()
	{
	auto now = std::chrono::system_clock::now();
	std::time_t end_time = std::chrono::system_clock::to_time_t(now);
	return static_cast<std::string>(std::ctime(&end_time));
	}

std::string FMTversion::getbuilddate()
	{
	return (static_cast<std::string>((__DATE__)) + static_cast<std::string>(std::string(__TIME__)));
	}

std::string FMTversion::getversion()
	{
	return std::to_string(getmajor()) + "." + std::to_string(getminor()) + "." + std::to_string(getpatch());
	}
bool FMTversion::isatleast(int major, int minor, int patch)
	{
	if (getmajor()>=major && getminor()>=minor && getpatch()>=patch)
		{
		return true;
		}
	return false;
	}	
bool FMTversion::hasfeature(const std::string& name)
	{
	#ifdef FMTWITHR
		if (name == "R")
		{
			return true;
		}
	#endif
	#ifdef FMTWITHPYTHON
	if (name == "PYTHON")
		{
		return true;
		}
	#endif
	#ifdef FMTWITHGDAL
		if (name=="GDAL")
			{
			return true;
			}
	#endif
	#ifdef FMTWITHOSI
		if (name == "OSI")
			{
			return true;
			}
		#ifdef  FMTWITHMOSEK
			if (name == "MOSEK")
			{
				return true;
			}
		#endif
	#endif
	return false;
	}

}