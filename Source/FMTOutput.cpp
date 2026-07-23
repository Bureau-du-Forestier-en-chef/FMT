/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTOutput.h"
#include "FMTOutputNode.h"
#include "FMTTheme.h"
#include "FMTExpression.h"
#include "FMTYields.h"
#include "FMTYieldHandler.h"
#include <memory>
#include "FMTAction.h"
#include <algorithm>
#include <cpl_port.h>
#include <queue>
#include <boost/regex/config.hpp>
#include <array>
#include "FMTExceptionHandler.h"

namespace Core{


FMTOutput::FMTOutput(const std::string& lname,const std::string& ldescription,const std::string& lgroup,
	const std::vector<FMTOutputSource>& lsources, const std::vector<FMTOperator>& loperators):
	FMTObject(),
    m_sources(lsources),
    m_operators(loperators),
    m_name(lname),
    m_description(ldescription),
	m_group(lgroup)
    {

    }
FMTOutput::FMTOutput(const std::string& lname) :FMTObject(), m_sources(),m_operators(),m_name(lname),m_description(),m_group()
    {

    }
FMTOutput::FMTOutput() : FMTObject(),m_sources(),m_operators(),m_name(),m_description(),m_group()
    {

    }

FMTOutput::FMTOutput(const FMTOutput& rhs) :
	FMTObject(rhs),
    m_sources(rhs.m_sources),
    m_operators(rhs.m_operators),
    m_name(rhs.m_name),
    m_description(rhs.m_description),
	m_group(rhs.m_group)
    {

    }
FMTOutput& FMTOutput::operator = (const FMTOutput& rhs)
    {
    if (this!=&rhs)
        {
		FMTObject::operator=(rhs);
        m_name = rhs.m_name;
        m_sources = rhs.m_sources;
        m_operators = rhs.m_operators;
        m_description = rhs.m_description;
        }
    return *this;
    }
FMTOutput& FMTOutput::operator +=(const FMTOutput& rhs)
	{
	try {
		if (!this->m_name.empty())
		{
			this->m_name = this->m_name + " + " + rhs.m_name;
			this->m_description = this->m_description + "+" + rhs.m_description;
			this->m_group = this->m_group + "+" + rhs.m_group;
		}
		else {
			this->m_name = rhs.m_name;
			this->m_description = rhs.m_description;
			this->m_group = rhs.m_group;
		}
		if (!this->m_sources.empty())
		{
			this->m_operators.push_back(FMTOperator("+"));
		}
		this->m_sources.insert(this->m_sources.end(), rhs.m_sources.begin(), rhs.m_sources.end());

		this->m_operators.insert(this->m_operators.end(), rhs.m_operators.begin(), rhs.m_operators.end());
		/*if (this->theme_target != rhs.theme_target)
			{
			this->theme_target = -1;
			}*/
	}catch (...)
		{
		_exhandler->raiseFromCatch(
			"", "FMTOutput::operator+=", __LINE__, __FILE__, Core::FMTsection::Outputs);
		}
	return *this;
	}
FMTOutput& FMTOutput::operator -=(const FMTOutput& rhs)
	{
	try{
	if (!this->m_name.empty())
		{
		this->m_name = this->m_name + " - " + rhs.m_name;
		this->m_description = this->m_description + "-" + rhs.m_description;
		this->m_group = this->m_group + "-" + rhs.m_group;
	}else {
		this->m_name = rhs.m_name;
		this->m_description = rhs.m_description;
		this->m_group = rhs.m_group;
		}
    if (!this->m_sources.empty())
		{
		this->m_operators.push_back(FMTOperator("-"));
		}
	this->m_sources.insert(this->m_sources.end(), rhs.m_sources.begin(), rhs.m_sources.end());

	/*if (this->theme_target != rhs.theme_target)
		{
		this->theme_target = -1;
		}*/
	for (const FMTOperator& rhsop : rhs.m_operators)
		{
		m_operators.push_back(rhsop.reverse());
		}
	}
	catch (...)
	{
		_exhandler->raiseFromCatch(
			"", "FMTOutput::operator-=", __LINE__, __FILE__, Core::FMTsection::Outputs);
	}
	return *this;
	}

FMTOutput& FMTOutput::operator  *= (const FMTOutputSource& p_source)
	{
	try {
		if (!this->m_name.empty() && (!p_source.isConstant() || p_source.getValue() != 1.0))
		{
			if (p_source.isTimeYield())
			{
				this->m_name = this->m_name + " * " + std::string(p_source);
				this->m_description = this->m_description + " * " + std::string(p_source);
			}else {
				this->m_name = std::string(p_source) + " * " + this->m_name;
				this->m_description = std::string(p_source) + " * " + this->m_description;
			}
			this->m_group =this->m_group;
		}
		if (!m_sources.empty())
		{
			FMTOutputSource LocalSource(p_source);
			std::vector<FMTOutputSource>new_sources;
			std::vector<FMTOperator>new_operators;
			size_t location = 0;
			for (const FMTOutputSource& source : m_sources)
			{
				new_sources.push_back(source);
				if (source.isVariable() || source.isLevel())
				{
					LocalSource.setThemeTarget(source.getThemeTarget());
					LocalSource.setOutputOrigin(source.getOutputOrigin());
					new_operators.push_back(FMTOperator("*"));
					new_sources.push_back(LocalSource);
				}
				if (location < m_operators.size())
				{
					new_operators.push_back(m_operators.at(location));
				}

				++location;
			}
			m_sources = new_sources;
			m_operators = new_operators;
		}
	}
	catch (...)
	{
		_exhandler->raiseFromCatch(
			"", "FMTOutput::operator *=", __LINE__, __FILE__, Core::FMTsection::Outputs);
	}
	return *this;
	}

FMTOutput& FMTOutput::operator /=(const FMTOutputSource& p_source)
	{
	try{
	if (!this->m_name.empty() && (!p_source.isConstant() || p_source.getValue() != 1.0))
		{
		this->m_name = this->m_name + "/" + std::string(p_source);
		this->m_description = this->m_description + "/" + std::string(p_source);
		this->m_group = this->m_group + "/" + std::string(p_source);
		}
	if (!m_sources.empty())
	{
		FMTOutputSource LocalSource(p_source);
		std::vector<FMTOutputSource>new_sources;
		std::vector<FMTOperator>new_operators;
		size_t location = 0;
		for (const FMTOutputSource& source : m_sources)
		{
			new_sources.push_back(source);
			if (location < m_operators.size())
			{
				new_operators.push_back(m_operators.at(location));
			}
			if (source.isVariable() || source.isVariableLevel())
			{
				LocalSource.setThemeTarget(source.getThemeTarget());
				LocalSource.setOutputOrigin(source.getOutputOrigin());
				new_sources.push_back(LocalSource);
				new_operators.push_back(FMTOperator("/"));
			}
			++location;
		}
		m_sources = new_sources;
		m_operators = new_operators;
	}
	}
	catch (...)
	{
		_exhandler->raiseFromCatch(
			"", "FMTOutput::operator/=", __LINE__, __FILE__, Core::FMTsection::Outputs);
	}
	return *this;
	}

FMTOutput::operator std::string() const
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
				for (const double& value : m_sources.begin()->getValues())
				{

					line += FMTOutputSource::trimDouble(std::to_string(value)) + " ";
				}
				line.pop_back();
				line += "\n";

			}
			else if (!isOnlyLevel())
			{
				line += "*SOURCE ";
				for (size_t id = 0; id < m_sources.size(); ++id)
				{
					line += std::string(m_sources[id]) + " ";
					if (id < m_operators.size())
					{
						m_operators[id].isFactor() ? line += std::string(m_operators[id]) + " " : line += std::string(m_operators[id]) + "\n";
					}
				}
				line += "\n";
			}
		}
	}
	catch (...)
	{
		_exhandler->raiseFromCatch(
			"", "FMTOutput::operator std::string()", __LINE__, __FILE__, Core::FMTsection::Outputs);
	}
	
	if (_sourceCounter(line))
	{
		return line;
	}
	
	return std::string();
    
    }
