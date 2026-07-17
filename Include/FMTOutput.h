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
FMTOutput let the user formulate constraint in the optimize section or just collect data across the FMTGraph.
FMTOutput hold a vector of outputsources and operators. Outputs  that are non linear cannot be used into
matrix constraints formulation. Outputs have multiple outputs node representing a set of FMTDevelopment in the 
FMTGraph. Each FMTDevelopment can be part of one FMTOutput.
*/
class FMTEXPORT FMTOutput: public FMTObject
    {
	public:
		// DocString: FMTOutput()
		/**
		Default constructor for FMTOutput
		*/
		FMTOutput();
		// DocString: ~FMTOutput()
		/**
		Default destructor for FMTOutput
		*/
		virtual ~FMTOutput() = default;
		// DocString: FMTOutput(const std::string&,const std::string&,const int&,std::vector<FMTOutputSource>&,std::vector<FMTOperator>&)
		/**
		Constructor for FMTOutput for a complete construction for (name) (description),
		theme_target,sources and operators.
		*/
		FMTOutput(const std::string& lname, const std::string& ldescription,
			const std::string& lgroup,
			const std::vector<FMTOutputSource>& lsources,
			const std::vector<FMTOperator>& loperators);
		// DocString: FMTOutput(const std::string&)
		/**
		Partial constructor for FMTOutput with only name
		*/
		FMTOutput(const std::string& lname);
		// DocString: FMTOutput(const FMTOutput&)
		/**
		FMTOutput copy constructor
		*/
		FMTOutput(const FMTOutput& rhs);
		// DocString: FMTOutput::operator=
		/**
		Copy assignement of FMTOutput
		*/
		FMTOutput& operator = (const FMTOutput& rhs);
		// DocString: FMTOutput::operator==
		/**
		FMTOutput equality operator check if FMTOutput (rhs) have the same name.
		*/
		bool operator == (const FMTOutput& rhs) const;
		// DocString: FMTOutput::operator!=
		/**
		FMTOutput non equality operator check if FMTOutput (rhs) have not the same name.
		*/
		bool operator != (const FMTOutput& rhs) const;
		// DocString: FMTOutput::operator+=
		/**
		FMTOutput addition assignment with an other FMTOutput (rhs)
		*/
		FMTOutput& operator +=(const FMTOutput& rhs);
		// DocString: FMTOutput::operator-=
		/**
		FMTOutput substraction assignment with an other FMTOutput (rhs)
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
		Convert the FMTOutput into a readable string like in a output section.
		*/
		operator std::string() const;
		// DocString: FMTOutput::getName
		/**
		Getter for the FMTOutput name.
		*/
		inline std::string getName() const
		{
			return name;
		}
		// DocString: FMTOutput::getDescription
		/**
		Getter for the FMTOutput description.
		*/
		inline std::string getDescription() const
		{
			return description;
		}
		// DocString: FMTOutput::getGroup
		/**
		Getter for the group of the output
		*/
		inline std::string getGroup() const
		{
			return group;
		}
		// DocString: FMTOutput::empty
		/**
		Returns true if the FMTOutput is empty and has no outputsource.
		*/
		bool empty() const;
		// DocString: FMTOutput::size
		/**
		Returns the number of outputsource that the output contains.
		*/
		size_t size() const;
		// DocString: FMTOutput::isLinear
		/**
		Check if the FMTOutput is linear no variable to variable multiplication or division.
		Only Linear output can be added to a linear programming matrix.
		*/
		bool isLinear() const;
		// DocString: FMTOutput::canBeNodesOnly
		/**
		If the output can be used as nodes only so in LP it will return true.
		It has to be linear and dont have -+ of level or timeyield or constant.
		*/
		bool canBeNodesOnly() const;
		// DocString: FMTOutput::isLevel
		/**
		Returns true if one outputsources of the FMTOutput is a level.
		*/
		bool isLevel() const;
		// DocString: FMTOutput::isOnlyLevel
		/**
		Returns true if all outputsources of the FMTOutput are level.
		*/
		bool isOnlyLevel() const;
		// DocString: FMTOutput::isConstantLevel
		/**
		Returns true if all outputsources of the FMTOutput are constant level
		*/
		bool isConstantLevel() const;
		// DocString: FMTOutput::isConstant
		/**
		Returns true if all outputsources of the FMTOutput are constants.
		*/
		bool isConstant() const;
		// DocString: FMTOutput::getConstantValue
		/**
		Get the constant value if the FMTOutput is just one constant.
		*/
		double getConstantValue() const;
		// DocString: FMTOutput::containsLevel
		/**
		Check if the FMTOutput contains any one level.
		*/
		bool containsLevel() const;

		// DocString: FMTOutput::shuntingYard
		/**
		Call shuntingYard using a vector of value for each outputsource and a vector of operators.
		*/
		double shuntingYard(const std::vector<double>& sourcevalues, const std::vector<FMTOperator>& simple_operators) const;
		// DocString: FMTOutput::boundTo
		/**
		Bound a output to specific periods bounds (for FMTConstraint) and return the new generated output.
		Can also modify a given targeted themes target (mask) of the new ouput.
		*/
		FMTOutput boundTo(const std::vector<FMTTheme>& themes, const FMTPerBounds& bound, const std::string& specialbound, std::string attribute = "") const;
		// DocString: FMTOutput::getNodes
		/**
		This function returns a vector of FMTOutputNode generated from the outputnodesource and FMToperators for the FMTouput,
		a multiplier can be added to multiply all the nodesource with a factor.
		*/
		std::vector<FMTOutputNode> getNodes(std::vector<std::string>& equation,
			double multiplier = 1,
			bool orderbyoutputid = false,
			int period = 1) const;
		// DocString: FMTOutput::isSinglePeriod
		/**
		Returns true if the FMTOutput cover only one single period of the FMTGraph, false if
		the FMTOutput covers multiple periods.
		*/
		bool isSinglePeriod() const;
		// DocString: FMTOutput::hasAverage
		/**
		Returns true if the output needs to be averaged.
		*/
		bool hasAverage() const;
		// DocString: FMTOutput::getTargetPeriod
		/**
		If single period is true then it will return the targeted period of the FMTouput.
		*/
		int getTargetPeriod() const;
		// DocString: FMTOutput::hash
		/**
		Hashing fuction for FMTOutput.
		*/
		size_t hash() const;
		// DocString: FMTOutput::setSources
		/**
		* @brief set new sources for this output
		* @param p_sources is the newly sources to set.
		*/
		void setSources(const std::vector<FMTOutputSource>& p_sources);
		// DocString: FMTOutput::setOperators
		/**
		* @brief set new operators to output
		* @param p_operators is the newly oeprators to set.
		*/
		void setOperators(const std::vector<FMTOperator>& p_operators);
		// DocString: FMTOutput::getSources
		/**
		Getter for the vector of FMTOutputSource of the FMTOutput.
		*/
		inline std::vector<FMTOutputSource> getSources() const
		{
			return sources;
		}
		// DocString: FMTOutput::getSourcesReference
		/**
		Getter for the vector of FMTOutputSource of the FMTOutput.
		*/
		inline const std::vector<FMTOutputSource>& getSourcesReference() const
		{
			return sources;
		}
		// DocString: FMTOutput::getOpes
		/**
		Getter for the FMTOperator of the FMTOutput.
		*/
		inline std::vector<FMTOperator> getOpes() const
		{
			return operators;
		}
		// DocString: FMTOutput::targetTheme
		/**
		If theme_target is not equal to -1 then the FMTOutput can target a given theme
		given all the model themes this function returns the FMTOutput targeted theme.
		*/
		FMTTheme targetTheme(const std::vector<FMTTheme>& themes) const;
		// DocString: FMTOutput::targetThemeId
		/**
		Getter for the targeted theme id.
		*/
		inline int targetThemeId() const
		{
			return sources.begin()->getThemeTarget();
		}
		// DocString: FMTOutput::getDecomposition
		/**
		Returns all possible attribute values the FMTOutput can for the specified theme based on the model (themes).
		*/
		std::vector<std::string>getThemeDecomposition(const FMTTheme& theme) const;
		// DocString: FMTOutput::getDecomposition
		/**
		Returns all possible attribute values the FMTOutput can have if the attribute_target != -1
		else returns a empty vector, based on the model (themes).
		*/
		std::vector<std::string>getDecomposition(const std::vector<FMTTheme>& themes) const;
		// DocString: FMTOutput::intersectWithMask
		/**
		Returns the intersected FMTOutput resulting from the mask intersection with this output.
		*/
		FMTOutput intersectWithMask(const Core::FMTMask& mask,
			const std::vector<Core::FMTTheme>& themes) const;
		// DocString: FMTOutput::getVariableIntersect
		/**
		Returns the intersected FMTMask of all variables
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
		Returns the static themes of the whole output.
		*/
		std::vector<const Core::FMTTheme*>getStaticThemes(const std::vector<Core::FMTTheme>& themes, const Core::FMTYields& yields, bool ignoreoutputvariables = false) const;
		// DocString: FMTOutput::presolve
		/**
		Presolve the FMTOutput and remove unused outputsource base on a (baseMask), original themes (originalthemes)
		presolved themes (newthemes) and presolved actions vector (actions)
		and a presolved yields section (yields).
		*/
		FMTOutput presolve(const FMTMaskFilter& filter,
			const std::vector<FMTTheme>& originalthemes,
			const std::vector<const FMTTheme*>& selectedthemes,
			const std::vector<FMTTheme>& newthemes,
			const std::vector<FMTAction>& actions,
			const std::vector<bool>& p_valideActions,
			const FMTYields& yields) const;
		// DocString: FMTOutput::changeoutputsorigin
		/**
		Change outputorigin and targetThemeId of each source with the distance from the begining of the set corresponding to the old outputorigin and themeid.
		Every output origin in the sources must be in the set. Normally used after presolve when some output are removed. Could also be overided with map.
		*/
		void changeSourcesId(const std::set<int>& newoutputsorigin, const std::set<int>& newthemeid);
		// DocString: FMTOutput::changeoutputsorigin
		/**
		Change outputorigin for the optimizationparser
		*/
		void changeSourcesId(const int& outid);
		// DocString: FMTOutput::extractRHSvalue
		/**
		It has to be a mix between noded output and constant level.
		It will also produce a new output and remove the extra constants.
		*/
		Core::FMTOutput removeRHSvalue() const;
		// DocString: FMTOutput::getRHSvalue
		/**
		Get the RHSvalues is mix a level with a variable node for a given period.
		*/
		void getRHSvalue(const int& period, double& lower, double& upper) const;

		// DocString: FMTOutput::setPeriod
		/**
		Set the outputsource to one period.
		*/
		void setPeriod(const int& newperiod);
		// DocString: FMTOutput::isActionbased
		/**
		Returns true if the output constaints action nodes
		*/
		bool isActionbased() const;
		// DocString: FMTOutput::isVariableSizeOf
		/**
		If one source is variable and masksize == source.getmask will return true else return false.
		Will return true if no sources are variable.
		*/
		bool isVariableSizeOf(const size_t& masksize) const;
		// DocString: FMTOutput::isInventory
		/**
		Returns true if contains inventory
		*/
		bool isInventory() const;
		// DocString: FMTOutput::fillFromShuntingYard
		/**
		If the output is non linear you need to use this function to get values.
		*/
		void fillFromShuntingYard(
			const std::vector<std::string>baseeq,
			std::map<std::string, double>& results,
			const std::vector<Core::FMTOutputNode>& nodes,
			std::map<std::string, std::vector<std::string>>& allequations) const;

	protected:
		// DocString: FMTOutput::sources
		///outputsources data used to generate outputnodes
		std::vector<FMTOutputSource>sources;
		// DocString: FMTOutput::operators
		///vector of simple operators like +-*/
		std::vector<FMTOperator>operators;
		// DocString: FMTOutput::theme_target
		///Sometime output can specify multiple attributes of a given themes
		//int theme_target;
		// DocString: FMTOutput::name
		///This is the name of the output
		std::string name;
		// DocString: FMTOutput::description
		///This is description of the FMTOutput has seen in the output section.
		std::string description;
		// DocString: FMTOutput::group
		///This is the group of the output, if empty there's no group
		std::string group;
		// DocString: FMTOutput::setProportions
		/**
		Set equations proportions for developpements....when original entry is numeric turn it into proportion.
		*/
		void setProportions(std::map<std::string, std::vector<std::string>>& allequations,
			const std::vector<std::string>& baseequation) const;
		// DocString: FMTOutput::isDivision
		/**
		Return true if output contains division
		*/
		bool isDivision() const;
		// DocString: FMTOutput::isDivision
		/**
		Replace the division with the bound and reverse the denominator
		*/
		void replaceDivision(const double& bound);
		// DocString: FMTOutput::isValidAction
		/**
		@Valid if an action or aggregate is in p_Actions.
		@param[in] p_actionOrAggregate name or aggregate of action
		@param[in] p_actions the full actions vector
		@param[in] p_validActions validity of actions.
		@return true if it's in
		*/
		static bool isValidAction(const std::string& p_actionOrAggregate,
			const std::vector<FMTAction>& p_actions,
			const std::vector<bool>& p_validActions);
	private:
		// DocString: FMTOutput::serialize
		/**
		serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			try {
				ar& boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<FMTObject>(*this));
				ar& BOOST_SERIALIZATION_NVP(sources);
				ar& BOOST_SERIALIZATION_NVP(operators);
				//ar & BOOST_SERIALIZATION_NVP(theme_target);
				ar& BOOST_SERIALIZATION_NVP(name);
				ar& BOOST_SERIALIZATION_NVP(description);
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
		 * @brief Verifies that no line in the given source string exceeds a specified maximum length.
		 *
		 * This function checks each line in the provided source string to ensure that it does not
		 * exceed the maximum allowed number of characters per line. If any line exceeds this limit,
		 * an exception is raised and the function returns false.
		 *
		 * @param p_source The source string to be checked.
		 * @return `true` if all lines in the source string are within the allowed length, `false` otherwise.
		 */
		bool _sourceCounter(const std::string& p_source) const;
		// DocString: FMTOutput::_verifyOperatorOrder
		/**
		@brief verify that a FMT output doesn't have more than one operator (* or /).
		@return True or false.
		*/
		bool _verifyOperatorOrder() const;		
		// DocString: FMTOutput::_verifyOperatorOrder
		/**
		@brief Split a FMT output with temporary output for operator management.
		@return a string with more than 1 output.
		*/
		std::string _operatorFormat() const;
    };
// DocString: FMTOutputComparator
/**
 FMTOutputComparator to check if the output_name already exist in a std container.
*/
class FMTEXPORT FMTOutputComparator
{
	// DocString: FMTOutputComparator::output_name
	///The name of the FMTOutput we are looking for.
	std::string output_name;
public:
	// DocString: FMTOutputComparator()
	/**
	 FMTOutputComparator constructor that takes the name the output we want to find in a std container.
	*/
	FMTOutputComparator(std::string name);
	// DocString: FMTOutputComparator::operator()(const FMTOutput&)
	/**
	Matching test operator for FMTOutputComparator.
	*/
	bool operator()(const FMTOutput& output) const;

};

}
BOOST_CLASS_EXPORT_KEY(Core::FMTOutput)
#endif // FMToutput_Hm_included
