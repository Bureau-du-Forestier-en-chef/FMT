/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
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
