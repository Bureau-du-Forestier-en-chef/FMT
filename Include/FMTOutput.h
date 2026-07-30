/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMToutput_Hm_included
#define FMToutput_Hm_included

#include <vector>
#include <string>
#include <stack>
#include <set>
#include "FMTOutputSource.h"
#include "FMTOperator.h"
#include "FMTBounds.hpp"
#include "FMTObject.h"
#include <boost/serialization/string.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/export.hpp>

namespace Core {
	class FMTOutputNode;
	class FMTTheme;
}


namespace Core
{
// DocString: FMTOutput
/**
@brief Class letting the user formulate a constraint in the optimize section or collect data across the graph.
@details Holds a vector of output sources and operators. Non linear outputs cannot be used in matrix constraint formulation. Outputs have multiple output nodes representing a set of developments in the graph.
*/
class FMTEXPORT FMTOutput: public FMTObject
    {
	public:
		// DocString: FMTOutput()
		/**
		@brief Default constructor for FMTOutput.
		*/
		FMTOutput();
		// DocString: ~FMTOutput()
		/**
		@brief Default virtual destructor for FMTOutput.
		*/
		virtual ~FMTOutput() = default;
		// DocString: FMTOutput(const std::string&,const std::string&,const int&,std::vector<FMTOutputSource>&,std::vector<FMTOperator>&)
		/**
		@brief Complete constructor from a name, a description, a group, sources and operators.
		@param[in] lname the name.
		@param[in] ldescription the description.
		@param[in] lgroup the group.
		@param[in] lsources the output sources.
		@param[in] loperators the operators.
		*/
		FMTOutput(const std::string& lname, const std::string& ldescription,
			const std::string& lgroup,
			const std::vector<FMTOutputSource>& lsources,
			const std::vector<FMTOperator>& loperators);
		// DocString: FMTOutput(const std::string&)
		/**
		@brief Partial constructor from a name only.
		@param[in] lname the name.
		*/
		FMTOutput(const std::string& lname);
		// DocString: FMTOutput(const FMTOutput&)
		/**
		@brief Copy constructor for FMTOutput.
		@param[in] rhs the FMTOutput to copy.
		*/
		FMTOutput(const FMTOutput& rhs);
		// DocString: FMTOutput::operator=
		/**
		@brief Copy assignment operator for FMTOutput.
		@param[in] rhs the FMTOutput to copy.
		@return a reference to this FMTOutput.
		*/
		FMTOutput& operator = (const FMTOutput& rhs);
		// DocString: FMTOutput::operator==
		/**
		@brief Equality comparison operator of FMTOutput, checking if they have the same name.
		@param[in] rhs the output to compare with.
		@return true if both outputs have the same name else false.
		*/
		bool operator == (const FMTOutput& rhs) const;
		// DocString: FMTOutput::operator!=
		/**
		@brief Inequality comparison operator of FMTOutput, checking if they have a different name.
		@param[in] rhs the output to compare with.
		@return true if both outputs have a different name else false.
		*/
		bool operator != (const FMTOutput& rhs) const;
		// DocString: FMTOutput::operator+=
		/**
		@brief Addition assignment operator with another FMTOutput.
		@param[in] rhs the output to add.
		@return a reference to this FMTOutput.
		*/
		FMTOutput& operator +=(const FMTOutput& rhs);
		// DocString: FMTOutput::operator-=
		/**
		@brief Subtraction assignment operator with another FMTOutput.
		@param[in] rhs the output to subtract.
		@return a reference to this FMTOutput.
		*/
		FMTOutput& operator -=(const FMTOutput& rhs);
		// DocString: FMTOutput::operator*=
		/**
		@brief FMTOutput multiplication assignment with a simple double to multiply the FMTOutputSource.
		@param[in] p_source an output source of type value or time yield.
		@return a valid FMTOutput
		*/
		FMTOutput& operator *=(const FMTOutputSource& p_source);
		// DocString: FMTOutput::operator/=
		/**
		@brief FMTOutput division assignment with a source.
		@param[in] p_source an output source of type value or time yield.
		@return a valid FMTOutput
		*/
		FMTOutput& operator /=(const FMTOutputSource& p_source);
		// DocString: FMTOutput::operator std::string
		/**
		@brief Convert the output to a readable string as in an output section.
		@return the string representation of the output.
		*/
		operator std::string() const;
		// DocString: FMTOutput::getName
		/**
		@brief Return the name of the output.
		@return the name of the output.
		*/
		inline std::string getName() const
		{
			return m_name;
		}
		// DocString: FMTOutput::getDescription
		/**
		@brief Return the description of the output.
		@return the description of the output.
		*/
		inline std::string getDescription() const
		{
			return m_description;
		}
		// DocString: FMTOutput::getGroup
		/**
		@brief Return the group of the output.
		@return the group of the output.
		*/
		inline std::string getGroup() const
		{
			return m_group;
		}
		// DocString: FMTOutput::empty
		/**
		@brief Return true if the output has no output source.
		@return true if the output is empty else false.
		*/
		bool empty() const;
		// DocString: FMTOutput::size
		/**
		@brief Return the number of output sources of the output.
		@return the number of output sources.
		*/
		size_t size() const;
		// DocString: FMTOutput::isLinear
		/**
		@brief Return true if the output is linear, with no variable to variable multiplication or division.
		@return true if the output is linear else false.
		*/
		bool isLinear() const;
		// DocString: FMTOutput::canBeNodesOnly
		/**
		@brief Return true if the output can be used as nodes only in LP.
		@return true if the output can be nodes only else false.
		*/
		bool canBeNodesOnly() const;
		// DocString: FMTOutput::isLevel
		/**
		@brief Return true if one output source of the output is a level.
		@return true if the output has a level else false.
		*/
		bool isLevel() const;
		// DocString: FMTOutput::isOnlyLevel
		/**
		@brief Return true if all output sources of the output are levels.
		@return true if the output is only levels else false.
		*/
		bool isOnlyLevel() const;
		// DocString: FMTOutput::isConstantLevel
		/**
		@brief Return true if all output sources of the output are constant levels.
		@return true if the output is constant level else false.
		*/
		bool isConstantLevel() const;
		// DocString: FMTOutput::isConstant
		/**
		@brief Return true if all output sources of the output are constants.
		@return true if the output is constant else false.
		*/
		bool isConstant() const;
		// DocString: FMTOutput::getConstantValue
		/**
		@brief Return the constant value if the output is a single constant.
		@return the constant value.
		*/
		double getConstantValue() const;
		// DocString: FMTOutput::containsLevel
		/**
		@brief Return true if the output contains any level.
		@return true if the output contains a level else false.
		*/
		bool containsLevel() const;

