/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTVersion.h"
#include "FMTParser.h"
#include <chrono>
#include <ctime>
#include "FMTsolverinterface.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include "FMTExceptionHandler.h"


namespace Version
{
int FMTVersion::getMajor()
	{
	return FMT_MAJOR;
	}
int FMTVersion::getMinor()
	{
	return FMT_MINOR;
	}
int FMTVersion::getPatch()
	{
	return FMT_PATCH;
	}

std::string FMTVersion::getDatenow()
	{
	auto now = std::chrono::system_clock::now();
	std::time_t end_time = std::chrono::system_clock::to_time_t(now);
	return static_cast<std::string>(std::ctime(&end_time));
	}

std::string FMTVersion::getBuildDate()
	{
	return (static_cast<std::string>((__DATE__)) +" "+ static_cast<std::string>(std::string(__TIME__)));
	}

std::string FMTVersion::getVersion()
	{
	return std::to_string(getMajor()) + "." + std::to_string(getMinor()) + "." + std::to_string(getPatch());
	}
bool FMTVersion::isAtLeast(int major, int minor, int patch)
	{
	if (getMajor()>=major && getMinor()>=minor && getPatch()>=patch)
		{
		return true;
		}
	return false;
	}
bool FMTVersion::hasFeature(const std::string& name)
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
	#ifdef FMTWITHTONNXR
		if (name == "ONNXRUNTIME")
			{
			return true;
			}
	#endif
	return false;
	}

std::string FMTVersion::getLicense(bool french)
    {
    std::string fulllicense;
    try{
        std::string licenselocation = getRuntimeLocation();
        licenselocation += "\\LICENSES";
        if (!boost::filesystem::is_directory(boost::filesystem::path(licenselocation)))
            {
            _exhandler->raise(Exception::FMTexc::FMTinvalid_path,
                "Cannot find LICENSES directory at "+licenselocation,"FMTVersion::getLicense",__LINE__,__FILE__);
            }
        if (french)
            {
            licenselocation+="\\FR\\LiLiQ-R11unicode.txt";
            }else{
            licenselocation+="\\EN\\LILIQ-R11ENunicode.txt";
            }
        if (!boost::filesystem::is_regular_file(boost::filesystem::path(licenselocation)))
            {
            _exhandler->raise(Exception::FMTexc::FMTinvalid_path,
                "Cannot find "+licenselocation,"FMTVersion::getLicense",__LINE__,__FILE__);
            }
    Parser::FMTParser newparser;
    std::ifstream licensestream(licenselocation);
    if (newparser.tryOpening(licensestream,licenselocation))
        {
        while (licensestream.is_open())
			{
            const std::string line = newparser.getCleanLine(licensestream);
            fulllicense+=(line+"\n");
			}
        }
    }catch(...)
        {
        _exhandler->printExceptions("", "FMTVersion::getLicense", __LINE__, __FILE__);
        }
    return fulllicense;
    }

}
