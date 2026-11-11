/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTExpression.h"
#include "FMTOperator.h"
#include "FMTFunctionCall.h"

namespace Core
{

FMTExpression::FMTExpression():m_infix()
	{

	}

FMTExpression::FMTExpression(const FMTExpression& rhs) : m_infix(rhs.m_infix)
	{

	}

FMTExpression& FMTExpression::operator = (const FMTExpression& rhs)
	{
	if (this!=&rhs)
		{
		m_infix = rhs.m_infix;
		}
	return *this;
	}

FMTExpression::operator std::string() const
    {
	std::string exx = "";
    for (const std::string& value : m_infix)
        {
        exx+=" "+value;
        }
    //exx.pop_back();
    return exx;
    }


std::vector<std::string> FMTExpression::_getPostFix(const std::vector<std::string>& localinfix) const
	{
	std::stack<std::string> values;
	std::stack<bool>function_parenthesis;
	bool function_start = false;
	const std::vector<std::string>infixes = localinfix;
	std::vector<std::string>postfix;
	//https://stackoverflow.com/questions/11708195/infix-to-postfix-with-function-support
	//http://interactivepython.org/runestone/static/pythonds/BasicDS/InfixPrefixandPostfixExpressions.html
	for (const std::string& invalue : infixes)
		{
		const FMTOperator op(invalue);
		const FMTFunctionCall fcall(invalue);
		if (fcall.valid())
			{
			function_start = true;
			values.push(invalue);
			}else if (op.valid())
				{
				while (!values.empty() && ((FMTOperator(values.top()).precedence() >= op.precedence() ) || FMTFunctionCall(values.top()).valid()) && values.top() != "(")
					{
					postfix.push_back(values.top());
					values.pop();
					}
				values.push(invalue);
				}
			else if (invalue == "(")
				{
				if (function_start)
					{
					function_parenthesis.push(true);
					function_start = false;
				}else {
					function_parenthesis.push(false);
					}
				values.push(invalue);
				}
			else if (invalue == ")")
				{
				while (!values.empty() && values.top() !="(")
					{
					postfix.push_back(values.top());
					values.pop();
					}
				if (!values.empty() && values.top() == "(" )//and in function!!! parenthesis
					{
					if (function_parenthesis.top())
						{
						values.pop();
						postfix.push_back(values.top());
						}
					function_parenthesis.pop();
					values.pop();
					}
			}else {
				postfix.push_back(invalue);
			}
		}
	while (!values.empty())
		{
		postfix.push_back(values.top());
		values.pop();
		}

		return postfix;
	}


bool FMTExpression::_isNumber(const std::string& s) const
    {
	std::string::const_iterator it = s.begin();
    while (it != s.end() && (isdigit(*it) || (*it)=='.')) ++it;
    return !s.empty() && it == s.end();
    }

std::vector<std::string>FMTExpression::getVariables() const
    {
	std::vector<std::string>variables;
    for (const std::string& value : m_infix)
        {
        if(!_isNumber(value) && !FMTOperator(value).valid() && !FMTFunctionCall(value).valid()) //assign 0 to all variables!
            {
            variables.push_back(value);
            }
        }
    return variables;
    }

FMTExpression FMTExpression::simplify(std::map<std::string,double>& values) const
    {
	std::map<std::string,double>shuntvalues;
    for (const std::string& value : m_infix)
        {
        if(!_isNumber(value) && !FMTOperator(value).valid() && !value.empty()) //assign 0 to all variables!
            {
            shuntvalues[value] = 0;
			
            }
        }
    double rest = this->shuntingYard(shuntvalues);
	
    values["RHS"] = -rest;
	std::vector<std::string>new_expression;
    for (std::map<std::string,double>::iterator shuntit = shuntvalues.begin(); shuntit != shuntvalues.end(); shuntit++)
        {
        shuntit->second = 1;//set to 1
        const double factorwrest = this->shuntingYard(shuntvalues);
        const double variable_factor = (factorwrest - rest);

        if (variable_factor!=0)
            {
            values[shuntit->first] = variable_factor;
            new_expression.push_back(shuntit->first);
            new_expression.push_back("*");
            if (variable_factor > 0)
                {
                new_expression.push_back(std::to_string(variable_factor));
                }else{
                new_expression.push_back("-");
                new_expression.push_back(std::to_string(abs(variable_factor)));
                }
            new_expression.push_back("+");
            }
        shuntit->second = 0; //unset
        }
	if (!new_expression.empty())
	{
		new_expression.pop_back();
	}
	
    if (rest != 0)
        {
        if (rest > 0)
            {
            new_expression.push_back("+");
            new_expression.push_back(std::to_string(rest));
            }else{
            new_expression.push_back("-");
            new_expression.push_back(std::to_string(abs(rest)));
            }
        }
    return FMTExpression(new_expression);
    }


FMTExpression::FMTExpression(const std::vector<std::string>& lsources) :m_infix(lsources)
	{

	}

std::vector<std::string> FMTExpression::_replaceVariables(const std::map<std::string, double>& mapping) const
	{
	if (!mapping.empty())
		{
		std::vector<std::string>newinfix;
		for (const std::string& invar : m_infix)
		{

			std::string result = invar;
			if (mapping.find(invar) != mapping.end())
			{
				result = std::to_string(mapping.at(invar));
			}
			newinfix.push_back(result);
		}
		return newinfix;
		}
	return m_infix;
	}

double FMTExpression::_evaluatePostFix(const std::vector<std::string>& postfix) const
	{
	std::stack<double>values;
	for (const std::string& post : postfix)
		{
		const FMTOperator op(post);
		const FMTFunctionCall fcall(post);
		if (!op.valid() && !fcall.valid())
			{
			values.push(std::stod(post));
			}else if (op.valid())
				{
				double rhs = values.top();
				values.pop();
				double lhs = 0;
				if (!values.empty())
					{
					lhs = values.top();
					values.pop();
					}
				values.push(op.call(lhs,rhs));
			}else if (fcall.valid())
				{
				double lhs = values.top();
				values.pop();
				values.push(fcall.call(lhs));
				}
		}
	return values.top();
	}



std::vector<std::string>FMTExpression::getInFix() const
    {
    return m_infix;
    }

double FMTExpression::shuntingYard(const std::map<std::string, double>& mapping) const
	{
	double result = 0;
	if (!m_infix.empty())
		{
		const std::vector<std::string>newin = _replaceVariables(mapping);
		const std::vector<std::string>postfix = _getPostFix(newin);
		result = _evaluatePostFix(postfix);
		}
	return result;
	}

}
