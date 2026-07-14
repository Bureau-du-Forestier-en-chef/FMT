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
		if (!this->name.empty() && (!p_source.isConstant() || p_source.getValue() != 1.0))
		{
			if (p_source.isTimeYield())
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
				if (source.isVariable() || source.isLevel())
				{
					LocalSource.setThemeTarget(source.getThemeTarget());
					LocalSource.setOutputOrigin(source.getOutputOrigin());
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
	if (!this->name.empty() && (!p_source.isConstant() || p_source.getValue() != 1.0))
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
			if (source.isVariable() || source.isVariableLevel())
			{
				LocalSource.setThemeTarget(source.getThemeTarget());
				LocalSource.setOutputOrigin(source.getOutputOrigin());
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
			if (isConstantLevel())
			{
				line += "*SOURCE ";
				for (const double& value : sources.begin()->getValues())
				{

					line += FMToutputsource::trimDouble(std::to_string(value)) + " ";
				}
				line.pop_back();
				line += "\n";

			}
			else if (!isOnlyLevel())
			{
				line += "*SOURCE ";
				for (size_t id = 0; id < sources.size(); ++id)
				{
					line += std::string(sources[id]) + " ";
					if (id < operators.size())
					{
						operators[id].isFactor() ? line += std::string(operators[id]) + " " : line += std::string(operators[id]) + "\n";
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
	
	return std::string();
    
    }
bool FMToutput::empty() const
	{
	return sources.empty();
	}
bool FMToutput::isLevel() const
	{
	for (const FMToutputsource& src : sources)
		{
		if (src.getTarget() == FMTotar::level)
			{
			return true;
			}
		}
    return false;
	}

bool FMToutput::isConstantLevel() const
{
	for (const FMToutputsource& src : sources)
	{
		if (src.getTarget() != FMTotar::level || src.isVariableLevel())
		{
			return false;
		}
	}
	return true;
}

bool FMToutput::isOnlyLevel() const
	{
	for (const FMToutputsource& src : sources)
		{
		if (src.getTarget() != FMTotar::level)
			{
			return false;
			}
		}
    return true;
	}


bool FMToutput::isConstant() const
	{
	for (const FMToutputsource& src : sources)
		{
		if (src.getTarget() != FMTotar::val)
			{
			return false;
			}
		}
	return true;
	}

double FMToutput::getConstantValue() const
	{
	double value = 0;
	try{
	std::vector<double>values;
	if (isConstant())
		{
		for (const FMToutputsource& src : sources)
			{
			values.push_back(src.getValue());
			}
		value = shuntingYard(values, this->operators);
		}
	}
	catch (...)
	{
		_exhandler->raisefromcatch(
			"for "+this->getname(), "FMToutput::getConstantValue", __LINE__, __FILE__, Core::FMTsection::Outputs);
	}
	return value;
	}


bool FMToutput::containsLevel() const
	{
	for (const FMToutputsource& src : sources)
		{
		if (src.isLevel())
			{
			return true;
			}
		}
	return false;
	}

bool FMToutput::isDivision() const
{
	for (const FMToperator& opr : operators)
	{
		if (opr.isDivide())
		{
			return true;
		}
	}
	return false;
}

void FMToutput::replaceDivision(const double& bound)
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
		if (baseoperators.at(opid).isDivide())
		{
			denominator = true;
			operators.push_back(Core::FMToperator("+"));
		}
		else {
			operators.push_back(baseoperators.at(opid));
		}
		if (denominator)
		{
			if (!source.isVariable() && (source.isConstant()||source.isLevel()))
			{
				if (!gotonefactor)
				{
					std::vector<double>allvalues = source.getValues();
					for (double& value : allvalues)
					{
						value *= -bound;
					}
					source = FMToutputsource(source.getTarget(), allvalues, source.getOutputOrigin(), source.getThemeTarget());
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
		_exhandler->raisefromcatch("for " + this->getname(), "FMToutput::replaceDivision",
			__LINE__, __FILE__, Core::FMTsection::Outputs);
	}
}

bool FMToutput::isValidAction(const std::string& p_actionOrAggregate,
	const std::vector<FMTaction>& p_actions,
	const std::vector<bool>& p_validActions)
{
	bool isValid = false;
	size_t Id = 0;
	while (!isValid && Id < p_actions.size())
		{
		if (p_validActions[Id] && p_actions[Id].isPartOf(p_actionOrAggregate))
			{
			isValid = true;
			}
		++Id;
		}
	return isValid;
}

void FMToutput::setProportions(std::map<std::string, std::vector<std::string>>& allequations,const std::vector<std::string>& baseequation) const
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
		_exhandler->raisefromcatch("for " + this->getname(),"FMToutput::setProportions",
			__LINE__, __FILE__, Core::FMTsection::Outputs);
		}
}

bool FMToutput::canBeNodesOnly() const
	{
	try {
		if (isLinear()&& !isDivision())
		{
			std::vector<Core::FMToperator>baseoperators(operators);
			baseoperators.insert(baseoperators.begin(), Core::FMToperator("+"));
			size_t opid = 0;
			for (const FMToutputsource& source : sources)
				{
				if (((source.isLevel()&&!source.isVariableLevel())||
					source.isTimeYield()||
					source.isConstant())&&
					!baseoperators.at(opid).isFactor())
					{
					if (source.isTimeYield())
						{
						_exhandler->raise(Exception::FMTexc::FMTunsupported_output,
							"for output " + std::string(*this),
							"FMToutput::canBeNodesOnly", __LINE__, __FILE__);
						}
					return false;
					}
				++opid;
				}
			return true;
		}
		
	}catch (...)
	{
		_exhandler->raisefromcatch("for " + this->getname(), "FMToutput::canBeNodesOnly",
			__LINE__, __FILE__, Core::FMTsection::Outputs);
	}
	return false;
	}

bool FMToutput::isLinear() const
	{
	try{
	if (sources.size() > 1 && (find(operators.begin(), operators.end(), FMToperator("*")) != operators.end() ||
		find(operators.begin(), operators.end(), FMToperator("/")) != operators.end()))
		{
		int location = 0;
		int lastnonlinear = -1;
		for (const FMToutputsource& source : sources)
			{
			if (source.isVariable() && lastnonlinear!=-1 && location == (lastnonlinear+1))
				{
				return false;
				}
			if (source.isVariable() && location < static_cast<int>(operators.size()) &&
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
			"for " + this->getname(), "FMToutput::isLinear", __LINE__, __FILE__, Core::FMTsection::Outputs);
	}
	return true;
	}
double FMToutput::shuntingYard(const std::vector<double>& sourcevalues,const std::vector<FMToperator>& simple_operators) const
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
		return newexpression.shuntingYard(mapping);
	}catch (...)
		{
		_exhandler->raisefromcatch("for " + this->getname(),"FMToutput::shuntingYard", __LINE__, __FILE__, Core::FMTsection::Outputs);
		}
	return 0;
	}

FMToutput FMToutput::boundTo(const std::vector<FMTtheme>& themes, const FMTperbounds& bound,const std::string& specialbound, std::string attribute) const
	{
	FMToutput newoutput(*this);
	try {
		if (!attribute.empty())
		{
			newoutput.name = newoutput.name + "(" + attribute + ")";
		}
		if (specialbound.empty() && !bound.empty())
		{
			if ( bound.getLower() == bound.getUpper())//single bounded
			{
				newoutput.name = newoutput.name + "[" + std::to_string(bound.getLower()) + "]";
			}
			/*/else if (!specialbound.empty())
			{
				std::string name = specialbound;
				name += "(" + newoutput.name;
				if (!(bound.getUpper() == std::numeric_limits<double>::max() && bound.getLower() == 1) )
				{
					name += ",";
					name += std::to_string(bound.getLower()) + "..";
					name += std::to_string(bound.getUpper());
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
				name += std::to_string(bound.getLower()) + "..";
				name += std::to_string(bound.getUpper());
			}
			name += ")";
			newoutput.name = name;
		}
		//if (!newoutput.isLevel())
		//{
			for (FMToutputsource& source : newoutput.sources)
			{
				
				if (source.isVariable()||source.isVariableLevel())
				{
					/*if (source.isVariableLevel())
					{
						source = Core::FMToutputsource(Core::FMTotar::level, 0, "",
							newoutput.name, source.getOutputOrigin(), source.getThemeTarget());
					}*/
					if (!bound.empty())
					{
						source.setBounds(bound);
					}
					if (!attribute.empty())
					{
						if (source.isVariableLevel())
						{
							source.setAction(source.getAction() + "("+ attribute +")");
						}else {
							FMTmask oldmask = FMTmask(source.getmask());
							oldmask.set(themes.at(targetThemeId()), attribute);
							source.setMask(oldmask);
							}
						}
					if (!specialbound.empty())
					{
						if (specialbound == "_AVG")
						{
							source.setAverage();
						}
						else if (specialbound == "_SUM")
						{
							source.setSum();
						}
					}

					
				}/*else if (source.isVariableLevel())
					{
					source = Core::FMToutputsource(Core::FMTotar::level,0, "",
						newoutput.name, source.getOutputOrigin(), source.getThemeTarget());
					}*/
			}
		/*}else if(newoutput.isLevel())
			{
			std::vector<FMToutputsource>levelsources;
			for (const FMToutputsource& source : newoutput.sources)
				{
				levelsources.push_back(Core::FMToutputsource(Core::FMTotar::level,source.getValue(),"",
										newoutput.name, source.getOutputOrigin(),source.getThemeTarget()));
				}
			newoutput.sources = levelsources;
			}*/
	}catch (...)
		{
		_exhandler->raisefromcatch("for "+this->getname(),"FMToutput::boundTo", __LINE__, __FILE__, Core::FMTsection::Outputs);
		}
	return newoutput;
	}

std::vector<FMToutputnode> FMToutput::getNodes(std::vector<std::string>& equation,double multiplier,bool orderbyoutputid,int period) const
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
			//if (ops.empty() || ops.front().isFactor())
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
				if (srs.front().isVariable() || srs.front().isVariableLevel())
				{
					double constant = 1;
					//Its now handle in FMToutputnode setToGraph
					/*if (srs.front().isAverage())
						{
						constant *= multiplier;
						}8=*/
					if (!ops.front().isFactor())
					{
						constant *= ops.front().call(0, 1);
					}
					if (ops.front().isFactor())
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
						FMToutputsource(FMTotar::val, 1, "", "", srs.front().getOutputOrigin()), constant);
					pushednode = true;
					pushedfactor = false;
				}
				else if (ops.front().isFactor() && (pushednode || pushedfactor))
				{
					if (srs.front().isConstant())
					{
						nodes.back().constant = ops.front().call(nodes.back().constant, srs.front().getValue());
					}
					else {
						if (nodes.back().factor.isTimeYield() &&
							srs.front().isTimeYield())
						{
							_exhandler->raise(Exception::FMTexc::FMTunsupported_output,
								"for output " + std::string(*this),
								"FMToutput::getNodes", __LINE__, __FILE__);
						}
						nodes.back().factor.resetValues(ops.front(), srs.front());
					}
					pushednode = false;
					pushedfactor = true;
					ops.pop_front();
				}
				else {
					equation.push_back(ops.front());
					const double value = srs.front().getValue(period);
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
			"","FMToutput::getNodes", __LINE__, __FILE__, Core::FMTsection::Outputs);
		}
	return nodes;
	}

