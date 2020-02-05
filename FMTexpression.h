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

#ifndef FMTEXPRESSION_H_INCLUDED
#define FMTEXPRESSION_H_INCLUDED

#include <string>
#include <vector>
#include <map>
#include "FMToperator.h"
#include "FMTfunctioncall.h"
#include "FMTlogger.h"
#include <stack>

namespace Core
{
	class FMTexpression
	{
		std::vector<std::string>infix;
		std::vector<std::string> getpostfix(const std::vector<std::string>& localinfix) const;
		std::vector<std::string> replacevariables(const std::map<std::string, double>& mapping) const;
		double evaluatepostfix(const std::vector<std::string>& postfix) const;
		bool is_number(const std::string& s) const;
	public:
		FMTexpression();
		FMTexpression(const std::vector<std::string>& lsources);
		FMTexpression(const FMTexpression& rhs);
        FMTexpression simplify(std::map<std::string,double>& values) const;
		std::vector<std::string>getvariables() const;
		double shuntingyard(const std::map<std::string, double>& mapping) const;
		std::vector<std::string>getinfix() const;
        operator std::string() const;
		FMTexpression& operator = (const FMTexpression& rhs);
		~FMTexpression() = default;

	};
}

#endif
