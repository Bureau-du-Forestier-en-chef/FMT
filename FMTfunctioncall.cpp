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

#include "FMTfunctioncall.h"
#include "FMTlogger.h"

namespace Core{


FMTfunctioncall::FMTfunctioncall():key()
{
}

FMTfunctioncall::FMTfunctioncall(const std::string& base)
	{
	key = FMTfkey::notvalid;
	if (base == "EXP")
		{ 
		key = FMTfkey::expo;
	}else if (base=="LN")
		{
		key = FMTfkey::ln;
		}
	}
double FMTfunctioncall::call(const double& rhs) const
	{
	double result = 0;
	if (FMTfkey::expo == key)
	{
		result = std::pow(2.71828, rhs); //precision stuff...
	}
	else if (FMTfkey::ln == key && rhs > 0)
	{
		result = std::log(rhs);
	}
	result = std::round(result * 100000000) / 100000000;
	return result;
	}
FMTfunctioncall::FMTfunctioncall(const FMTfunctioncall& rhs) : key(rhs.key)
	{
	
	}

bool FMTfunctioncall::valid() const
	{
	return (key != FMTfkey::notvalid);
	}
FMTfunctioncall& FMTfunctioncall::	operator = (const FMTfunctioncall& rhs)
	{
	if (this!=&rhs)
		{
		key = rhs.key;
		}
	return *this;
	}

}