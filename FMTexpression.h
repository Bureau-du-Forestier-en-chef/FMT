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
		vector<string>infix;
		vector<string> getpostfix(const vector<string>& localinfix) const;
		vector<string> replacevariables(const map<string, double>& mapping) const;
		double evaluatepostfix(const vector<string>& postfix) const;
		bool is_number(const string& s) const;
	public:
		FMTexpression();
		FMTexpression(const vector<string>& lsources);
		FMTexpression(const FMTexpression& rhs);
        FMTexpression simplify(map<string,double>& values) const;
        vector<string>getvariables() const;
		double shuntingyard(const map<string, double>& mapping) const;
		vector<string>getinfix() const;
        operator string() const;
		FMTexpression& operator = (const FMTexpression& rhs);
		~FMTexpression() = default;

	};
}

#endif