bool FMTOutput::empty() const
	{
	return m_sources.empty();
	}
bool FMTOutput::isLevel() const
	{
	for (const FMTOutputSource& src : m_sources)
		{
		if (src.getTarget() == FMTotar::level)
			{
			return true;
			}
		}
    return false;
	}

bool FMTOutput::isConstantLevel() const
{
	for (const FMTOutputSource& src : m_sources)
	{
		if (src.getTarget() != FMTotar::level || src.isVariableLevel())
		{
			return false;
		}
	}
	return true;
}

bool FMTOutput::isOnlyLevel() const
	{
	for (const FMTOutputSource& src : m_sources)
		{
		if (src.getTarget() != FMTotar::level)
			{
			return false;
			}
		}
    return true;
	}


bool FMTOutput::isConstant() const
	{
	for (const FMTOutputSource& src : m_sources)
		{
		if (src.getTarget() != FMTotar::val)
			{
			return false;
			}
		}
	return true;
	}

double FMTOutput::getConstantValue() const
	{
	double value = 0;
	try{
	std::vector<double>values;
	if (isConstant())
		{
		for (const FMTOutputSource& src : m_sources)
			{
			values.push_back(src.getValue());
			}
		value = shuntingYard(values, this->m_operators);
		}
	}
	catch (...)
	{
		_exhandler->raiseFromCatch(
			"for "+this->getName(), "FMTOutput::getConstantValue", __LINE__, __FILE__, Core::FMTsection::Outputs);
	}
	return value;
	}


bool FMTOutput::containsLevel() const
	{
	for (const FMTOutputSource& src : m_sources)
		{
		if (src.isLevel())
			{
			return true;
			}
		}
	return false;
	}

