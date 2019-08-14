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
namespace Version
{

FMTversion::FMTversion()
	{

	}

int FMTversion::getmajor()
	{
	return API_MAJOR;
	}
int FMTversion::getminor()
	{
	return API_MINOR;
	}
int FMTversion::getpatch()
	{
	return API_PATCH;
	}
string FMTversion::getversion()
	{
	return to_string(getmajor()) + "." + to_string(getminor()) + "." + to_string(getpatch());
	}
bool FMTversion::isatleast(int major, int minor, int patch)
	{
	if (getmajor()>=major && getminor()>=minor && getpatch()>=patch)
		{
		return true;
		}
	return false;
	}	
bool FMTversion::hashfeature(const string& name)
	{
	return false;
	}

}