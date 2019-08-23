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

FMTexpression::operator string() const
    {
    string exx = "";
    for (const string& value : infix)
        {
        exx+=" "+value;
        }
    exx.pop_back();
    return exx;
    }


vector<string> FMTexpression::getpostfix(const vector<string>& localinfix) const
	{
	stack<string> values;
	stack<bool>function_parenthesis;
	bool function_start = false;
	vector<string>infixes = localinfix;
	//values.push("(");
	/*infixes.insert(infixes.begin(), "(");*/
	//infixes.push_back(")");
	vector<string>postfix;
	//https://stackoverflow.com/questions/11708195/infix-to-postfix-with-function-support
	//http://interactivepython.org/runestone/static/pythonds/BasicDS/InfixPrefixandPostfixExpressions.html
	for (const string& invalue : infixes)
		{
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "inprocess : " << invalue << "\n";
		FMToperator op(invalue);
		FMTfunctioncall fcall(invalue);
		/*if (!op.valid() && !fcall.valid() && invalue != "("&& invalue != ")")
			{
			postfix.push_back(invalue);
			}*/
		if (fcall.valid())//isfunctioncall()
			{
			//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "function : " << invalue << "\n";
			//postfix.push_back(invalue);
			//FMToperator topop(values.top());
			function_start = true;
			values.push(invalue);
			}else if (op.valid())
				{

				//FMTfunctioncall topcall(values.top());
				//FMToperator topop(values.top());
				while (!values.empty() && ((FMToperator(values.top()).precedence() >= op.precedence() ) || FMTfunctioncall(values.top()).valid()) && values.top() != "(")
					{
					postfix.push_back(values.top());
					//topcall = FMTfunctioncall(values.top());
					//topop = FMToperator(values.top());
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
			/*}else if(invalue==",")
				{
				while (!values.empty() && values.top() !="(")
					{
					postfix.push_back(values.top());
					values.pop();
					}
				postfix.push_back(invalue);*/
			}else {
				postfix.push_back(invalue);
			}
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "stasck size : " << values.size() << "\n";
		}
	while (!values.empty())
		{
		/*if (values.top()!="(")
			{*/
			postfix.push_back(values.top());
			//}
		values.pop();
		}

		return postfix;
	}


bool FMTexpression::is_number(const string& s) const
    {
    string::const_iterator it = s.begin();
    while (it != s.end() && (isdigit(*it) || (*it)=='.')) ++it;
    return !s.empty() && it == s.end();
    }

 vector<string>FMTexpression::getvariables() const
    {
    vector<string>variables;
    for (const string& value : infix)
        {
        if(!is_number(value) && !FMToperator(value).valid() && !FMTfunctioncall(value).valid()) //assign 0 to all variables!
            {
            variables.push_back(value);
            }
        }
    return variables;
    }

FMTexpression FMTexpression::simplify(map<string,double>& values) const
    {
    map<string,double>shuntvalues;
    for (const string& value : infix)
        {
        if(!is_number(value) && !FMToperator(value).valid() && !value.empty()) //assign 0 to all variables!
            {
            shuntvalues[value] = 0;
            }
        }
    double rest = this->shuntingyard(shuntvalues);
    values["RHS"] = -rest;
    vector<string>new_expression;
    //for each variable find factor
    for (map<string,double>::iterator shuntit = shuntvalues.begin(); shuntit != shuntvalues.end(); shuntit++)
        {
        shuntit->second = 1;//set to 1
        double factorwrest = this->shuntingyard(shuntvalues);
        double variable_factor = (factorwrest - rest);
        if (variable_factor!=0)
            {
            values[shuntit->first] = variable_factor;
            new_expression.push_back(shuntit->first);
            new_expression.push_back("*");
             if (variable_factor > 0)
                {
                new_expression.push_back(to_string(variable_factor));
                }else{
                new_expression.push_back("-");
                new_expression.push_back(to_string(abs(variable_factor)));
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
            new_expression.push_back(to_string(rest));
            }else{
            new_expression.push_back("-");
            new_expression.push_back(to_string(abs(rest)));
            }
        }
    return FMTexpression(new_expression);
    }


FMTexpression::FMTexpression(const vector<string>& lsources) :infix(lsources)
	{

	}

vector<string> FMTexpression::replacevariables(const map<string, double>& mapping) const
	{
	if (!mapping.empty())
		{
		vector<string>newinfix;
		for (const string& invar : infix)
		{

			string result = invar;
			if (mapping.find(invar) != mapping.end())
			{
				result = to_string(mapping.at(invar));
			}
			newinfix.push_back(result);
		}
		return newinfix;
		}
	return infix;
	}

double FMTexpression::evaluatepostfix(const vector<string>& postfix) const
	{
	//here if its not opeator or functin it's a double!
	stack<double>values;
	for (const string& post : postfix)
		{
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "calculating : " << post << "\n";
		FMToperator op(post);
		FMTfunctioncall fcall(post);
		if (!op.valid() && !fcall.valid())
			{
			values.push(stod(post));
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
				//values.top();
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



vector<string>FMTexpression::getinfix() const
    {
    return infix;
    }

double FMTexpression::shuntingyard(const map<string, double>& mapping) const
	{
	double result = 0;
	if (!infix.empty())
		{
		vector<string>newin = replacevariables(mapping);
		/*string test = "";
		for (string var : newin)
			{
			test += var;
			}
		Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "EXPRESSION2: " << test << "\n";*/
		vector<string>postfix = getpostfix(newin);
		/*string test2 = "";
		for (const string ps : postfix)
			{
			test2 += ps;
			}
		Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "POSTfix is : " << test2 << "\n";*/
		result = evaluatepostfix(postfix);
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "POSTfix result : " << to_string(result) << "\n";


		}
	return result;
	}

}
