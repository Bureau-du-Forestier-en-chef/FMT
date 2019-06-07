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