bool FMTOutput::_isDivision() const
{
	for (const FMTOperator& opr : m_operators)
	{
		if (opr.isDivide())
		{
			return true;
		}
	}
	return false;
}

void FMTOutput::_replaceDivision(const double& bound)
{
	try {
	std::vector<Core::FMTOperator>baseoperators(m_operators);
	baseoperators.insert(baseoperators.begin(), Core::FMTOperator("+"));
	size_t opid = 0;
	bool denominator = false;
	bool gotonefactor = false;
	m_operators.clear();
	for (FMTOutputSource& source : m_sources)
	{
		if (baseoperators.at(opid).isDivide())
		{
			denominator = true;
			m_operators.push_back(Core::FMTOperator("+"));
		}
		else {
			m_operators.push_back(baseoperators.at(opid));
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
					source = FMTOutputSource(source.getTarget(), allvalues, source.getOutputOrigin(), source.getThemeTarget());
				}
				gotonefactor = true;
			}
			else {
				gotonefactor = false;//Multiply the other variable...
			}
			
		}
		++opid;
	}
	m_operators.erase(m_operators.begin());
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("for " + this->getName(), "FMTOutput::_replaceDivision",
			__LINE__, __FILE__, Core::FMTsection::Outputs);
	}
}

bool FMTOutput::_isValidAction(const std::string& p_actionOrAggregate,
	const std::vector<FMTAction>& p_actions,
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

void FMTOutput::_setProportions(std::map<std::string, std::vector<std::string>>& allequations,const std::vector<std::string>& baseequation) const
{
	try {
		const double totalentry = static_cast<double>(allequations.size());
		std::vector<size_t>numericalentry;
		std::vector<std::string>newvalues;
		size_t location = 0;
		for (const std::string& equation : baseequation)
			{
			if ((*equation.begin())!='O'&&//Got numerical
				!FMTOperator(equation).valid()&&
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
		_exhandler->raiseFromCatch("for " + this->getName(),"FMTOutput::_setProportions",
			__LINE__, __FILE__, Core::FMTsection::Outputs);
		}
}

bool FMTOutput::canBeNodesOnly() const
	{
	try {
		if (isLinear()&& !_isDivision())
		{
			std::vector<Core::FMTOperator>baseoperators(m_operators);
			baseoperators.insert(baseoperators.begin(), Core::FMTOperator("+"));
			size_t opid = 0;
			for (const FMTOutputSource& source : m_sources)
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
							"FMTOutput::canBeNodesOnly", __LINE__, __FILE__);
						}
					return false;
					}
				++opid;
				}
			return true;
		}
		
	}catch (...)
	{
		_exhandler->raiseFromCatch("for " + this->getName(), "FMTOutput::canBeNodesOnly",
			__LINE__, __FILE__, Core::FMTsection::Outputs);
	}
	return false;
	}

bool FMTOutput::isLinear() const
	{
	try{
	if (m_sources.size() > 1 && (find(m_operators.begin(), m_operators.end(), FMTOperator("*")) != m_operators.end() ||
		find(m_operators.begin(), m_operators.end(), FMTOperator("/")) != m_operators.end()))
		{
		int location = 0;
		int lastnonlinear = -1;
		for (const FMTOutputSource& source : m_sources)
			{
			if (source.isVariable() && lastnonlinear!=-1 && location == (lastnonlinear+1))
				{
				return false;
				}
			if (source.isVariable() && location < static_cast<int>(m_operators.size()) &&
				(m_operators[location] == FMTOperator("*") || m_operators[location] == FMTOperator("/")))
				{
				lastnonlinear = location;
				}
			++location;
			}

		}
	}
	catch (...)
	{
		_exhandler->raiseFromCatch(
			"for " + this->getName(), "FMTOutput::isLinear", __LINE__, __FILE__, Core::FMTsection::Outputs);
	}
	return true;
	}
double FMTOutput::shuntingYard(const std::vector<double>& sourcevalues,const std::vector<FMTOperator>& simple_operators) const
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
		const FMTExpression newexpression(expression_inputs);
		return newexpression.shuntingYard(mapping);
	}catch (...)
		{
		_exhandler->raiseFromCatch("for " + this->getName(),"FMTOutput::shuntingYard", __LINE__, __FILE__, Core::FMTsection::Outputs);
		}
	return 0;
	}