bool FMToutput::isSinglePeriod() const
	{
	for (const FMToutputsource& source : this->sources)
		{
		if (source.emptyPeriod() || (source.getPeriodLowerBound() != source.getPeriodUpperBound()))
			{
			return false;
			}
		}
	return true;
	}

bool FMToutput::hasAverage() const
{
return (name.find("_AVG") != std::string::npos);
}

int FMToutput::getTargetPeriod() const
	{
	int target = -1;
	if (isSinglePeriod())
		{
		return sources.at(0).getPeriodLowerBound();
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
		boost::hash_combine(seed, src.getYield());
		}
	return seed;
	}


bool FMToutput::operator == (const FMToutput& rhs) const
    {
	// TODO gab mettre un bool sexy
	bool is_valid = false;
	if (name != rhs.name ||
		description != rhs.description ||
		sources != rhs.sources ||
		operators != rhs.operators)
	{
		return false;
	}
	if (!sources.empty() && !rhs.sources.empty())
	{
		if (targetThemeId() != rhs.targetThemeId()) {
			return false;
		}
	}
	return true;
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
	const std::vector<FMTaction>& actions,
	const std::vector<bool>& p_valideActions,
	const FMTyields& yields) const
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
			const std::string& yieldname = sources.at(sourceid).getYield();
			if (sources.at(sourceid).isVariable())
			{
				const std::string& actionname = sources.at(sourceid).getAction();
				const bool IS_VALId_ACTION = isValidAction(actionname,actions, p_valideActions);
				if (filter.canPresolve(sources.at(sourceid).getmask(), selectedthemes) &&
					(actionname.empty() ||
						IS_VALId_ACTION) &&
						(yieldname.empty() || !yields.isNullYld(yieldname)))
				{
					if (!filter.emptyFlipped())
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
			else if (pushfactor&&!sources.at(sourceid).isVariable() && (sources.at(sourceid).isLevel() || (sources.at(sourceid).isTimeYield() && !yields.isNullYld(yieldname)) ||
				(sources.at(sourceid).isConstant())))
			{
				pushedsource = true;
				if (turntonegative)
				{
					newsources.push_back(FMToutputsource(sources.at(sourceid).getTarget(),
						sources.at(sourceid).getValue()*-1,
						sources.at(sourceid).getYield(),
						sources.at(sourceid).getAction(),
						sources.at(sourceid).getOutputOrigin(),
						sources.at(sourceid).getThemeTarget()));
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
				&&operators.at(operatorid).isFactor())
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

void FMToutput::changeSourcesId(const int& outid)
{
	try {
		for (Core::FMToutputsource& source : sources)
			{
			source.setOutputOrigin(outid);
			}
	}catch (...)
		{
		_exhandler->raisefromcatch("for " + this->getname(),
			"FMToutput::changeSourcesId", __LINE__, __FILE__, Core::FMTsection::Outputs);
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
			if (((source.isLevel() && !source.isVariableLevel()) ||
				source.isConstant()) &&
				!baseoperators.at(opid).isFactor())
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

			if (((source.isLevel() && !source.isVariableLevel()) ||
				source.isConstant()))
			{
				toshunt.push_back(source.getValue(period));
			}else
			{
				toshunt.push_back(0);
			}

		}

		const double outrhs = this->shuntingYard(toshunt, operators);


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


void FMToutput::changeSourcesId(const std::set<int>& newoutputsorigin,const std::set<int>& newthemeid)
	{
	try{
		for (Core::FMToutputsource& source : sources)
		{
			const int oldorigin = source.getOutputOrigin();
			std::set<int>::const_iterator koit = newoutputsorigin.find(oldorigin);
			const int neworigin = static_cast<int>(std::distance(newoutputsorigin.begin(),koit));
			if (koit==newoutputsorigin.end() && !(source.getmask().empty()))
			{
				_exhandler->raise(Exception::FMTexc::FMTignore,"The outputorigin "+std::to_string(oldorigin)+" for the mask "+std::string(source.getmask())+" is not in the newsoutputorigin after presolve",
							"FMToutput::changeoutputsorigin", __LINE__, __FILE__);
			}else if(oldorigin!=neworigin)
			{
				source.setOutputOrigin(neworigin);
			}
			const int oldthemetarget = source.getThemeTarget();
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
					source.setThemeTarget(newtarget);
				}
			}
		}
	}catch (...)
		{
		_exhandler->raisefromcatch("for " + this->getname(), "FMToutput::changeSourcesId", __LINE__, __FILE__, Core::FMTsection::Outputs);
		}
	}

void FMToutput::setSources(const std::vector<FMToutputsource>& p_sources)
	{
	sources = p_sources;
	}

void FMToutput::setOperators(const std::vector<FMToperator>& p_operators)
	{
	operators = p_operators;
	}

std::vector<std::string> FMToutput::getDecomposition(const std::vector<FMTtheme>& themes) const
	{
	std::vector<std::string>validdecomp;
	try{
	if (targetThemeId()!=-1)
		{
		return getThemeDecomposition(themes[targetThemeId()]);
		/*int srcid = 0;
		for (const FMToutputsource& source : sources)
			{
			if (source.isVariable())
				{
				const FMTmask srcmask = source.getmask();
				std::vector<std::string>unique_selection;
				for (const FMTmask& decmask : srcmask.decompose(themes[targetThemeId()]))
					{
					unique_selection.push_back(decmask.get(themes[targetThemeId()]));
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
		_exhandler->raisefromcatch("for " + this->getname(), "FMToutput::getDecomposition", __LINE__, __FILE__, Core::FMTsection::Outputs);
	}
	return validdecomp;
	}

std::vector<std::string> FMToutput::getThemeDecomposition(const FMTtheme& theme) const
{
	std::vector<std::string>validdecomp;
	try {
		int srcid = 0;
		for (const FMToutputsource& source : sources)
		{
			if (source.isVariable())
			{
				const FMTmask srcmask = source.getmask();
				const std::vector<FMTmask> ALL_MASKS = srcmask.decompose(theme);
				std::vector<std::string>unique_selection(ALL_MASKS.size());
				for (size_t Id = 0; Id < ALL_MASKS.size();++Id)
				{
					unique_selection[Id] = ALL_MASKS[Id].get(theme);
				}
				if (srcid == 0)
				{
					validdecomp = unique_selection;
				}
				else {
					std::vector<std::string>newvalid;
					newvalid.reserve(validdecomp.size());
					std::sort(validdecomp.begin(), validdecomp.end());
					std::sort(unique_selection.begin(), unique_selection.end());
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
		_exhandler->raisefromcatch("for " + this->getname(), "FMToutput::getDecomposition", __LINE__, __FILE__, Core::FMTsection::Outputs);
	}
	return validdecomp;
}

FMToutput FMToutput::intersectWithMask(const Core::FMTmask& mask,
	const std::vector<Core::FMTtheme>& themes) const
	{
	FMToutput newoutput(*this);
	try{
		for (FMToutputsource& source : newoutput.sources)
		{
			if (source.isVariable())
			{
				Core::FMTmask newmask = source.getmask().getIntersect(mask);
				bool outmask = false;
				for (const Core::FMTtheme& theme : themes)
					{
					if (newmask.getSubsetCount(theme)==0)
						{
						outmask = true;
						break;
						}
					}
				if (!outmask)
				{
					newmask.update(themes);
					source.setMask(newmask);
				}else {
					//if the intersect gives mask with only 0 in one theme, change source for level = 0 
					source = Core::FMToutputsource(Core::FMTotar::val,
						std::vector<double>(3,0),
						source.getOutputOrigin(), source.getThemeTarget());//constant level!
				}
			}
		}
		//if (getname() == "OSUPP7MREGECOCOS")
		//{
			//_exhandler->raise(Exception::FMTexc::FMTunsupported_output,
			//	"for output " + std::string(newoutput),
			//	"FMToutput::canBeNodesOnly", __LINE__, __FILE__);
		//}
	}catch (...)
		{
			_exhandler->raisefromcatch(
				"", "FMToutput::intersectWithMask", __LINE__, __FILE__, Core::FMTsection::Outputs);
		}
	return newoutput;
	}

FMTmask FMToutput::getVariableIntersect() const
{
	FMTmask mask;
	try{
		for (const FMToutputsource& source :sources)
		{
			if (source.isVariable())
			{
				if (mask.empty())
					{
					mask = source.getmask();
				}else {
					mask = mask.getIntersect(source.getmask());
				}
			
			}
		}
	}catch (...)
		{
		_exhandler->raisefromcatch(
			"", "FMToutput::getVariableIntersect", __LINE__, __FILE__, Core::FMTsection::Outputs);
		}
	return mask;
}

FMTmask FMToutput::getMasksUnion() const
	{
	FMTmask mask;
	for (const FMToutputsource& source : sources)
		{
		if (source.isVariable())
			{
				if (mask.empty())
				{
					mask = source.getmask();
				}
				else {
					mask = mask.getUnion(source.getmask());
				}

			}
		}
	return mask;
	}

std::vector<const Core::FMTtheme*>FMToutput::getStaticThemes(const std::vector<Core::FMTtheme>& themes, const Core::FMTyields& yields, bool ignoreoutputvariables) const
{
	std::vector<const Core::FMTtheme*>statics;
	try {
		std::vector<std::string>yieldstolookat;
		for (const FMToutputsource& source : sources)
		{
			if (source.isVariable())
			{
				if (!ignoreoutputvariables)
					{
					statics = source.getmask().getStaticThemes(statics);
					}
				const std::string yieldvalue = source.getYield();
				for (const std::string& yldbound : source.getYlds())
					{
					if (yields.isYld(yldbound))
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
			while (yieldit!= yieldstolookat.end() && !handlerit->second->containsYield(*yieldit))
				{
				++yieldit;
				}
			if (yieldit != yieldstolookat.end())
				{
				const Core::FMTmask maskof(std::string(handlerit->first), themes);
				const std::vector<const Core::FMTtheme*>newstatic = maskof.getStaticThemes(statics);
				statics = newstatic;
				yieldstolookat.erase(yieldit);
				}
			++handlerit;
			}
	}catch (...)
		{
			_exhandler->raisefromcatch(
				"", "FMToutput::getStaticThemes", __LINE__, __FILE__, Core::FMTsection::Outputs);
		}
	return statics;
	}


void FMToutput::setPeriod(const int& newperiod)
	{
	for (FMToutputsource& source : sources)
		{
		source.setBounds(FMTperbounds(FMTsection::Outputs, newperiod, newperiod));
		}
	}

bool FMToutput::isActionbased() const
	{
	for (const FMToutputsource& source : sources)
		{
		if (!source.getAction().empty())
			{
			return true;
			}
		}
	return false;
	}

bool FMToutput::isVariableSizeOf(const size_t& masksize) const
	{
	for (const FMToutputsource& source : sources)
		{
			if (source.isVariable())
			{
				return (masksize == source.getmask().size());
			}
		}
	return true;
	}


bool FMToutput::isInventory() const
	{
	for (const FMToutputsource& source : sources)
	{
		if (source.isInventory())
		{
			return true;
		}
	}
	return false;
	}

void FMToutput::fillFromShuntingYard(const std::vector<std::string>baseeq,
						std::map<std::string, double>& results,
						const std::vector<Core::FMToutputnode>& nodes,
						std::map<std::string,std::vector<std::string>>& allequations) const
	{
	try {
		setProportions(allequations,baseeq);
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
			results[outit->first] = expression.shuntingYard(vals);
		}
	}catch (...)
		{
		_exhandler->raisefromcatch(
			"", "FMToutput::getfromshuntingyard", __LINE__, __FILE__, Core::FMTsection::Outputs);
		}
	}


FMTtheme FMToutput::targetTheme(const std::vector<FMTtheme>& themes) const
	{
	if (targetThemeId()>=0)
		{
		return themes[targetThemeId()];
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
	//const int MAXSIZE = 256;
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
				/*if (count > MAXSIZE)
				{
					const std::string ERRORMESSAGE = "Le nombre de caratères par ligne est trop grand. Nombre de caractères : " + std::to_string(count) + " nombre max : " + std::to_string(MAXSIZE);
					_exhandler->raise(Exception::FMTexc::FMTrangeerror,
						 ERRORMESSAGE, "FMToutput::splitSource" ,__LINE__, __FILE__);
					passed = false;
				}*/

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
		Core::FMTokey op = operators[i].getKey();
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
	std::string tilde = "~";
	std::string modified = "";
	std::string newLine;
	int op_count = 0;
	int output_num = 0;

	if (newOutput.getname()[0] == '~')
	{
		tilde = "";
		modified = "_" + std::to_string(output_num);
		output_num += 1;
	}
	newLine += "*OUTPUT " + tilde + newOutput.getname();
	newLine += modified;
	newLine += " \n";
	newLine += "*SOURCE ";
	// On regarde le dernier double op * ou / pour ajuster les outputs en conséquence
	int last_operator_position = 0;
	for (size_t i = newOutput.operators.size(); i-- > 0; )
	{
		if (newOutput.operators[i].getKey() == Core::FMTokey::multiply ||
			newOutput.operators[i].getKey() == Core::FMTokey::divide)
		{
			last_operator_position ++;
		}
		else 
		{
			last_operator_position = 0;
		}
		if (last_operator_position > 1)
		{
			last_operator_position = static_cast<int>(i) + 1;
			break;
		}
	}
	// On réécrit les outputs
	for (size_t i = 0; i < newOutput.sources.size(); ++i)
	{
		std::string source_str = std::string(newOutput.sources[i]);
		std::string op_str;
		Core::FMTokey op = Core::FMTokey::notvalid;
		if (i < newOutput.operators.size())
			{
			op_str  = newOutput.operators[i];
			op = operators[i].getKey();
			}
		std::string lastNum = "";
		if (output_num > 0) 
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
			newLine += "*OUTPUT " + tilde + newOutput.getname() + "_" + std::to_string(output_num) + " \n";
			newLine += "*SOURCE " + tilde + newOutput.getname() + lastNum + " ";
			newLine += op_str + " ";
			op_count -= 1;
			output_num++;
		}
		else if (op_count == 2 && i >= last_operator_position)
		{
			newLine += "\n\n";
			newLine += "*OUTPUT " + newOutput.getname() + " \n";
			newLine += "*SOURCE " + tilde + newOutput.getname() + lastNum + " ";
			newLine += op_str + " ";
			op_count -= 1;
		}
	}
	newLine += " \n" ;
	return newLine;
}

std::string FMToutput::_getFormatedOutputName() const
{
	std::string result("*OUTPUT ");
	if (isOnlyLevel())
	{
		result = "*LEVEL ";
	}
	result += name;
	if (targetThemeId() != -1)
		{
		result += " (_TH" + std::to_string(targetThemeId() + 1) + ")";
		}
	result += " " + description + "\n";
	return result;
}

bool FMToutput::_needWsFormat() const
{
	bool needit = false;
	if (!isConstant())
	{
		bool useInEdges = false;
		bool useOutEdges = false;
		for (const FMToutputsource& SOURCE : sources)
		{
			if (SOURCE.useInEdges())
			{
				useInEdges = true;
			}
			else if (SOURCE.useOutEdges())
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
	bool isInEdge = sources.begin()->useInEdges();
	size_t outputId = 0;
	size_t operatorId = 0;
	std::string SumOutput(_getFormatedOutputName() +"*SOURCE ");
	std::array<std::string,2>outTypes{"OUT","IN"};
	for (const FMToutputsource& SOURCE : sources)
		{
		const bool SOURCE_TYPE = SOURCE.useInEdges();
		bool packNGo = false;
		if (SOURCE_TYPE!=isInEdge && SOURCE.isVariable())//build an output
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
			const Core::FMToutput NEW_OUTPUT(tempName, "", getGroup(), tempSources, tempOperators);
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
			const Core::FMToutput NEW_OUTPUT(tempName, "", getGroup(), tempSources, tempOperators);
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