		// DocString: FMTOutput::shuntingYard
		/**
		@brief Evaluate the output using the shunting yard algorithm with source values and operators.
		@param[in] sourcevalues the values of each output source.
		@param[in] simple_operators the operators.
		@return the evaluated value.
		*/
		double shuntingYard(const std::vector<double>& sourcevalues, const std::vector<FMTOperator>& simple_operators) const;
		// DocString: FMTOutput::boundTo
		/**
		@brief Bound the output to specific period bounds and return the new output, optionally targeting a theme.
		@param[in] themes the themes.
		@param[in] bound the period bounds.
		@param[in] specialbound the special bound.
		@param[in] attribute the targeted theme attribute.
		@return the bounded output.
		*/
		FMTOutput boundTo(const std::vector<FMTTheme>& themes, const FMTPerBounds& bound, const std::string& specialbound, std::string attribute = "") const;
		// DocString: FMTOutput::getNodes
		/**
		@brief Return the output nodes generated from the output sources and operators, optionally multiplied by a factor.
		@param[in,out] equation the equation.
		@param[in] multiplier the multiplier factor.
		@param[in] orderbyoutputid if true orders by output id.
		@param[in] period the period.
		@return the output nodes.
		*/
		std::vector<FMTOutputNode> getNodes(std::vector<std::string>& equation,
			double multiplier = 1,
			bool orderbyoutputid = false,
			int period = 1) const;
		// DocString: FMTOutput::isSinglePeriod
		/**
		@brief Return true if the output covers a single period of the graph.
		@return true if the output is single period else false.
		*/
		bool isSinglePeriod() const;
		// DocString: FMTOutput::hasAverage
		/**
		@brief Return true if the output needs to be averaged.
		@return true if the output has an average else false.
		*/
		bool hasAverage() const;
		// DocString: FMTOutput::getTargetPeriod
		/**
		@brief Return the targeted period of the output if it is single period.
		@return the target period.
		*/
		int getTargetPeriod() const;
		// DocString: FMTOutput::hash
		/**
		@brief Return the hash of the output.
		@return the hash value.
		*/
		size_t hash() const;
		// DocString: FMTOutput::setSources
		/**
		@brief Set new sources for the output.
		@param[in] p_sources the sources to set.
		*/
		void setSources(const std::vector<FMTOutputSource>& p_sources);
		// DocString: FMTOutput::setOperators
		/**
		@brief Set new operators for the output.
		@param[in] p_operators the operators to set.
		*/
		void setOperators(const std::vector<FMTOperator>& p_operators);
		// DocString: FMTOutput::getSources
		/**
		@brief Return a copy of the output sources of the output.
		@return the output sources.
		*/
		inline std::vector<FMTOutputSource> getSources() const
		{
			return m_sources;
		}
		// DocString: FMTOutput::getSourcesReference
		/**
		@brief Return a const reference to the output sources of the output.
		@return a const reference to the output sources.
		*/
		inline const std::vector<FMTOutputSource>& getSourcesReference() const
		{
			return m_sources;
		}
		// DocString: FMTOutput::getOpes
		/**
		@brief Return a copy of the operators of the output.
		@return the operators.
		*/
		inline std::vector<FMTOperator> getOpes() const
		{
			return m_operators;
		}
		// DocString: FMTOutput::targetTheme
		/**
		@brief Return the theme targeted by the output.
		@param[in] themes the model themes.
		@return the targeted theme.
		*/
		FMTTheme targetTheme(const std::vector<FMTTheme>& themes) const;
		// DocString: FMTOutput::targetThemeId
		/**
		@brief Return the id of the theme targeted by the output.
		@return the targeted theme id.
		*/
		inline int targetThemeId() const
		{
			return m_sources.begin()->getThemeTarget();
		}
		// DocString: FMTOutput::getDecomposition
		/**
		@brief Return all possible attribute values the output can have for the given theme.
		@param[in] theme the theme.
		@return the possible attribute values.
		*/
		std::vector<std::string>getThemeDecomposition(const FMTTheme& theme) const;
		// DocString: FMTOutput::getDecomposition
		/**
		@brief Return all possible attribute values the output can have if the attribute target is set, else an empty vector.
		@param[in] themes the model themes.
		@return the possible attribute values.
		*/
		std::vector<std::string>getDecomposition(const std::vector<FMTTheme>& themes) const;
		// DocString: FMTOutput::intersectWithMask
		/**
		@brief Return the output resulting from the intersection of a mask with this output.
		@param[in] mask the mask.
		@param[in] themes the themes.
		@return the intersected output.
		*/
		FMTOutput intersectWithMask(const Core::FMTMask& mask,
			const std::vector<Core::FMTTheme>& themes) const;
		// DocString: FMTOutput::getVariableIntersect
		/**
		@brief Return the intersected mask of all variables.
		@return the intersected mask.
		*/
		FMTMask getVariableIntersect() const;
		// DocString: FMTOutput::getMasksUnion
		/**
		@brief get the union of all masks of the sources and factor
		@return the union of the mask.
		*/
		FMTMask getMasksUnion() const;
		// DocString: FMTOutput::getStaticThemes()
		/**
		@brief Return the static themes of the whole output.
		@param[in] themes the themes.
		@param[in] yields the yields.
		@param[in] ignoreoutputvariables if true ignores the output variables.
		@return the static themes.
		*/
		std::vector<const Core::FMTTheme*>getStaticThemes(const std::vector<Core::FMTTheme>& themes, const Core::FMTYields& yields, bool ignoreoutputvariables = false) const;
		// DocString: FMTOutput::preSolve
		/**
		@brief Presolve the output and remove unused output sources.
		@param[in] filter the mask filter.
		@param[in] originalthemes the original themes.
		@param[in] selectedthemes the selected themes.
		@param[in] newthemes the presolved themes.
		@param[in] actions the actions.
		@param[in] p_valideActions the valid actions.
		@param[in] yields the yields.
		@return the presolved output.
		*/
		FMTOutput preSolve(const FMTMaskFilter& filter,
			const std::vector<FMTTheme>& originalthemes,
			const std::vector<const FMTTheme*>& selectedthemes,
			const std::vector<FMTTheme>& newthemes,
			const std::vector<FMTAction>& actions,
			const std::vector<bool>& p_valideActions,
			const FMTYields& yields) const;
		// DocString: FMTOutput::changeoutputsorigin
		/**
		@brief Change the output origin and target theme id of each source with the distance from the beginning of the set, normally used after preSolve.
		@param[in] newoutputsorigin the new output origins.
		@param[in] newthemeid the new theme ids.
		*/
		void changeSourcesId(const std::set<int>& newoutputsorigin, const std::set<int>& newthemeid);
		// DocString: FMTOutput::changeoutputsorigin
		/**
		@brief Change the output origin of each source, for the optimization parser.
		@param[in] outid the output origin to set.
		*/
		void changeSourcesId(const int& outid);
		// DocString: FMTOutput::extractRHSvalue
		/**
		@brief Return a new output removing the extra constants from a mix of noded output and constant level.
		@return the output without the RHS value.
		*/
		Core::FMTOutput removeRHSvalue() const;
		// DocString: FMTOutput::getRHSvalue
		/**
		@brief Get the RHS values mixing a level with a variable node for a given period.
		@param[in] period the period.
		@param[out] lower the lower value.
		@param[out] upper the upper value.
		*/
		void getRHSvalue(const int& period, double& lower, double& upper) const;