FMTOutput FMTOutput::boundTo(const std::vector<FMTTheme>& themes, const FMTPerBounds& bound,const std::string& specialbound, std::string attribute) const
	{
	FMTOutput newoutput(*this);
	try {
		if (!attribute.empty())
		{
			newoutput.m_name = newoutput.m_name + "(" + attribute + ")";
		}
		if (specialbound.empty() && !bound.empty())
		{
			if ( bound.getLower() == bound.getUpper())//single bounded
			{
				newoutput.m_name = newoutput.m_name + "[" + std::to_string(bound.getLower()) + "]";
			}
			/*/else if (!specialbound.empty())
			{
				std::string name = specialbound;
				name += "(" + newoutput.m_name;
				if (!(bound.getUpper() == std::numeric_limits<double>::max() && bound.getLower() == 1) )
				{
					name += ",";
					name += std::to_string(bound.getLower()) + "..";
					name += std::to_string(bound.getUpper());
				}
				name += ")";
				newoutput.m_name = name;
			}*/
		}
		if (!specialbound.empty())
		{
			std::string name = specialbound;
			name += "(" + newoutput.m_name;
			if (!bound.empty())
			{
				name += ",";
				name += std::to_string(bound.getLower()) + "..";
				name += std::to_string(bound.getUpper());
			}
			name += ")";
			newoutput.m_name = name;
		}
		//if (!newoutput.isLevel())
		//{
			for (FMTOutputSource& source : newoutput.m_sources)
			{
				
				if (source.isVariable()||source.isVariableLevel())
				{
					/*if (source.isVariableLevel())
					{
						source = Core::FMTOutputSource(Core::FMTotar::level, 0, "",
							newoutput.m_name, source.getOutputOrigin(), source.getThemeTarget());
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
							FMTMask oldmask = FMTMask(source.getMask());
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
					source = Core::FMTOutputSource(Core::FMTotar::level,0, "",
						newoutput.m_name, source.getOutputOrigin(), source.getThemeTarget());
					}*/
			}
		/*}else if(newoutput.isLevel())
			{
			std::vector<FMTOutputSource>levelsources;
			for (const FMTOutputSource& source : newoutput.m_sources)
				{
				levelsources.push_back(Core::FMTOutputSource(Core::FMTotar::level,source.getValue(),"",
										newoutput.m_name, source.getOutputOrigin(),source.getThemeTarget()));
				}
			newoutput.m_sources = levelsources;
			}*/
	}catch (...)
		{
		_exhandler->raiseFromCatch("for "+this->getName(),"FMTOutput::boundTo", __LINE__, __FILE__, Core::FMTsection::Outputs);
		}
	return newoutput;
	}

std::vector<FMTOutputNode> FMTOutput::getNodes(std::vector<std::string>& equation,double multiplier,bool orderbyoutputid,int period) const
	{
	//set a expression and get the nodes! check if the node is positive or negative accross the equation!!!
	std::vector<FMTOutputNode>nodes;
	try {
		if (!empty())
		{
			size_t nodeid = 0;
			size_t sourceid = 0;
			equation.clear();
			//std::queue<FMTOperator>ops;
			//ops.push(FMTOperator("+"));
			std::deque<FMTOperator>ops(m_operators.begin(), m_operators.end());
			//if (ops.empty() || ops.front().isFactor())
			//{
			ops.push_front(FMTOperator("+"));
			//}

			/*for (const FMTOperator& op : operators)
				{
				ops.push(op);
				}*/
			std::deque<FMTOutputSource>srs(m_sources.begin(), m_sources.end());
			/*for (const FMTOutputSource& sr : sources)
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
					//Its now handle in FMTOutputNode setToGraph
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
						FMTOutputSource(FMTotar::val, 1, "", "", srs.front().getOutputOrigin()), constant);
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
								"FMTOutput::getNodes", __LINE__, __FILE__);
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
				std::sort(nodes.begin(), nodes.end(), FMTOutputNodeOriginComparator());
			}
		}
	}catch (...)
		{
		_exhandler->raiseFromCatch(
			"","FMTOutput::getNodes", __LINE__, __FILE__, Core::FMTsection::Outputs);
		}
	return nodes;
	}

bool FMTOutput::isSinglePeriod() const
	{
	for (const FMTOutputSource& source : this->m_sources)
		{
		if (source.emptyPeriod() || (source.getPeriodLowerBound() != source.getPeriodUpperBound()))
			{
			return false;
			}
		}
	return true;
	}

bool FMTOutput::hasAverage() const
{
return (m_name.find("_AVG") != std::string::npos);
}

int FMTOutput::getTargetPeriod() const
	{
	int target = -1;
	if (isSinglePeriod())
		{
		return m_sources.at(0).getPeriodLowerBound();
		}
	return target;
	}

size_t FMTOutput::hash() const
	{
	size_t seed = 0;
	for (const FMTOperator& op : m_operators)
		{
		boost::hash_combine(seed,op.hash());
		}
	for (const FMTOutputSource& src : m_sources)
		{
		boost::hash_combine(seed, src.hash());
		boost::hash_combine(seed, src.getYield());
		}
	return seed;
	}


bool FMTOutput::operator == (const FMTOutput& rhs) const
    {
	// TODO gab mettre un bool sexy
	bool is_valid = false;
	if (m_name != rhs.m_name ||
		m_description != rhs.m_description ||
		m_sources != rhs.m_sources ||
		m_operators != rhs.m_operators)
	{
		return false;
	}
	if (!m_sources.empty() && !rhs.m_sources.empty())
	{
		if (targetThemeId() != rhs.targetThemeId()) {
			return false;
		}
	}
	return true;
    }

bool FMTOutput::operator != (const FMTOutput& rhs) const
    {
    return !(*this== rhs);
    }

size_t FMTOutput::size() const
	{
	return m_sources.size();
	}


FMTOutput FMTOutput::presolve(const FMTMaskFilter& filter,
	const std::vector<FMTTheme>& originalthemes,
	const std::vector<const FMTTheme*>& selectedthemes,
	const std::vector<FMTTheme>& newthemes,
	const std::vector<FMTAction>& actions,
	const std::vector<bool>& p_valideActions,
	const FMTYields& yields) const
	{
	FMTOutput newoutput(*this);
	try {
		std::vector<FMTOutputSource>newsources;
		newsources.reserve(m_sources.size());
		std::vector<FMTOperator>newoperators;
		newoperators.reserve(m_operators.size());
		size_t operatorid = 0;
		bool pushfactor = true;
		bool turntonegative = false;
		for (size_t sourceid = 0; sourceid < m_sources.size(); ++sourceid)
		{
			bool pushedsource = true;
			const std::string& yieldname = m_sources.at(sourceid).getYield();
			if (m_sources.at(sourceid).isVariable())
			{
				const std::string& actionname = m_sources.at(sourceid).getAction();
				const bool IS_VALId_ACTION = _isValidAction(actionname,actions, p_valideActions);
				if (filter.canPresolve(m_sources.at(sourceid).getMask(), selectedthemes) &&
					(actionname.empty() ||
						IS_VALId_ACTION) &&
						(yieldname.empty() || !yields.isNullYld(yieldname)))
				{
					if (!filter.emptyFlipped())
					{
						newsources.push_back(m_sources.at(sourceid).presolve(filter, newthemes));
						pushfactor = true;
					}else {
						pushedsource = false;
					}
					
				}
				else {
					pushedsource = false;
				}
			}
			else if (pushfactor&&!m_sources.at(sourceid).isVariable() && (m_sources.at(sourceid).isLevel() || (m_sources.at(sourceid).isTimeYield() && !yields.isNullYld(yieldname)) ||
				(m_sources.at(sourceid).isConstant())))
			{
				pushedsource = true;
				if (turntonegative)
				{
					newsources.push_back(FMTOutputSource(m_sources.at(sourceid).getTarget(),
						m_sources.at(sourceid).getValue()*-1,
						m_sources.at(sourceid).getYield(),
						m_sources.at(sourceid).getAction(),
						m_sources.at(sourceid).getOutputOrigin(),
						m_sources.at(sourceid).getThemeTarget()));
				}
				else {
					newsources.push_back(m_sources.at(sourceid));
				}
			}
			else {
				pushedsource = false;
			}
			turntonegative = false;
			if (operatorid < m_operators.size() && pushedsource)
			{
				newoperators.push_back(m_operators.at(operatorid));
			}else if (operatorid < m_operators.size() && !pushedsource
				&&m_operators.at(operatorid).isFactor())
			{
				pushfactor = false;
			}else if (operatorid < m_operators.size() && !pushedsource&&
				m_operators.at(operatorid).call(1,1)==0)
				{
				turntonegative = true;
				}
		++operatorid;
		}
		if (!newoperators.empty()&&
			newsources.size() == newoperators.size())//remove the last operator if it's the same size of the m_sources...
		{
			newoperators.erase(newoperators.end() - 1);
		}
		newoutput.m_sources.swap(newsources);
		newoutput.m_operators.swap(newoperators);
	}catch (...)
		{
		_exhandler->raiseFromCatch("for "+this->getName(),"FMTOutput::presolve", __LINE__, __FILE__, Core::FMTsection::Outputs);
		}
	return newoutput;
	}

void FMTOutput::changeSourcesId(const int& outid)
{
	try {
		for (Core::FMTOutputSource& source : m_sources)
			{
			source.setOutputOrigin(outid);
			}
	}catch (...)
		{
		_exhandler->raiseFromCatch("for " + this->getName(),
			"FMTOutput::changeSourcesId", __LINE__, __FILE__, Core::FMTsection::Outputs);
		}
}

Core::FMTOutput FMTOutput::removeRHSvalue() const
	{
	Core::FMTOutput newoutput(*this);
	double outrhs = 1;
	try {
		std::vector<Core::FMTOperator>baseoperators(m_operators);
		baseoperators.insert(baseoperators.begin(), Core::FMTOperator("+"));
		size_t opid = 0;
		std::vector<FMTOutputSource>newsources;
		std::vector<Core::FMTOperator>newoperators;
		for (const FMTOutputSource& source : m_sources)
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
		newoutput.m_sources = newsources;
		newoutput.m_operators = newoperators;
	}catch (...)
		{
		_exhandler->raiseFromCatch("for " + this->getName(),
			"removeRHSvalue", __LINE__, __FILE__, Core::FMTsection::Outputs);
		}
	return newoutput;
	}

void FMTOutput::getRHSvalue(const int& period, double& lower, double& upper) const
{
	
	try {
		std::vector<double>toshunt;
		for (const FMTOutputSource& source : m_sources)
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

		const double outrhs = this->shuntingYard(toshunt, m_operators);


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
		_exhandler->raiseFromCatch("for " + this->getName(),
			"getRHSvalue", __LINE__, __FILE__, Core::FMTsection::Outputs);
	}
}


void FMTOutput::changeSourcesId(const std::set<int>& newoutputsorigin,const std::set<int>& newthemeid)
	{
	try{
		for (Core::FMTOutputSource& source : m_sources)
		{
			const int oldorigin = source.getOutputOrigin();
			std::set<int>::const_iterator koit = newoutputsorigin.find(oldorigin);
			const int neworigin = static_cast<int>(std::distance(newoutputsorigin.begin(),koit));
			if (koit==newoutputsorigin.end() && !(source.getMask().empty()))
			{
				_exhandler->raise(Exception::FMTexc::FMTignore,"The outputorigin "+std::to_string(oldorigin)+" for the mask "+std::string(source.getMask())+" is not in the newsoutputorigin after presolve",
							"FMTOutput::changeoutputsorigin", __LINE__, __FILE__);
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
		_exhandler->raiseFromCatch("for " + this->getName(), "FMTOutput::changeSourcesId", __LINE__, __FILE__, Core::FMTsection::Outputs);
		}
	}

void FMTOutput::setSources(const std::vector<FMTOutputSource>& p_sources)
	{
	m_sources = p_sources;
	}

void FMTOutput::setOperators(const std::vector<FMTOperator>& p_operators)
	{
	m_operators = p_operators;
	}

std::vector<std::string> FMTOutput::getDecomposition(const std::vector<FMTTheme>& themes) const
	{
	std::vector<std::string>validdecomp;
	try{
	if (targetThemeId()!=-1)
		{
		return getThemeDecomposition(themes[targetThemeId()]);
		/*int srcid = 0;
		for (const FMTOutputSource& source : m_sources)
			{
			if (source.isVariable())
				{
				const FMTMask srcmask = source.getmask();
				std::vector<std::string>unique_selection;
				for (const FMTMask& decmask : srcmask.decompose(themes[targetThemeId()]))
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
		_exhandler->raiseFromCatch("for " + this->getName(), "FMTOutput::getDecomposition", __LINE__, __FILE__, Core::FMTsection::Outputs);
	}
	return validdecomp;
	}

std::vector<std::string> FMTOutput::getThemeDecomposition(const FMTTheme& theme) const
{
	std::vector<std::string>validdecomp;
	try {
		int srcid = 0;
		for (const FMTOutputSource& source : m_sources)
		{
			if (source.isVariable())
			{
				const FMTMask srcmask = source.getMask();
				const std::vector<FMTMask> ALL_MASKS = srcmask.decompose(theme);
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
		_exhandler->raiseFromCatch("for " + this->getName(), "FMTOutput::getDecomposition", __LINE__, __FILE__, Core::FMTsection::Outputs);
	}
	return validdecomp;
}

FMTOutput FMTOutput::intersectWithMask(const Core::FMTMask& mask,
	const std::vector<Core::FMTTheme>& themes) const
	{
	FMTOutput newoutput(*this);
	try{
		for (FMTOutputSource& source : newoutput.m_sources)
		{
			if (source.isVariable())
			{
				Core::FMTMask newmask = source.getMask().getIntersect(mask);
				bool outmask = false;
				for (const Core::FMTTheme& theme : themes)
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
					source = Core::FMTOutputSource(Core::FMTotar::val,
						std::vector<double>(3,0),
						source.getOutputOrigin(), source.getThemeTarget());//constant level!
				}
			}
		}
		//if (getName() == "OSUPP7MREGECOCOS")
		//{
			//_exhandler->raise(Exception::FMTexc::FMTunsupported_output,
			//	"for output " + std::string(newoutput),
			//	"FMTOutput::canBeNodesOnly", __LINE__, __FILE__);
		//}
	}catch (...)
		{
			_exhandler->raiseFromCatch(
				"", "FMTOutput::intersectWithMask", __LINE__, __FILE__, Core::FMTsection::Outputs);
		}
	return newoutput;
	}

FMTMask FMTOutput::getVariableIntersect() const
{
	FMTMask mask;
	try{
		for (const FMTOutputSource& source :m_sources)
		{
			if (source.isVariable())
			{
				if (mask.empty())
					{
					mask = source.getMask();
				}else {
					mask = mask.getIntersect(source.getMask());
				}
			
			}
		}
	}catch (...)
		{
		_exhandler->raiseFromCatch(
			"", "FMTOutput::getVariableIntersect", __LINE__, __FILE__, Core::FMTsection::Outputs);
		}
	return mask;
}

FMTMask FMTOutput::getMasksUnion() const
	{
	FMTMask mask;
	for (const FMTOutputSource& source : m_sources)
		{
		if (source.isVariable())
			{
				if (mask.empty())
				{
					mask = source.getMask();
				}
				else {
					mask = mask.getUnion(source.getMask());
				}

			}
		}
	return mask;
	}

std::vector<const Core::FMTTheme*>FMTOutput::getStaticThemes(const std::vector<Core::FMTTheme>& themes, const Core::FMTYields& yields, bool ignoreoutputvariables) const
{
	std::vector<const Core::FMTTheme*>statics;
	try {
		std::vector<std::string>yieldstolookat;
		for (const FMTOutputSource& source : m_sources)
		{
			if (source.isVariable())
			{
				if (!ignoreoutputvariables)
					{
					statics = source.getMask().getStaticThemes(statics);
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
		std::vector< std::pair<FMTMask, std::unique_ptr<FMTYieldHandler>>>::const_iterator handlerit = yields.begin();
		while (handlerit!=yields.end()&&!yieldstolookat.empty())
			{
			std::vector<std::string>::const_iterator yieldit = yieldstolookat.begin();
			while (yieldit!= yieldstolookat.end() && !handlerit->second->containsYield(*yieldit))
				{
				++yieldit;
				}
			if (yieldit != yieldstolookat.end())
				{
				const Core::FMTMask maskof(std::string(handlerit->first), themes);
				const std::vector<const Core::FMTTheme*>newstatic = maskof.getStaticThemes(statics);
				statics = newstatic;
				yieldstolookat.erase(yieldit);
				}
			++handlerit;
			}
	}catch (...)
		{
			_exhandler->raiseFromCatch(
				"", "FMTOutput::getStaticThemes", __LINE__, __FILE__, Core::FMTsection::Outputs);
		}
	return statics;
	}


void FMTOutput::setPeriod(const int& newperiod)
	{
	for (FMTOutputSource& source : m_sources)
		{
		source.setBounds(FMTPerBounds(FMTsection::Outputs, newperiod, newperiod));
		}
	}

bool FMTOutput::isActionbased() const
	{
	for (const FMTOutputSource& source : m_sources)
		{
		if (!source.getAction().empty())
			{
			return true;
			}
		}
	return false;
	}

bool FMTOutput::isVariableSizeOf(const size_t& masksize) const
	{
	for (const FMTOutputSource& source : m_sources)
		{
			if (source.isVariable())
			{
				return (masksize == source.getMask().size());
			}
		}
	return true;
	}


bool FMTOutput::isInventory() const
	{
	for (const FMTOutputSource& source : m_sources)
	{
		if (source.isInventory())
		{
			return true;
		}
	}
	return false;
	}

void FMTOutput::fillFromShuntingYard(const std::vector<std::string>baseeq,
						std::map<std::string, double>& results,
						const std::vector<Core::FMTOutputNode>& nodes,
						std::map<std::string,std::vector<std::string>>& allequations) const
	{
	try {
		_setProportions(allequations,baseeq);
		for (std::map<std::string, std::vector<std::string>>::const_iterator outit = allequations.begin(); outit != allequations.end(); outit++)
		{
			size_t oid = 0;
			std::vector<std::string> equation(outit->second);
			for (const Core::FMTOutputNode& output_node : nodes)
				{
				const std::string oldvalue = "O" + std::to_string(oid);
				const std::string newvalue("0");
				std::replace(equation.begin(), equation.end(),oldvalue, newvalue);
				++oid;
				}
			Core::FMTExpression expression(equation);
			std::map<std::string, double>vals;
			results[outit->first] = expression.shuntingYard(vals);
		}
	}catch (...)
		{
		_exhandler->raiseFromCatch(
			"", "FMTOutput::getfromshuntingyard", __LINE__, __FILE__, Core::FMTsection::Outputs);
		}
	}


FMTTheme FMTOutput::targetTheme(const std::vector<FMTTheme>& themes) const
	{
	if (targetThemeId()>=0)
		{
		return themes[targetThemeId()];
		}
	return FMTTheme();
	}


FMTOutputComparator::FMTOutputComparator(std::string name) : m_outputName(name)
	{

	}

bool FMTOutputComparator::operator()(const FMTOutput& output) const
	{
	return m_outputName == output.getName();
	}

bool FMTOutput::_sourceCounter(const std::string& p_source) const
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
						 ERRORMESSAGE, "FMTOutput::splitSource" ,__LINE__, __FILE__);
					passed = false;
				}*/

			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("",
				"FMTOutput::splitSource",
				__LINE__, __FILE__);
		}
	}
	return passed;

}

bool FMTOutput::_verifyOperatorOrder() const
{
	bool need_change = false;
	int count = 0;
	for (size_t i = 0; i < m_operators.size(); ++i) {
		Core::FMTokey op = m_operators[i].getKey();
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

std::string FMTOutput::_operatorFormat() const
{
	// setup
	Core::FMTOutput newOutput(*this);
	std::string tilde = "~";
	std::string modified = "";
	std::string newLine;
	int op_count = 0;
	int output_num = 0;

	if (newOutput.getName()[0] == '~')
	{
		tilde = "";
		modified = "_" + std::to_string(output_num);
		output_num += 1;
	}
	newLine += "*OUTPUT " + tilde + newOutput.getName();
	newLine += modified;
	newLine += " \n";
	newLine += "*SOURCE ";
	// On regarde le dernier double op * ou / pour ajuster les outputs en conséquence
	int last_operator_position = 0;
	for (size_t i = newOutput.m_operators.size(); i-- > 0; )
	{
		if (newOutput.m_operators[i].getKey() == Core::FMTokey::multiply ||
			newOutput.m_operators[i].getKey() == Core::FMTokey::divide)
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
	for (size_t i = 0; i < newOutput.m_sources.size(); ++i)
	{
		std::string source_str = std::string(newOutput.m_sources[i]);
		std::string op_str;
		Core::FMTokey op = Core::FMTokey::notvalid;
		if (i < newOutput.m_operators.size())
			{
			op_str  = newOutput.m_operators[i];
			op = m_operators[i].getKey();
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
			newLine += "*OUTPUT " + tilde + newOutput.getName() + "_" + std::to_string(output_num) + " \n";
			newLine += "*SOURCE " + tilde + newOutput.getName() + lastNum + " ";
			newLine += op_str + " ";
			op_count -= 1;
			output_num++;
		}
		else if (op_count == 2 && i >= last_operator_position)
		{
			newLine += "\n\n";
			newLine += "*OUTPUT " + newOutput.getName() + " \n";
			newLine += "*SOURCE " + tilde + newOutput.getName() + lastNum + " ";
			newLine += op_str + " ";
			op_count -= 1;
		}
	}
	newLine += " \n" ;
	return newLine;
}

std::string FMTOutput::_getFormatedOutputName() const
{
	std::string result("*OUTPUT ");
	if (isOnlyLevel())
	{
		result = "*LEVEL ";
	}
	result += m_name;
	if (targetThemeId() != -1)
		{
		result += " (_TH" + std::to_string(targetThemeId() + 1) + ")";
		}
	result += " " + m_description + "\n";
	return result;
}

bool FMTOutput::_needWsFormat() const
{
	bool needit = false;
	if (!isConstant())
	{
		bool useInEdges = false;
		bool useOutEdges = false;
		for (const FMTOutputSource& SOURCE : m_sources)
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

std::string FMTOutput::_toWsFormat() const
	{
	std::string result;
	std::vector<Core::FMTOperator>tempOperators;
	std::vector<Core::FMTOutputSource>tempSources;
	std::string tempName;
	bool isInEdge = m_sources.begin()->useInEdges();
	size_t outputId = 0;
	size_t operatorId = 0;
	std::string SumOutput(_getFormatedOutputName() +"*SOURCE ");
	std::array<std::string,2>outTypes{"OUT","IN"};
	for (const FMTOutputSource& SOURCE : m_sources)
		{
		const bool SOURCE_TYPE = SOURCE.useInEdges();
		bool packNGo = false;
		if (SOURCE_TYPE!=isInEdge && SOURCE.isVariable())//build an output
			{
			packNGo = true;
			isInEdge = SOURCE_TYPE;
			}else {//push into the existing output
				tempName = "~" + getName() + std::to_string(outputId) + outTypes.at(static_cast<size_t>(SOURCE_TYPE));
				tempSources.push_back(SOURCE);
				if (operatorId<m_operators.size())
					{
					tempOperators.push_back(m_operators.at(operatorId));
					}
				}
		if (packNGo)
			{
			Core::FMTOperator outputOperator;
			//if (operatorId < operators.size())
			//	{
					outputOperator = tempOperators.back();
					tempOperators.pop_back();
			//	}
			const Core::FMTOutput NEW_OUTPUT(tempName, "", getGroup(), tempSources, tempOperators);
			result += std::string(NEW_OUTPUT) + "\n";
			SumOutput += tempName +" " + std::string(outputOperator)+ " ";
			tempOperators.clear();
			tempSources.clear();
			tempName.clear();
			if (outputId != m_sources.size() - 1)
				{
					tempName = "~" + getName() + std::to_string(outputId) + outTypes.at(static_cast<size_t>(!SOURCE_TYPE));
					tempSources.push_back(SOURCE);
					if (operatorId < m_operators.size())
					{
						tempOperators.push_back(m_operators.at(operatorId));
					}
				}
			}

		if (outputId == m_sources.size() - 1)
			{
			if (packNGo)
				{
				tempSources.push_back(SOURCE);
				}
			tempName = "~" + getName() + std::to_string(outputId) + outTypes.at(static_cast<size_t>(isInEdge));
			const Core::FMTOutput NEW_OUTPUT(tempName, "", getGroup(), tempSources, tempOperators);
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

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTOutput)
