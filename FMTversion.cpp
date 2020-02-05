/*
MIT License

Copyright (c) [2019] [Bureau du forestier en chef]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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
	#ifdef FMTWITHGDAL
		if (name=="GDAL")
			{
			return true;
			}
		#ifdef HAVE_GEOS
		if (name == "GEOS")
			{
			return true;
			}
		#endif
	#endif
	return false;
	}

}