		// DocString: FMTOutput::setPeriod
		/**
		@brief Set the output sources to a single period.
		@param[in] newPeriod the period to set.
		*/
		void setPeriod(const int& newPeriod);
		// DocString: FMTOutput::isActionBased
		/**
		@brief Return true if the output contains action nodes.
		@return true if the output is action based else false.
		*/
		bool isActionBased() const;
		// DocString: FMTOutput::isVariableSizeOf
		/**
		@brief Return true if a variable source has a mask size equal to the given size, or if no source is variable.
		@param[in] masksize the mask size.
		@return true if the output is variable size of the given size else false.
		*/
		bool isVariableSizeOf(const size_t& masksize) const;
		// DocString: FMTOutput::isInventory
		/**
		@brief Return true if the output contains an inventory.
		@return true if the output contains an inventory else false.
		*/
		bool isInventory() const;
		// DocString: FMTOutput::fillFromShuntingYard
		/**
		@brief Get the values of a non linear output using the shunting yard algorithm.
		@param[in] baseeq the base equation.
		@param[in,out] results the results.
		@param[in] nodes the output nodes.
		@param[in,out] allequations the equations.
		*/
		void fillFromShuntingYard(
			const std::vector<std::string>baseeq,
			std::map<std::string, double>& results,
			const std::vector<Core::FMTOutputNode>& nodes,
			std::map<std::string, std::vector<std::string>>& allequations) const;

