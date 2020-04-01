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

#include "FMTexpression.h"

namespace Core
{

FMTexpression::FMTexpression():infix()
	{

	}

FMTexpression::FMTexpression(const FMTexpression& rhs) : infix(rhs.infix)
	{

	}

FMTexpression& FMTexpression::operator = (const FMTexpression& rhs)
	{
	if (this!=&rhs)
		{
		infix = rhs.infix;
		}
	return *this;
	}

FMTexpression::operator std::string() const
    {
	std::string exx = "";
    for (const std::string& value : infix)
        {
        exx+=" "+value;
        }
    exx.pop_back();
    return exx;
    }


std::vector<std::string> FMTexpression::getpostfix(const std::vector<std::string>& localinfix) const
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
		const FMToperator op(invalue);
		const FMTfunctioncall fcall(invalue);
		if (fcall.valid())
			{
			function_start = true;
			values.push(invalue);
			}else if (op.valid())
				{
				while (!values.empty() && ((FMToperator(values.top()).precedence() >= op.precedence() ) || FMTfunctioncall(values.top()).valid()) && values.top() != "(")
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


bool FMTexpression::is_number(const std::string& s) const
    {
	std::string::const_iterator it = s.begin();
    while (it != s.end() && (isdigit(*it) || (*it)=='.')) ++it;
    return !s.empty() && it == s.end();
    }

std::vector<std::string>FMTexpression::getvariables() const
    {
	std::vector<std::string>variables;
    for (const std::string& value : infix)
        {
        if(!is_number(value) && !FMToperator(value).valid() && !FMTfunctioncall(value).valid()) //assign 0 to all variables!
            {
            variables.push_back(value);
            }
        }
    return variables;
    }

FMTexpression FMTexpression::simplify(std::map<std::string,double>& values) const
    {
	std::map<std::string,double>shuntvalues;
    for (const std::string& value : infix)
        {
        if(!is_number(value) && !FMToperator(value).valid() && !value.empty()) //assign 0 to all variables!
            {
            shuntvalues[value] = 0;
            }
        }
    double rest = this->shuntingyard(shuntvalues);
    values["RHS"] = -rest;
	std::vector<std::string>new_expression;
    for (std::map<std::string,double>::iterator shuntit = shuntvalues.begin(); shuntit != shuntvalues.end(); shuntit++)
        {
        shuntit->second = 1;//set to 1
        const double factorwrest = this->shuntingyard(shuntvalues);
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
    new_expression.pop_back();
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
    return FMTexpression(new_expression);
    }


FMTexpression::FMTexpression(const std::vector<std::string>& lsources) :infix(lsources)
	{

	}

std::vector<std::string> FMTexpression::replacevariables(const std::map<std::string, double>& mapping) const
	{
	if (!mapping.empty())
		{
		std::vector<std::string>newinfix;
		for (const std::string& invar : infix)
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
	return infix;
	}

double FMTexpression::evaluatepostfix(const std::vector<std::string>& postfix) const
	{
	std::stack<double>values;
	for (const std::string& post : postfix)
		{
		const FMToperator op(post);
		const FMTfunctioncall fcall(post);
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



std::vector<std::string>FMTexpression::getinfix() const
    {
    return infix;
    }

double FMTexpression::shuntingyard(const std::map<std::string, double>& mapping) const
	{
	double result = 0;
	if (!infix.empty())
		{
		const std::vector<std::string>newin = replacevariables(mapping);
		const std::vector<std::string>postfix = getpostfix(newin);
		result = evaluatepostfix(postfix);
		}
	return result;
	}

}
