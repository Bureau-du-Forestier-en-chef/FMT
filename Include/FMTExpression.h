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
	// DocString: FMTExpression
	/**
	@brief Mathematical expression evaluated with the shunting yard algorithm, used in yields and outputs.
	*/
	class FMTExpression
	{
		std::vector<std::string>infix;
		// DocString: FMTExpression::getPostFix
		/**
		@brief Convert an infix expression to postfix notation.
		@param[in] localinfix the infix expression.
		@return the postfix expression.
		*/
		std::vector<std::string> getPostFix(const std::vector<std::string>& localinfix) const;
		// DocString: FMTExpression::replaceVariables
		/**
		@brief Replace the variables of the expression by their values.
		@param[in] mapping the values of the variables.
		@return the expression with variables replaced by their values.
		*/
		std::vector<std::string> replaceVariables(const std::map<std::string, double>& mapping) const;
		// DocString: FMTExpression::evaluatePostFix
		/**
		@brief Evaluate a postfix expression.
		@param[in] postfix the postfix expression.
		@return the value of the expression.
		*/
		double evaluatePostFix(const std::vector<std::string>& postfix) const;
		// DocString: FMTExpression::isNumber
		/**
		@brief Return true if a string is a number.
		@param[in] s the string to check.
		@return true if the string is a number else false.
		*/
		bool isNumber(const std::string& s) const;
	public:
		// DocString: FMTExpression()
		/**
		@brief Default constructor for FMTExpression.
		*/
		FMTExpression();
		// DocString: FMTExpression(const std::vector<std::string>&)
		/**
		@brief Construct a FMTExpression from a vector of tokens.
		@param[in] lsources the tokens of the expression.
		*/
		FMTExpression(const std::vector<std::string>& lsources);
		// DocString: FMTExpression(const FMTExpression&)
		/**
		@brief Copy constructor for FMTExpression.
		@param[in] rhs the FMTExpression to copy.
		*/
		FMTExpression(const FMTExpression& rhs);
        // DocString: FMTExpression::simplify
        /**
        @brief Return a simplified expression using the given variable values.
        @param[in,out] values the values of the variables.
        @return the simplified expression.
        */
        FMTExpression simplify(std::map<std::string,double>& values) const;
		// DocString: FMTExpression::getVariables
		/**
		@brief Return the variables of the expression.
		@return the variables of the expression.
		*/
		std::vector<std::string>getVariables() const;
		// DocString: FMTExpression::shuntingYard
		/**
		@brief Evaluate the expression using the shunting yard algorithm.
		@param[in] mapping the values of the variables.
		@return the value of the expression.
		*/
		double shuntingYard(const std::map<std::string, double>& mapping) const;
		// DocString: FMTExpression::getInFix
		/**
		@brief Return the infix expression.
		@return the infix expression.
		*/
		std::vector<std::string>getInFix() const;
        // DocString: FMTExpression::operator std::string
        /**
        @brief Convert the expression to its string representation.
        @return the string representation of the expression.
        */
        operator std::string() const;
		// DocString: FMTExpression::operator=
		/**
		@brief Copy assignment operator for FMTExpression.
		@param[in] rhs the FMTExpression to copy.
		@return a reference to this FMTExpression.
		*/
		FMTExpression& operator = (const FMTExpression& rhs);
		// DocString: ~FMTExpression()
		/**
		@brief Default destructor for FMTExpression.
		*/
		~FMTExpression() = default;

	};
}

#endif
