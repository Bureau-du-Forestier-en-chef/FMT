/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMToutput.h"
#include "FMToutputnode.h"
#include "FMTtheme.h"
#include "FMTexpression.h"
#include "FMTyields.h"
#include "FMTyieldhandler.h"
#include <memory>
#include "FMTaction.h"
#include <algorithm>
#include <cpl_port.h>
#include <queue>
#include <boost/regex/config.hpp>
#include <array>
#include "FMTexceptionhandler.h"

namespace Core{


FMToutput::FMToutput(const std::string& lname,const std::string& ldescription,const std::string& lgroup,
	const std::vector<FMToutputsource>& lsources, const std::vector<FMToperator>& loperators):
	FMTobject(),
    sources(lsources),
    operators(loperators),
    name(lname),
    description(ldescription),
	group(lgroup)
    {

    }
FMToutput::FMToutput(const std::string& lname) :FMTobject(), sources(),operators(),name(lname),description(),group()
    {

    }
FMToutput::FMToutput() : FMTobject(),sources(),operators(),name(),description(),group()
    {

    }

FMToutput::FMToutput(const FMToutput& rhs) :
	FMTobject(rhs),
    sources(rhs.sources),
    operators(rhs.operators),
    name(rhs.name),
    description(rhs.description),
	group(rhs.group)
    {

    }
FMToutput& FMToutput::operator = (const FMToutput& rhs)
    {
    if (this!=&rhs)
        {
		FMTobject::operator=(rhs);
        name = rhs.name;
        sources = rhs.sources;
        operators = rhs.operators;
        description = rhs.description;
        }
    return *this;
    }
FMToutput& FMToutput::operator +=(const FMToutput& rhs)
	{
	try {
		if (!this->name.empty())
		{
			this->name = this->name + " + " + rhs.name;
			this->description = this->description + "+" + rhs.description;
			this->group = this->group + "+" + rhs.group;
		}
		else {
			this->name = rhs.name;
			this->description = rhs.description;
			this->group = rhs.group;
		}
		if (!this->sources.empty())
		{
			this->operators.push_back(FMToperator("+"));
		}
		this->sources.insert(this->sources.end(), rhs.sources.begin(), rhs.sources.end());

		this->operators.insert(this->operators.end(), rhs.operators.begin(), rhs.operators.end());
		/*if (this->theme_target != rhs.theme_target)
			{
			this->theme_target = -1;
			}*/
	}catch (...)
		{
		_exhandler->raisefromcatch(
			"", "FMToutput::operator+=", __LINE__, __FILE__, Core::FMTsection::Outputs);
		}
	return *this;
	}
FMToutput& FMToutput::operator -=(const FMToutput& rhs)
	{
	try{
	if (!this->name.empty())
		{
		this->name = this->name + " - " + rhs.name;
		this->description = this->description + "-" + rhs.description;
		this->group = this->group + "-" + rhs.group;
	}else {
		this->name = rhs.name;
		this->description = rhs.description;
		this->group = rhs.group;
		}
    if (!this->sources.empty())
		{
		this->operators.push_back(FMToperator("-"));
		}
	this->sources.insert(this->sources.end(), rhs.sources.begin(), rhs.sources.end());

	/*if (this->theme_target != rhs.theme_target)
		{
		this->theme_target = -1;
		}*/
	for (const FMToperator& rhsop : rhs.operators)
		{
		operators.push_back(rhsop.reverse());
		}
	}
	catch (...)
	{
		_exhandler->raisefromcatch(
			"", "FMToutput::operator-=", __LINE__, __FILE__, Core::FMTsection::Outputs);
	}
	return *this;
	}

FMToutput& FMToutput::operator  *= (const FMToutputsource& p_source)
	{
	try {
		if (!this->name.empty() && (!p_source.isconstant() || p_source.getvalue() != 1.0))
		{
			if (p_source.istimeyield())
			{
				this->name = this->name + " * " + std::string(p_source);
				this->description = this->description + " * " + std::string(p_source);
			}else {
				this->name = std::string(p_source) + " * " + this->name;
				this->description = std::string(p_source) + " * " + this->description;
			}
			this->group =this->group;
		}
		if (!sources.empty())
		{
			FMToutputsource LocalSource(p_source);
			std::vector<FMToutputsource>new_sources;
			std::vector<FMToperator>new_operators;
			size_t location = 0;
			for (const FMToutputsource& source : sources)
			{
				new_sources.push_back(source);
				if (source.isvariable() || source.islevel())
				{
					LocalSource.setthemetarget(source.getthemetarget());
					LocalSource.setoutputorigin(source.getoutputorigin());
					new_operators.push_back(FMToperator("*"));
					new_sources.push_back(LocalSource);
				}
				if (location < operators.size())
				{
					new_operators.push_back(operators.at(location));
				}

				++location;
			}
			sources = new_sources;
			operators = new_operators;
		}
	}
	catch (...)
	{
		_exhandler->raisefromcatch(
			"", "FMToutput::operator *=", __LINE__, __FILE__, Core::FMTsection::Outputs);
	}
	return *this;
	}

FMToutput& FMToutput::operator /=(const FMToutputsource& p_source)
	{
	try{
	if (!this->name.empty() && (!p_source.isconstant() || p_source.getvalue() != 1.0))
		{
		this->name = this->name + "/" + std::string(p_source);
		this->description = this->description + "/" + std::string(p_source);
		this->group = this->group + "/" + std::string(p_source);
		}
	if (!sources.empty())
	{
		FMToutputsource LocalSource(p_source);
		std::vector<FMToutputsource>new_sources;
		std::vector<FMToperator>new_operators;
		size_t location = 0;
		for (const FMToutputsource& source : sources)
		{
			new_sources.push_back(source);
			if (location < operators.size())
			{
				new_operators.push_back(operators.at(location));
			}
			if (source.isvariable() || source.isvariablelevel())
			{
				LocalSource.setthemetarget(source.getthemetarget());
				LocalSource.setoutputorigin(source.getoutputorigin());
				new_sources.push_back(LocalSource);
				new_operators.push_back(FMToperator("/"));
			}
			++location;
		}
		sources = new_sources;
		operators = new_operators;
	}
	}
	catch (...)
	{
		_exhandler->raisefromcatch(
			"", "FMToutput::operator/=", __LINE__, __FILE__, Core::FMTsection::Outputs);
	}
	return *this;
	}

FMToutput::operator std::string() const
    {
		std::string line;
	try{
		if (_needWsFormat())
		{
			line = _toWsFormat();
		}
		else if (_verifyOperatorOrder()){
			line  = _operatorFormat();
			//throw Exception::FMTexc::FMTunsupported_output;
		}
		else {
			line = _getFormatedOutputName();
			if (isconstantlevel())
			{
				line += "*SOURCE ";
				for (const double& value : sources.begin()->getvalues())
				{

					line += FMToutputsource::trimDouble(std::to_string(value)) + " ";
				}
				line.pop_back();
				line += "\n";

			}
			else if (!isonlylevel())
			{
				line += "*SOURCE ";
				for (size_t id = 0; id < sources.size(); ++id)
				{
					line += std::string(sources[id]) + " ";
					if (id < operators.size())
					{
						operators[id].isfactor() ? line += std::string(operators[id]) + " " : line += std::string(operators[id]) + "\n";
					}
				}
				line += "\n";
			}
		}
	}
	catch (...)
	{
		_exhandler->raisefromcatch(
			"", "FMToutput::operator std::string()", __LINE__, __FILE__, Core::FMTsection::Outputs);
	}
	
	if (_sourceCounter(line))
	{
		return line;
	}
	
	//return line;
    
    }
bool FMToutput::empty() const
	{
	return sources.empty();
	}
bool FMToutput::islevel() const
	{
	for (const FMToutputsource& src : sources)
		{
		if (src.gettarget() == FMTotar::level)
			{
			return true;
			}
		}
    return false;
	}

bool FMToutput::isconstantlevel() const
{
	for (const FMToutputsource& src : sources)
	{
		if (src.gettarget() != FMTotar::level || src.isvariablelevel())
		{
			return false;
		}
	}
	return true;
}

bool FMToutput::isonlylevel() const
	{
	for (const FMToutputsource& src : sources)
		{
		if (src.gettarget() != FMTotar::level)
			{
			return false;
			}
		}
    return true;
	}


bool FMToutput::isconstant() const
	{
	for (const FMToutputsource& src : sources)
		{
		if (src.gettarget() != FMTotar::val)
			{
			return false;
			}
		}
	return true;
	}

double FMToutput::getconstantvalue() const
	{
	double value = 0;
	try{
	std::vector<double>values;
	if (isconstant())
		{
		for (const FMToutputsource& src : sources)
			{
			values.push_back(src.getvalue());
			}
		value = shuntingyard(values, this->operators);
		}
	}
	catch (...)
	{
		_exhandler->raisefromcatch(
			"for "+this->getname(), "FMToutput::getconstantvalue", __LINE__, __FILE__, Core::FMTsection::Outputs);
	}
	return value;
	}


bool FMToutput::containslevel() const
	{
	for (const FMToutputsource& src : sources)
		{
		if (src.islevel())
			{
			return true;
			}
		}
	return false;
	}

bool FMToutput::isdivision() const
{
	for (const FMToperator& opr : operators)
	{
		if (opr.isdivide())
		{
			return true;
		}
	}
	return false;
}

void FMToutput::replacedivision(const double& bound)
{
	try {
	std::vector<Core::FMToperator>baseoperators(operators);
	baseoperators.insert(baseoperators.begin(), Core::FMToperator("+"));
	size_t opid = 0;
	bool denominator = false;
	bool gotonefactor = false;
	operators.clear();
	for (FMToutputsource& source : sources)
	{
		if (baseoperators.at(opid).isdivide())
		{
			denominator = true;
			operators.push_back(Core::FMToperator("+"));
		}
		else {
			operators.push_back(baseoperators.at(opid));
		}
		if (denominator)
		{
			if (!source.isvariable() && (source.isconstant()||source.islevel()))
			{
				if (!gotonefactor)
				{
					std::vector<double>allvalues = source.getvalues();
					for (double& value : allvalues)
					{
						value *= -bound;
					}
					source = FMToutputsource(source.gettarget(), allvalues, source.getoutputorigin(), source.getthemetarget());
				}
				gotonefactor = true;
			}
			else {
				gotonefactor = false;//Multiply the other variable...
			}
			
		}
		++opid;
	}
	operators.erase(operators.begin());
	}
	catch (...)
	{
		_exhandler->raisefromcatch("for " + this->getname(), "FMToutput::replacedivision",
			__LINE__, __FILE__, Core::FMTsection::Outputs);
	}
}

void FMToutput::setproportions(std::map<std::string, std::vector<std::string>>& allequations,const std::vector<std::string>& baseequation) const
{
	try {
		const double totalentry = static_cast<double>(allequations.size());
		std::vector<size_t>numericalentry;
		std::vector<std::string>newvalues;
		size_t location = 0;
		for (const std::string& equation : baseequation)
			{
			if ((*equation.begin())!='O'&&//Got numerical
				!FMToperator(equation).valid()&&
				(*equation.begin()) != '('&&
				(*equation.begin()) != ')')
				{
				double value = std::stod(equation);
				value /= totalentry;
				newvalues.push_back(std::to_string(value));
				numericalentry.push_back(location);
				}
			++location;
			}
		for (std::map<std::string, std::vector<std::string>>::iterator eqit = allequations.begin();eqit!= allequations.end();eqit++)
			{
			size_t loc = 0;
			for (const size_t& entryid : numericalentry)
				{
				eqit->second[entryid] = newvalues.at(loc);
				++loc;
				}
			}
	}catch (...)
		{
		_exhandler->raisefromcatch("for " + this->getname(),"FMToutput::setproportions",
			__LINE__, __FILE__, Core::FMTsection::Outputs);
		}
}

bool FMToutput::canbenodesonly() const
	{
	try {
		if (islinear()&& !isdivision())
		{
			std::vector<Core::FMToperator>baseoperators(operators);
			baseoperators.insert(baseoperators.begin(), Core::FMToperator("+"));
			size_t opid = 0;
			for (const FMToutputsource& source : sources)
				{
				if (((source.islevel()&&!source.isvariablelevel())||
					source.istimeyield()||
					source.isconstant())&&
					!baseoperators.at(opid).isfactor())
					{
					if (source.istimeyield())
						{
						_exhandler->raise(Exception::FMTexc::FMTunsupported_output,
							"for output " + std::string(*this),
							"FMToutput::canbenodesonly", __LINE__, __FILE__);
						}
					return false;
					}
				++opid;
				}
			return true;
		}
		
	}catch (...)
	{
		_exhandler->raisefromcatch("for " + this->getname(), "FMToutput::canbenodesonly",
			__LINE__, __FILE__, Core::FMTsection::Outputs);
	}
	return false;
	}

bool FMToutput::islinear() const
	{
	try{
	if (sources.size() > 1 && (find(operators.begin(), operators.end(), FMToperator("*")) != operators.end() ||
		find(operators.begin(), operators.end(), FMToperator("/")) != operators.end()))
		{
		int location = 0;
		int lastnonlinear = -1;
		for (const FMToutputsource& source : sources)
			{
			if (source.isvariable() && lastnonlinear!=-1 && location == (lastnonlinear+1))
				{
				return false;
				}
			if (source.isvariable() && location < static_cast<int>(operators.size()) &&
				(operators[location] == FMToperator("*") || operators[location] == FMToperator("/")))
				{
				lastnonlinear = location;
				}
			++location;
			}

		}
	}
	catch (...)
	{
		_exhandler->raisefromcatch(
			"for " + this->getname(), "FMToutput::islinear", __LINE__, __FILE__, Core::FMTsection::Outputs);
	}
	return true;
	}
double FMToutput::shuntingyard(const std::vector<double>& sourcevalues,const std::vector<FMToperator>& simple_operators) const
	{
	try {
		std::map<std::string, double>mapping;
		size_t id = 0;
		std::vector<std::string>expression_inputs;
		std::string test = "";
		for (const double& value : sourcevalues)
		{
			expression_inputs.push_back(std::to_string(value));
			test += std::to_string(value);
			if (id < simple_operators.size())
			{
				expression_inputs.push_back(simple_operators[id]);
				test += simple_operators[id];
			}
			++id;
		}
		const FMTexpression newexpression(expression_inputs);
		return newexpression.shuntingyard(mapping);
	}catch (...)
		{
		_exhandler->raisefromcatch("for " + this->getname(),"FMToutput::shuntingyard", __LINE__, __FILE__, Core::FMTsection::Outputs);
		}
	return 0;
	}

FMToutput FMToutput::boundto(const std::vector<FMTtheme>& themes, const FMTperbounds& bound,const std::string& specialbound, std::string attribute) const
	{
	FMToutput newoutput(*this);
	try {
		if (!attribute.empty())
		{
			newoutput.name = newoutput.name + "(" + attribute + ")";
		}
		if (specialbound.empty() && !bound.empty())
		{
			if ( bound.getlower() == bound.getupper())//single bounded
			{
				newoutput.name = newoutput.name + "[" + std::to_string(bound.getlower()) + "]";
			}
			/*/else if (!specialbound.empty())
			{
				std::string name = specialbound;
				name += "(" + newoutput.name;
				if (!(bound.getupper() == std::numeric_limits<double>::max() && bound.getlower() == 1) )
				{
					name += ",";
					name += std::to_string(bound.getlower()) + "..";
					name += std::to_string(bound.getupper());
				}
				name += ")";
				newoutput.name = name;
			}*/
		}
		if (!specialbound.empty())
		{
			std::string name = specialbound;
			name += "(" + newoutput.name;
			if (!bound.empty())
			{
				name += ",";
				name += std::to_string(bound.getlower()) + "..";
				name += std::to_string(bound.getupper());
			}
			name += ")";
			newoutput.name = name;
		}
		//if (!newoutput.islevel())
		//{
			for (FMToutputsource& source : newoutput.sources)
			{
				
				if (source.isvariable()||source.isvariablelevel())
				{
					/*if (source.isvariablelevel())
					{
						source = Core::FMToutputsource(Core::FMTotar::level, 0, "",
							newoutput.name, source.getoutputorigin(), source.getthemetarget());
					}*/
					if (!bound.empty())
					{
						source.setbounds(bound);
					}
					if (!attribute.empty())
					{
						if (source.isvariablelevel())
						{
							source.setaction(source.getaction() + "("+ attribute +")");
						}else {
							FMTmask oldmask = FMTmask(source.getmask());
							oldmask.set(themes.at(targetthemeid()), attribute);
							source.setmask(oldmask);
							}
						}
					if (!specialbound.empty())
					{
						if (specialbound == "_AVG")
						{
							source.setaverage();
						}
						else if (specialbound == "_SUM")
						{
							source.setsum();
						}
					}

					
				}/*else if (source.isvariablelevel())
					{
					source = Core::FMToutputsource(Core::FMTotar::level,0, "",
						newoutput.name, source.getoutputorigin(), source.getthemetarget());
					}*/
			}
		/*}else if(newoutput.islevel())
			{
			std::vector<FMToutputsource>levelsources;
			for (const FMToutputsource& source : newoutput.sources)
				{
				levelsources.push_back(Core::FMToutputsource(Core::FMTotar::level,source.getvalue(),"",
										newoutput.name, source.getoutputorigin(),source.getthemetarget()));
				}
			newoutput.sources = levelsources;
			}*/
	}catch (...)
		{
		_exhandler->raisefromcatch("for "+this->getname(),"FMToutput::boundto", __LINE__, __FILE__, Core::FMTsection::Outputs);
		}
	return newoutput;
	}

std::vector<FMToutputnode> FMToutput::getnodes(std::vector<std::string>& equation,double multiplier,bool orderbyoutputid,int period) const
	{
	//set a expression and get the nodes! check if the node is positive or negative accross the equation!!!
	std::vector<FMToutputnode>nodes;
	try {
		if (!empty())
		{
			size_t nodeid = 0;
			size_t sourceid = 0;
			equation.clear();
			//std::queue<FMToperator>ops;
			//ops.push(FMToperator("+"));
			std::deque<FMToperator>ops(operators.begin(), operators.end());
			//if (ops.empty() || ops.front().isfactor())
			//{
			ops.push_front(FMToperator("+"));
			//}

			/*for (const FMToperator& op : operators)
				{
				ops.push(op);
				}*/
			std::deque<FMToutputsource>srs(sources.begin(), sources.end());
			/*for (const FMToutputsource& sr : sources)
				{
				srs.push(sr);
				}*/
			bool pushednode = false;
			bool pushedfactor = false;
			while (!srs.empty())
			{
				if (srs.front().isvariable() || srs.front().isvariablelevel())
				{
					double constant = 1;
					//Its now handle in FMToutputnode settograph
					/*if (srs.front().isaverage())
						{
						constant *= multiplier;
						}8=*/
					if (!ops.front().isfactor())
					{
						constant *= ops.front().call(0, 1);
					}
					if (ops.front().isfactor())
					{
						equation.push_back(")");
						equation.push_back(ops.front());
						equation.push_back("(");
					}
					else {
						if (!pushednode)
						{
							equation.push_back("+");
						}
						else {
							equation.push_back(ops.front());
						}
					}
					equation.push_back("O" + std::to_string(nodes.size()));
					ops.pop_front();
					nodes.emplace_back(srs.front(),
						FMToutputsource(FMTotar::val, 1, "", "", srs.front().getoutputorigin()), constant);
					pushednode = true;
					pushedfactor = false;
				}
				else if (ops.front().isfactor() && (pushednode || pushedfactor))
				{
					if (srs.front().isconstant())
					{
						nodes.back().constant = ops.front().call(nodes.back().constant, srs.front().getvalue());
					}
					else {
						if (nodes.back().factor.istimeyield() &&
							srs.front().istimeyield())
						{
							_exhandler->raise(Exception::FMTexc::FMTunsupported_output,
								"for output " + std::string(*this),
								"FMToutput::getnodes", __LINE__, __FILE__);
						}
						nodes.back().factor.resetvalues(ops.front(), srs.front());
					}
					pushednode = false;
					pushedfactor = true;
					ops.pop_front();
				}
				else {
					equation.push_back(ops.front());
					const double value = srs.front().getvalue(period);
					equation.push_back(std::to_string(value));
					pushednode = false;
					pushedfactor = false;
					ops.pop_front();
				}
				srs.pop_front();
			}
			equation.erase(equation.begin());
			equation.insert(equation.begin(), "(");
			equation.push_back(")");
			if (orderbyoutputid)
			{
				std::sort(nodes.begin(), nodes.end(), FMToutputnodeorigincomparator());
			}
		}
	}catch (...)
		{
		_exhandler->raisefromcatch(
			"","FMToutput::getnodes", __LINE__, __FILE__, Core::FMTsection::Outputs);
		}
	return nodes;
	}

bool FMToutput::issingleperiod() const
	{
	for (const FMToutputsource& source : this->sources)
		{
		if (source.emptyperiod() || (source.getperiodlowerbound() != source.getperiodupperbound()))
			{
			return false;
			}
		}
	return true;
	}

bool FMToutput::hasaverage() const
{
return (name.find("_AVG") != std::string::npos);
}

int FMToutput::gettargetperiod() const
	{
	int target = -1;
	if (issingleperiod())
		{
		return sources.at(0).getperiodlowerbound();
		}
	return target;
	}

size_t FMToutput::hash() const
	{
	size_t seed = 0;
	for (const FMToperator& op : operators)
		{
		boost::hash_combine(seed,op.hash());
		}
	for (const FMToutputsource& src : sources)
		{
		boost::hash_combine(seed, src.hash());
		boost::hash_combine(seed, src.getyield());
		}
	return seed;
	}


bool FMToutput::operator == (const FMToutput& rhs) const
    {
    return (name == rhs.name ||
		(!sources.empty() && !rhs.empty() &&
		description == rhs.description &&
		targetthemeid() == rhs.targetthemeid() &&
		sources == rhs.sources &&
		operators == rhs.operators));
    }

bool FMToutput::operator != (const FMToutput& rhs) const
    {
    return !(*this== rhs);
    }

size_t FMToutput::size() const
	{
	return sources.size();
	}

FMToutput FMToutput::presolve(const FMTmaskfilter& filter,
	const std::vector<FMTtheme>& originalthemes,
	const std::vector<const FMTtheme*>& selectedthemes,
	const std::vector<FMTtheme>& newthemes,
	const std::vector<FMTaction>& actions, const FMTyields& yields) const
	{
	FMToutput newoutput(*this);
	try {
		std::vector<FMToutputsource>newsources;
		newsources.reserve(sources.size());
		std::vector<FMToperator>newoperators;
		newoperators.reserve(operators.size());
		size_t operatorid = 0;
		bool pushfactor = true;
		bool turntonegative = false;
		for (size_t sourceid = 0; sourceid < sources.size(); ++sourceid)
		{
			bool pushedsource = true;
			const std::string& yieldname = sources.at(sourceid).getyield();
			if (sources.at(sourceid).isvariable())
			{
				const std::string& actionname = sources.at(sourceid).getaction();
				
				if (filter.canpresolve(sources.at(sourceid).getmask(), selectedthemes) &&
					(actionname.empty() ||
						std::find_if(actions.begin(), actions.end(), FMTactioncomparator(actionname, true)) != actions.end()) && 
						(yieldname.empty() || !yields.isnullyld(yieldname)))
				{
					if (!filter.emptyflipped())
					{
						newsources.push_back(sources.at(sourceid).presolve(filter, newthemes));
						pushfactor = true;
					}else {
						pushedsource = false;
					}
					
				}
				else {
					pushedsource = false;
				}
			}
			else if (pushfactor&&!sources.at(sourceid).isvariable() && (sources.at(sourceid).islevel() || (sources.at(sourceid).istimeyield() && !yields.isnullyld(yieldname)) ||
				(sources.at(sourceid).isconstant())))
			{
				pushedsource = true;
				if (turntonegative)
				{
					newsources.push_back(FMToutputsource(sources.at(sourceid).gettarget(),
						sources.at(sourceid).getvalue()*-1,
						sources.at(sourceid).getyield(),
						sources.at(sourceid).getaction(),
						sources.at(sourceid).getoutputorigin(),
						sources.at(sourceid).getthemetarget()));
				}
				else {
					newsources.push_back(sources.at(sourceid));
				}
			}
			else {
				pushedsource = false;
			}
			turntonegative = false;
			if (operatorid < operators.size() && pushedsource)
			{
				newoperators.push_back(operators.at(operatorid));
			}else if (operatorid < operators.size() && !pushedsource
				&&operators.at(operatorid).isfactor())
			{
				pushfactor = false;
			}else if (operatorid < operators.size() && !pushedsource&&
				operators.at(operatorid).call(1,1)==0)
				{
				turntonegative = true;
				}
		++operatorid;
		}
		if (!newoperators.empty()&&
			newsources.size() == newoperators.size())//remove the last operator if it's the same size of the sources...
		{
			newoperators.erase(newoperators.end() - 1);
		}
		newoutput.sources.swap(newsources);
		newoutput.operators.swap(newoperators);
	}catch (...)
		{
		_exhandler->raisefromcatch("for "+this->getname(),"FMToutput::presolve", __LINE__, __FILE__, Core::FMTsection::Outputs);
		}
	return newoutput;
	}

void FMToutput::changesourcesid(const int& outid)
{
	try {
		for (Core::FMToutputsource& source : sources)
			{
			source.setoutputorigin(outid);
			}
	}catch (...)
		{
		_exhandler->raisefromcatch("for " + this->getname(),
			"FMToutput::changesourcesid", __LINE__, __FILE__, Core::FMTsection::Outputs);
		}
}

Core::FMToutput FMToutput::removeRHSvalue() const
	{
	Core::FMToutput newoutput(*this);
	double outrhs = 1;
	try {
		std::vector<Core::FMToperator>baseoperators(operators);
		baseoperators.insert(baseoperators.begin(), Core::FMToperator("+"));
		size_t opid = 0;
		std::vector<FMToutputsource>newsources;
		std::vector<Core::FMToperator>newoperators;
		for (const FMToutputsource& source : sources)
		{
			if (((source.islevel() && !source.isvariablelevel()) ||
				source.isconstant()) &&
				!baseoperators.at(opid).isfactor())
			{//get double and remove the rest

			}else {
				newsources.push_back(source);
				if (opid>0)
					{
					newoperators.push_back(baseoperators.at(opid));
					}
			}
			++opid;
		}
		newoutput.sources = newsources;
		newoutput.operators = newoperators;
	}catch (...)
		{
		_exhandler->raisefromcatch("for " + this->getname(),
			"removeRHSvalue", __LINE__, __FILE__, Core::FMTsection::Outputs);
		}
	return newoutput;
	}

void FMToutput::getRHSvalue(const int& period, double& lower, double& upper) const
{
	
	try {
		std::vector<double>toshunt;
		for (const FMToutputsource& source : sources)
		{

			if (((source.islevel() && !source.isvariablelevel()) ||
				source.isconstant()))
			{
				toshunt.push_back(source.getvalue(period));
			}else
			{
				toshunt.push_back(0);
			}

		}

		const double outrhs = this->shuntingyard(toshunt, operators);


		if (outrhs!=1)
		{
			if (lower != std::numeric_limits<double>::lowest())
			{
				lower -= outrhs;
			}
			if (upper != std::numeric_limits<double>::max())
			{
				upper -= outrhs;
			}
		}
		
	}
	catch (...)
	{
		_exhandler->raisefromcatch("for " + this->getname(),
			"getRHSvalue", __LINE__, __FILE__, Core::FMTsection::Outputs);
	}
}


void FMToutput::changesourcesid(const std::set<int>& newoutputsorigin,const std::set<int>& newthemeid)
	{
	try{
		for (Core::FMToutputsource& source : sources)
		{
			const int oldorigin = source.getoutputorigin();
			std::set<int>::const_iterator koit = newoutputsorigin.find(oldorigin);
			const int neworigin = static_cast<int>(std::distance(newoutputsorigin.begin(),koit));
			if (koit==newoutputsorigin.end() && !(source.getmask().empty()))
			{
				_exhandler->raise(Exception::FMTexc::FMTignore,"The outputorigin "+std::to_string(oldorigin)+" for the mask "+std::string(source.getmask())+" is not in the newsoutputorigin after presolve",
							"FMToutput::changeoutputsorigin", __LINE__, __FILE__);
			}else if(oldorigin!=neworigin)
			{
				source.setoutputorigin(neworigin);
			}
			const int oldthemetarget = source.getthemetarget();
			if (oldthemetarget >= 0)
			{
				std::set<int>::const_iterator ktit = newthemeid.find(oldthemetarget);
				int newtarget = static_cast<int>(std::distance(newthemeid.begin(),ktit));
				if (ktit== newthemeid.end())
				{
					newtarget = -1;
				}
				if (oldthemetarget!=newtarget)
				{
					source.setthemetarget(newtarget);
				}
			}
		}
	}catch (...)
		{
		_exhandler->raisefromcatch("for " + this->getname(), "FMToutput::changesourcesid", __LINE__, __FILE__, Core::FMTsection::Outputs);
		}
	}

void FMToutput::setsources(const std::vector<FMToutputsource>& p_sources)
	{
	sources = p_sources;
	}

void FMToutput::setOperators(const std::vector<FMToperator>& p_operators)
	{
	operators = p_operators;
	}

std::vector<std::string> FMToutput::getdecomposition(const std::vector<FMTtheme>& themes) const
	{
	std::vector<std::string>validdecomp;
	try{
	if (targetthemeid()!=-1)
		{
		return getthemedecomposition(themes[targetthemeid()]);
		/*int srcid = 0;
		for (const FMToutputsource& source : sources)
			{
			if (source.isvariable())
				{
				const FMTmask srcmask = source.getmask();
				std::vector<std::string>unique_selection;
				for (const FMTmask& decmask : srcmask.decompose(themes[targetthemeid()]))
					{
					unique_selection.push_back(decmask.get(themes[targetthemeid()]));
					}
				if (srcid==0)
					{
					validdecomp = unique_selection;
				}else {
					std::vector<std::string>newvalid;
					std::set_intersection(validdecomp.begin(), validdecomp.end(),
						unique_selection.begin(), unique_selection.end(), back_inserter(newvalid));
					validdecomp = newvalid;
				}
				++srcid;
				}
			}*/
		}
	}
	catch (...)
	{
		_exhandler->raisefromcatch("for " + this->getname(), "FMToutput::getdecomposition", __LINE__, __FILE__, Core::FMTsection::Outputs);
	}
	return validdecomp;
	}

std::vector<std::string> FMToutput::getthemedecomposition(const FMTtheme& theme) const
{
	std::vector<std::string>validdecomp;
	try {
		int srcid = 0;
		for (const FMToutputsource& source : sources)
		{
			if (source.isvariable())
			{
				const FMTmask srcmask = source.getmask();
				std::vector<std::string>unique_selection;
				for (const FMTmask& decmask : srcmask.decompose(theme))
				{
					unique_selection.push_back(decmask.get(theme));
				}
				if (srcid == 0)
				{
					validdecomp = unique_selection;
				}
				else {
					std::vector<std::string>newvalid;
					std::set_intersection(validdecomp.begin(), validdecomp.end(),
						unique_selection.begin(), unique_selection.end(), back_inserter(newvalid));
					validdecomp = newvalid;
				}
				++srcid;
			}
		}
	}
	catch (...)
	{
		_exhandler->raisefromcatch("for " + this->getname(), "FMToutput::getdecomposition", __LINE__, __FILE__, Core::FMTsection::Outputs);
	}
	return validdecomp;
}

FMToutput FMToutput::intersectwithmask(const Core::FMTmask& mask,
	const std::vector<Core::FMTtheme>& themes) const
	{
	FMToutput newoutput(*this);
	try{
		for (FMToutputsource& source : newoutput.sources)
		{
			if (source.isvariable())
			{
				Core::FMTmask newmask = source.getmask().getintersect(mask);
				bool outmask = false;
				for (const Core::FMTtheme& theme : themes)
					{
					if (newmask.getsubsetcount(theme)==0)
						{
						outmask = true;
						break;
						}
					}
				if (!outmask)
				{
					newmask.update(themes);
					source.setmask(newmask);
				}else {
					//if the intersect gives mask with only 0 in one theme, change source for level = 0 
					source = Core::FMToutputsource(Core::FMTotar::val,
						std::vector<double>(3,0),
						source.getoutputorigin(), source.getthemetarget());//constant level!
				}
			}
		}
		//if (getname() == "OSUPP7MREGECOCOS")
		//{
			//_exhandler->raise(Exception::FMTexc::FMTunsupported_output,
			//	"for output " + std::string(newoutput),
			//	"FMToutput::canbenodesonly", __LINE__, __FILE__);
		//}
	}catch (...)
		{
			_exhandler->raisefromcatch(
				"", "FMToutput::intersectwithmask", __LINE__, __FILE__, Core::FMTsection::Outputs);
		}
	return newoutput;
	}

FMTmask FMToutput::getvariableintersect() const
{
	FMTmask mask;
	try{
		for (const FMToutputsource& source :sources)
		{
			if (source.isvariable())
			{
				if (mask.empty())
					{
					mask = source.getmask();
				}else {
					mask = mask.getintersect(source.getmask());
				}
			
			}
		}
	}catch (...)
		{
		_exhandler->raisefromcatch(
			"", "FMToutput::getvariableintersect", __LINE__, __FILE__, Core::FMTsection::Outputs);
		}
	return mask;
}

std::vector<const Core::FMTtheme*>FMToutput::getstaticthemes(const std::vector<Core::FMTtheme>& themes, const Core::FMTyields& yields, bool ignoreoutputvariables) const
{
	std::vector<const Core::FMTtheme*>statics;
	try {
		std::vector<std::string>yieldstolookat;
		for (const FMToutputsource& source : sources)
		{
			if (source.isvariable())
			{
				if (!ignoreoutputvariables)
					{
					statics = source.getmask().getstaticthemes(statics);
					}
				const std::string yieldvalue = source.getyield();
				for (const std::string& yldbound : source.getylds())
					{
					if (yields.isyld(yldbound))
						{
						yieldstolookat.push_back(yldbound);
						}
					}
				if (!yieldvalue.empty())
					{
					yieldstolookat.push_back(yieldvalue);
					}
			}
		}
		std::vector< std::pair<FMTmask, std::unique_ptr<FMTyieldhandler>>>::const_iterator handlerit = yields.begin();
		while (handlerit!=yields.end()&&!yieldstolookat.empty())
			{
			std::vector<std::string>::const_iterator yieldit = yieldstolookat.begin();
			while (yieldit!= yieldstolookat.end() && !handlerit->second->containsyield(*yieldit))
				{
				++yieldit;
				}
			if (yieldit != yieldstolookat.end())
				{
				const Core::FMTmask maskof(std::string(handlerit->first), themes);
				const std::vector<const Core::FMTtheme*>newstatic = maskof.getstaticthemes(statics);
				statics = newstatic;
				yieldstolookat.erase(yieldit);
				}
			++handlerit;
			}
	}catch (...)
		{
			_exhandler->raisefromcatch(
				"", "FMToutput::getstaticthemes", __LINE__, __FILE__, Core::FMTsection::Outputs);
		}
	return statics;
	}


void FMToutput::setperiod(const int& newperiod)
	{
	for (FMToutputsource& source : sources)
		{
		source.setbounds(FMTperbounds(FMTsection::Outputs, newperiod, newperiod));
		}
	}

bool FMToutput::isactionbased() const
	{
	for (const FMToutputsource& source : sources)
		{
		if (!source.getaction().empty())
			{
			return true;
			}
		}
	return false;
	}

bool FMToutput::isvariablesizeof(const size_t& masksize) const
	{
	for (const FMToutputsource& source : sources)
		{
			if (source.isvariable())
			{
				return (masksize == source.getmask().size());
			}
		}
	return true;
	}


bool FMToutput::isinventory() const
	{
	for (const FMToutputsource& source : sources)
	{
		if (source.isinventory())
		{
			return true;
		}
	}
	return false;
	}

void FMToutput::fillfromshuntingyard(const std::vector<std::string>baseeq,
						std::map<std::string, double>& results,
						const std::vector<Core::FMToutputnode>& nodes,
						std::map<std::string,std::vector<std::string>>& allequations) const
	{
	try {
		setproportions(allequations,baseeq);
		for (std::map<std::string, std::vector<std::string>>::const_iterator outit = allequations.begin(); outit != allequations.end(); outit++)
		{
			size_t oid = 0;
			std::vector<std::string> equation(outit->second);
			for (const Core::FMToutputnode& output_node : nodes)
				{
				const std::string oldvalue = "O" + std::to_string(oid);
				const std::string newvalue("0");
				std::replace(equation.begin(), equation.end(),oldvalue, newvalue);
				++oid;
				}
			Core::FMTexpression expression(equation);
			std::map<std::string, double>vals;
			results[outit->first] = expression.shuntingyard(vals);
		}
	}catch (...)
		{
		_exhandler->raisefromcatch(
			"", "FMToutput::getfromshuntingyard", __LINE__, __FILE__, Core::FMTsection::Outputs);
		}
	}


FMTtheme FMToutput::targettheme(const std::vector<FMTtheme>& themes) const
	{
	if (targetthemeid()>=0)
		{
		return themes[targetthemeid()];
		}
	return FMTtheme();
	}


FMToutputcomparator::FMToutputcomparator(std::string name) : output_name(name)
	{

	}

bool FMToutputcomparator::operator()(const FMToutput& output) const
	{
	return output_name == output.getname();
	}

bool FMToutput::_sourceCounter(const std::string& p_source) const
{
	bool passed = true;
	const int MAXSIZE = 256;
	int count = 0;

	for (std::size_t i = 0; i < p_source.size(); ++i) {
		try
		{
			if (p_source[i] == '\n')
			{
				count = 0;
			}
			else
			{
				count++;
				if (count > MAXSIZE)
				{
					const std::string ERRORMESSAGE = "Le nombre de caratères par ligne est trop grand. Nombre de caractères : " + std::to_string(count) + " nombre max : " + std::to_string(MAXSIZE);
					_exhandler->raise(Exception::FMTexc::FMTrangeerror,
						 ERRORMESSAGE, "FMToutput::splitSource" ,__LINE__, __FILE__);
					passed = false;
				}

			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("",
				"FMToutput::splitSource",
				__LINE__, __FILE__);
		}
	}
	return passed;

}

bool FMToutput::_verifyOperatorOrder() const
{
	bool need_change = false;
	int count = 0;
	for (size_t i = 0; i < operators.size(); ++i) {
		Core::FMTokey op = operators[i].getkey();
		if (op == Core::FMTokey::multiply || op == Core::FMTokey::divide) {
			count++;
		}
		else if (op == Core::FMTokey::add || op == Core::FMTokey::sub) {
			count = 0;
		}
		if (count > 1) 
		{
			need_change = true;
			break;
		}
	}
	return need_change;
}

std::string FMToutput::_operatorFormat() const
{
	// setup
	Core::FMToutput newOutput(*this);
	std::string newLine;
	int op_count = 0;
	int output_num = 1;
	newLine += "*OUTPUT ~" + newOutput.getname() + " \n";
	newLine += "*SOURCE ";
	// On regarde le dernier double op * ou / pour ajuster les outputs en conséquence
	int last_operator_position = 0;
	for (size_t i = newOutput.operators.size(); i-- > 0; )
	{
		if (newOutput.operators[i].getkey() == Core::FMTokey::multiply ||
			newOutput.operators[i].getkey() == Core::FMTokey::divide)
		{
			last_operator_position ++;
		}
		else 
		{
			last_operator_position = 0;
		}
		if (last_operator_position > 1)
		{
			last_operator_position = i + 1;
			break;
		}
	}
	// On réécrit les outputs
	for (size_t i = 0; i < newOutput.sources.size(); ++i)
	{
		std::string source_str = std::string(newOutput.sources[i]);
		std::string op_str = newOutput.operators[i];
		Core::FMTokey op = operators[i].getkey();
		std::string lastNum = "";
		if (output_num > 1) 
		{
			lastNum = "_" + std::to_string(output_num - 1);
		}
		if (!source_str.empty())
		{
			newLine += source_str + " ";
		}
		if (op == Core::FMTokey::multiply || op == Core::FMTokey::divide) {
			op_count++;
		}
		else if (op == Core::FMTokey::add || op == Core::FMTokey::sub) {
			op_count = 0;
		}
		if (op_count < 2)
		{
			newLine += op_str + " ";
			if (op == Core::FMTokey::add || op == Core::FMTokey::sub) {
				newLine += "\n";
			}
		}
		else if (op_count == 2 && i < last_operator_position)
		{
			newLine += "\n\n";
			newLine += "*OUTPUT ~" + newOutput.getname() + "_" + std::to_string(output_num) + " \n";
			newLine += "*SOURCE ~" + newOutput.getname() + lastNum + " ";
			newLine += op_str + " ";
			op_count == 1;
			output_num++;
		}
		else if (op_count == 2 && i >= last_operator_position)
		{
			newLine += "\n\n";
			newLine += "*OUTPUT " + newOutput.getname() + " \n";
			newLine += "*SOURCE ~" + newOutput.getname() + lastNum + " ";
			newLine += op_str + " ";
			op_count == 1;
		}
	}
	newLine += " \n" ;
	return newLine;
}

std::string FMToutput::_getFormatedOutputName() const
{
	std::string result("*OUTPUT ");
	if (isonlylevel())
	{
		result = "*LEVEL ";
	}
	result += name;
	if (targetthemeid() != -1)
		{
		result += " (_TH" + std::to_string(targetthemeid() + 1) + ")";
		}
	if (targetthemeid() != -1)
		{
		result += " (_TH" + std::to_string(targetthemeid() + 1) + ")";
		}
	result += " " + description + "\n";
	return result;
}

bool FMToutput::_needWsFormat() const
{
	bool needit = false;
	if (!isconstant())
	{
		bool useInEdges = false;
		bool useOutEdges = false;
		for (const FMToutputsource& SOURCE : sources)
		{
			if (SOURCE.useinedges())
			{
				useInEdges = true;
			}
			else if (SOURCE.useoutedges())
			{
				useOutEdges = true;
			}
		}
		needit = useInEdges && useOutEdges;
	}
	return needit;
}

std::string FMToutput::_toWsFormat() const
	{
	std::string result;
	std::vector<Core::FMToperator>tempOperators;
	std::vector<Core::FMToutputsource>tempSources;
	std::string tempName;
	bool isInEdge = sources.begin()->useinedges();
	size_t outputId = 0;
	size_t operatorId = 0;
	std::string SumOutput(_getFormatedOutputName() +"*SOURCE ");
	std::array<std::string,2>outTypes{"OUT","IN"};
	for (const FMToutputsource& SOURCE : sources)
		{
		const bool SOURCE_TYPE = SOURCE.useinedges();
		bool packNGo = false;
		if (SOURCE_TYPE!=isInEdge && SOURCE.isvariable())//build an output
			{
			packNGo = true;
			isInEdge = SOURCE_TYPE;
			}else {//push into the existing output
				tempName = "~" + getname() + std::to_string(outputId) + outTypes.at(static_cast<size_t>(SOURCE_TYPE));
				tempSources.push_back(SOURCE);
				if (operatorId<operators.size())
					{
					tempOperators.push_back(operators.at(operatorId));
					}
				}
		if (packNGo)
			{
			Core::FMToperator outputOperator;
			//if (operatorId < operators.size())
			//	{
					outputOperator = tempOperators.back();
					tempOperators.pop_back();
			//	}
			const Core::FMToutput NEW_OUTPUT(tempName, "", getgroup(), tempSources, tempOperators);
			result += std::string(NEW_OUTPUT) + "\n";
			SumOutput += tempName +" " + std::string(outputOperator)+ " ";
			tempOperators.clear();
			tempSources.clear();
			tempName.clear();
			if (outputId != sources.size() - 1)
				{
					tempName = "~" + getname() + std::to_string(outputId) + outTypes.at(static_cast<size_t>(!SOURCE_TYPE));
					tempSources.push_back(SOURCE);
					if (operatorId < operators.size())
					{
						tempOperators.push_back(operators.at(operatorId));
					}
				}
			}

		if (outputId == sources.size() - 1)
			{
			if (packNGo)
				{
				tempSources.push_back(SOURCE);
				}
			tempName = "~" + getname() + std::to_string(outputId) + outTypes.at(static_cast<size_t>(isInEdge));
			const Core::FMToutput NEW_OUTPUT(tempName, "", getgroup(), tempSources, tempOperators);
			result += std::string(NEW_OUTPUT) + "\n";
			SumOutput += tempName + "\n";
			}
		
		/*if (outputId>1)
			{
			++operatorId;
			}*/
		++operatorId;
		++outputId;
		}
	result += SumOutput;
	return result;
	}


}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMToutput)
