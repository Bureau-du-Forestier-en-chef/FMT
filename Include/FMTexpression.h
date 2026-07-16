/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTEXPRESSION_Hm_included
#define FMTEXPRESSION_Hm_included

#include <string>
#include <vector>
#include <map>
#include <stack>

namespace Core
{
	class FMTExpression
	{
		std::vector<std::string>infix;
		std::vector<std::string> getPostFix(const std::vector<std::string>& localinfix) const;
		std::vector<std::string> replaceVariables(const std::map<std::string, double>& mapping) const;
		double evaluatePostFix(const std::vector<std::string>& postfix) const;
		bool isNumber(const std::string& s) const;
	public:
		FMTExpression();
		FMTExpression(const std::vector<std::string>& lsources);
		FMTExpression(const FMTExpression& rhs);
        FMTExpression simplify(std::map<std::string,double>& values) const;
		std::vector<std::string>getVariables() const;
		double shuntingYard(const std::map<std::string, double>& mapping) const;
		std::vector<std::string>getInFix() const;
        operator std::string() const;
		FMTExpression& operator = (const FMTExpression& rhs);
		~FMTExpression() = default;

	};
}

#endif
