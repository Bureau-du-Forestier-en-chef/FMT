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

#ifndef FMTOUTPUTNODE_H_INCLUDED
#define FMTOUTPUTNODE_H_INCLUDED

#include "FMToutputsource.h"
#include "FMToperator.h"
#include <map>

namespace Core
{
class FMToutputnode
	{
	public:
		FMToutputsource source;
		FMToutputsource factor;
		double constant;
		FMToutputnode();
		FMToutputnode(const FMToutputsource& lsource, const FMToutputsource& lfactor, const double& lconstant);
		FMToutputnode(const FMToutputnode& rhs);
		size_t hash() const;
		bool allowcashdeduction() const;
		bool singleperiod() const;
		bool multiperiod() const;
		bool ispastperiod() const;
		bool isnull() const;
		bool operator < (const FMToutputnode& rhs) const;
		bool operator == (const FMToutputnode& rhs) const;
		bool issubsetof(const FMToutputnode& rhs) const;
		bool issubsetof(const FMToutputnode& rhs,const std::vector<Core::FMTaction>& actions) const;
		bool issamebutdifferentaction(const FMToutputnode& rhs) const;
		operator std::string() const;
		FMToutputnode setperiod(int period) const;
		FMToutputnode& operator = (const FMToutputnode& rhs);
		int settograph(std::vector<int>& targetedperiods, int period,int max_period);
		~FMToutputnode()=default;
	};

class FMToutputnodehashcomparator
	{
		size_t sourcehash;
	public:
		FMToutputnodehashcomparator(size_t hash);
		bool operator()(const FMToutputnode& node) const;

	};

}

#endif
