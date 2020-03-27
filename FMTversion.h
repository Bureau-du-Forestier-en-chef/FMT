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

#ifndef FMTVERSION_H_INCLUDED
#define FMTVERSION_H_INCLUDED

#include <string>


#define FMT_MAJOR 0
#define FMT_MINOR 0
#define FMT_PATCH 0


namespace Version
{
// DocString: FMTversion
/**
The only purpose of this class is to let the user see which version of FMT he's using.
It also gives some information about the build date and the actual date.
It also informs the user about which features are avalaible in FMT. 
*/

class FMTversion
	{
	public:
		// DocString: FMTversion()
		/**
		Default constructor of FMTversion.
		*/
		FMTversion();
		// DocString: ~FMTversion()
		/**
		Destructor of FMTversion.
		*/
		~FMTversion()=default;
		// DocString: FMTversion::getmajor
		/**
		Getter of the major version of FMT.
		*/
		static int getmajor();
		// DocString: FMTversion::getminor
		/**
		Getter of the minor version of FMT.
		*/
		static int getminor();
		// DocString: FMTversion::getpatch
		/**
		Getter of the patch version of FMT.
		*/
		static int getpatch();
		// DocString: FMTversion::getversion
		/**
		Getter of the string of the version : MAjor.Minor.PATCH
		*/
		static std::string getversion();
		// DocString: FMTversion::getdatenow
		/**
		Getter of the actual date: Day Month date
		*/
		static std::string getdatenow();
		// DocString: FMTversion::getbuilddate
		/**
		Getter of the build date: date
		*/
		static std::string getbuilddate();
		// DocString: FMTversion::isatleast
		/**
		Test if the FMT version is at least the given major/minor/patch parameters.
		*/
		static bool isatleast(int major, int minor, int patch);
		// DocString: FMTversion::hasfeature
		/**
		Test if the FMT version have the named feature.
		Features GDAL,OSI,MOSEK,PYTHON,R.
		*/
		static bool hasfeature(const std::string& name);
	};
}

#endif