	protected:
		// DocString: FMTOutput::m_sources
		///outputsources data used to generate outputnodes
		std::vector<FMTOutputSource>m_sources;
		// DocString: FMTOutput::m_operators
		///vector of simple operators like +-*/
		std::vector<FMTOperator>m_operators;
		// DocString: FMTOutput::theme_target
		///Sometime output can specify multiple attributes of a given themes
		//int theme_target;
		// DocString: FMTOutput::m_name
		///This is the name of the output
		std::string m_name;
		// DocString: FMTOutput::m_description
		///This is description of the FMTOutput has seen in the output section.
		std::string m_description;
		// DocString: FMTOutput::m_group
		///This is the group of the output, if empty there's no group
		std::string m_group;
		// DocString: FMTOutput::_setProportions
		/**
		@brief Set the equation proportions for developments, turning numeric entries into proportions.
		@param[in,out] allequations the equations.
		@param[in] baseequation the base equation.
		*/
		void _setProportions(std::map<std::string, std::vector<std::string>>& allequations,
			const std::vector<std::string>& baseequation) const;
		// DocString: FMTOutput::_isDivision
		/**
		@brief Return true if the output contains a division.
		@return true if the output contains a division else false.
		*/
		bool _isDivision() const;
		// DocString: FMTOutput::_isDivision
		/**
		@brief Replace the division with the bound and reverse the denominator.
		@param[in] bound the bound.
		*/
		void _replaceDivision(const double& bound);
		// DocString: FMTOutput::_isValidAction
		/**
		@brief Return true if an action or aggregate is in the actions.
		@param[in] p_actionOrAggregate the name or aggregate of the action.
		@param[in] p_actions the full actions vector.
		@param[in] p_validActions the validity of the actions.
		@return true if the action is valid else false.
		*/
		static bool _isValidAction(const std::string& p_actionOrAggregate,
			const std::vector<FMTAction>& p_actions,
			const std::vector<bool>& p_validActions);
	private:
		// DocString: FMTOutput::serialize
		/**
		@brief Serialize the FMTOutput through its base FMTObject for multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to serialize to or from.
		@param[in] version the serialization version.
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			try {
				ar& boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<FMTObject>(*this));
				ar& boost::serialization::make_nvp("sources", m_sources);
				ar& boost::serialization::make_nvp("operators", m_operators);
				//ar & BOOST_SERIALIZATION_NVP(theme_target);
				ar& boost::serialization::make_nvp("name", m_name);
				ar& boost::serialization::make_nvp("description", m_description);
			}
			catch (...)
			{
				_exhandler->printExceptions("", "FMTOutput::serialize", __LINE__, __FILE__);
			}
		}
		// DocString: FMTOutput::_getFormatedOutputName()
		/**
		@brief get the name of the output and attribute and description in a string
		@return a string of the output name.
		*/
		std::string _getFormatedOutputName() const;
		// DocString: FMTOutput::_needWsFormat
		/**
		@brief check if the output needs to be converted to WS format (composed of inedges vs outedges sources)
		@return true if need to be converted else false.
		*/
		bool _needWsFormat() const;
		// DocString: FMTOutput::_toWsFormat
		/**
		@brief from a FMT output format split the outputs in multiple outputs that fit with the Woodstock Format.
		@return the output in string format for Woodstock.
		*/
		std::string _toWsFormat() const;
		// DocString: FMTOutput::_sourceCounter
		/**
		@brief Verify that no line in the given source string exceeds the maximum allowed length, raising an exception otherwise.
		@param[in] p_source the source string to check.
		@return true if all lines are within the allowed length else false.
		*/
		bool _sourceCounter(const std::string& p_source) const;
		// DocString: FMTOutput::_verifyOperatorOrder
		/**
		@brief Verify that the output does not have more than one operator (* or /).
		@return true if the operator order is valid else false.
		*/
		bool _verifyOperatorOrder() const;		
		// DocString: FMTOutput::_verifyOperatorOrder
		/**
		@brief Split the output with temporary outputs for operator management.
		@return a string with more than one output.
		*/
		std::string _operatorFormat() const;
    };
// DocString: FMTOutputComparator
/**
@brief Comparator used to check if an output name already exists in a std container.
*/
class FMTEXPORT FMTOutputComparator
{
public:
	// DocString: FMTOutputComparator()
	/**
	@brief Construct the comparator from the output name to find.
	@param[in] name the name of the output to find.
	*/
	FMTOutputComparator(std::string name);
	// DocString: FMTOutputComparator::operator()(const FMTOutput&)
	/**
	@brief Matching test operator for the comparator.
	@param[in] output the output to test.
	@return true if the output matches else false.
	*/
	bool operator()(const FMTOutput& output) const;

private:
	// DocString: FMTOutputComparator::m_outputName
	///The name of the FMTOutput we are looking for.
	std::string m_outputName;
};

}
BOOST_CLASS_EXPORT_KEY(Core::FMTOutput)
#endif // FMToutput_Hm_included
