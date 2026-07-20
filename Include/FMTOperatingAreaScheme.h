/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI

#ifndef FMToperatingareascheme_Hm_included
#define FMToperatingareascheme_Hm_included

#include "FMTOperatingArea.h"
#include <vector>
#include <map>
#include <memory>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include "FMTGraph.hpp"
#include "FMTObject.h"

namespace Core
{
	class FMTMask;
}

namespace Models
{
	class FMTLpSolver;
}


namespace Heuristics
	{
	// DocString: FMTOperatingAreaScheme
	/**
	@brief Operating area scheme giving information about its opening time, green up and return time.
	@details The operating area scheme has to be based on static themes not used within transitions. Multiple potential schedules are generated for the FMTOperatingArea formulated as a heuristic and/or MIP.
	*/
	class FMTEXPORT FMTOperatingAreaScheme: public FMTOperatingArea, public Core::FMTObject
		{
		// DocString: FMTOperatingAreaScheme::serialize
		/**
		@brief Serialize the FMTOperatingAreaScheme for multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to serialize to or from.
		@param[in] version the serialization version.
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
			{
			ar & boost::serialization::make_nvp("operatingarea", boost::serialization::base_object<FMTOperatingArea>(*this));
			ar & BOOST_SERIALIZATION_NVP(openingconstraints);
			ar & BOOST_SERIALIZATION_NVP(openingbinaries);
			ar & BOOST_SERIALIZATION_NVP(maximalschemesconstraint);
			ar & BOOST_SERIALIZATION_NVP(schemesperiods);
			ar & BOOST_SERIALIZATION_NVP(openingtime);
			ar & BOOST_SERIALIZATION_NVP(returntime);
			ar & BOOST_SERIALIZATION_NVP(repetition);
			ar & BOOST_SERIALIZATION_NVP(greenup);
			}
		// DocString: FMTOperatingAreaScheme::openingconstraints
		///Opening constraints indexes first vector is the potential scheme second one is the indexes related to this scheme.
		std::vector<std::vector<int>>openingconstraints;
		// DocString: FMTOperatingAreaScheme::openingbinaries
		///Each potential scheme of the operating area has a binary index.
		std::vector<int>openingbinaries;
		// DocString: FMTOperatingAreaScheme::maximalschemesconstraint
		///potentialscheme1 + potentialscheme2 <= 1 constraint index to make sure only one scheme in choose
		int maximalschemesconstraint;
		// DocString: FMTOperatingAreaScheme::schemesperiods
		///The first vector is the scheme index and the second vector is the period id related to each opening constraint index
		std::vector<std::vector<int>>schemesperiods;
		// DocString: FMTOperatingAreaScheme::openingtime
		///opening time is the time that a operating area will be available for harvest
		size_t openingtime;
		// DocString: FMTOperatingAreaScheme::returntime
		///return time is the time the opening area wont be available for harvest after being harvested.
		size_t returntime;
		// DocString: FMTOperatingAreaScheme::repetition
		///repetition is the repetition of the open + close pattern only use when generating the solution.
		size_t repetition;
		// DocString: FMTOperatingAreaScheme::greenup
		///The greenup is the number of periods before a neighbor of a newly harvested block can be harvested.
		size_t greenup;
		// DocString: FMTOperatingAreaScheme::startingperiod
		///The starting period is the period from where the heuristic needs to fin a schedule (in case of model update).
		size_t startingperiod;
		// DocString: FMTOperatingAreaScheme::threshold
		///Area threshold the proportion of area harvested need to be at least this number
		double threshold; 
		// DocString: FMTOperatingAreaScheme::maxreturntime
		///is the maximum time the opening area wont be available for harvest after being harvested. If it's different than returntime, it will enumerate all possibilities 
		//between return time and maxreturntime for each closing
		size_t maxreturntime;
		// DocString: FMTOperatingAreaScheme::rejectednodescid
		///In schemestolp, we create a constraint for rejectednodes, this is the id.
		int rejectednodescid;
		// DocString: FMTOperatingAreaScheme::getArea
		/**
		@brief Get the area of the operating area based on a primal solution for the initial vertices of the FMTLpModel graph.
		@param[in] primalsolution the primal solution.
		@param[in] maingraph the main graph.
		@param[in] vertices the initial vertices.
		@return the primal area.
		*/
		double getPrimalArea(const double* primalsolution, const Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>& maingraph, const std::vector<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor>& vertices) const;
		// DocString: FMTOperatingAreaScheme::getBestSchemeId
		/**
		@brief Get the index of the best possible scheme looking at the primal solution.
		@param[in] primalsolution the primal solution.
		@return the best scheme index.
		*/
		size_t getBestSchemeId(const double* primalsolution) const;
		// DocString: FMTOperatingAreaScheme::generateSchemes
		/**
		@brief Generate all the potential schemes of the operating area from the vertices.
		@details For a return time of 4 and an opening time of 2 two schemes are 00011000011000 and 11000011000011.
		@param[in] vertices the vertices.
		@return the potential schemes.
		*/
		std::vector<std::vector<std::vector<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor>>> generateSchemes(const std::vector<std::vector<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor>>& vertices);
		// DocString: FMTOperatingAreaScheme::schemestoLP
		/**
		@brief Fill the opening constraints and binaries and add the newly formulated constraints and binaries to the solver interface, keeping the model linear.
		@param[in] schemes the generated schemes.
		@param[in] periodictargetednodes the periodic targeted nodes.
		@param[in] totalareavertices the initial vertices.
		@param[in,out] solver the solver.
		@param[in] primalsolution the primal solution.
		@param[in] maingraph the main graph.
		@param[in] actionIDS the action ids.
		*/
		void schemestoLP(const std::vector<std::vector<std::vector<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor>>>& schemes,
			const std::vector<std::vector<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor>>& periodictargetednodes,
			const std::vector<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor>& totalareavertices,
			Models::FMTLpSolver& solver,
			const double* primalsolution,
			const Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>& maingraph, const std::vector<int>& actionIDS);

		// DocString: FMTOperatingAreaScheme::getIgnoredVertices
		/**
		@brief Return the vertices that are not in the schemes and must be set to 0 in the matrix.
		@param[in] schemes the schemes.
		@param[in] targetedperiodsvertices the targeted periods vertices.
		@return the ignored vertices.
		*/
		std::vector<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor> getIgnoredVertices(const std::vector<std::vector<std::vector<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor>>>& schemes,
			const std::vector<std::vector<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor>>& targetedperiodsvertices) const;

		// DocString: FMTOperatingAreaScheme::schemesToPeriods
		/**
		@brief Return the periods corresponding to each scheme from the graph, used when generating the yield solution.
		@param[in] schemes the schemes.
		@param[in] maingraph the main graph.
		@return the periods of each scheme.
		*/
		std::vector<std::vector<int>> schemesToPeriods(const std::vector<std::vector<std::vector<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor>>>& schemes, const Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>& maingraph) const;
		// DocString: FMTOperatingAreaScheme::getRowsActivitySum
		/**
		@brief Summarize the total amount of activities of a set of rows from the dual solution.
		@param[in] rows the rows.
		@param[in] dualsolution the dual solution.
		@return the sum of the row activities.
		*/
		double getRowsActivitySum(const std::vector<int>& rows, const double* dualsolution) const;
		// DocString: FMTOperatingAreaScheme::isThresholdActivityRows
		/**
		@brief Return true if the row activities are above the threshold.
		@param[in] rows the rows.
		@param[in] dualsolution the dual solution.
		@param[in] tempThreshold the temporary threshold.
		@return true if the activities are above the threshold else false.
		*/
		bool isThresholdActivityRows(const std::vector<int>& rows, const double* dualsolution, double tempThreshold = -1) const;
		// DocString: FMTOperatingAreaScheme::fillPattern
		/**
		@brief Build a complete pattern from a base pattern using the starting period and repetition members.
		@details From the pattern 110000 it builds a complete pattern like 0110000110000110000.
		@param[in] pattern the base pattern.
		@param[in] startat the starting period.
		@return the complete pattern.
		*/
		std::vector<double>fillPattern(const std::vector<double>& pattern, const int& startat) const;
		// DocString: FMTOperatingAreaScheme::closeNoActivity
		/**
		@brief Close an opened operating area with no cost to the LP problem if no activity is detected.
		@param[in,out] filleduppattern the filled pattern.
		@param[in] selected the selected scheme.
		@param[in] dualsolution the dual solution.
		*/
		void closeNoActivity(std::vector<double>& filleduppattern, const size_t& selected, const double* dualsolution) const;
		// DocString: FMTOperatingAreaScheme::getMaxPeriod
		/**
		@brief Get the maximal period of all the potential schemes.
		@return the maximal period.
		*/
		int getMaxPeriod() const;
		public:
			// DocString: FMTOperatingAreaScheme::setReturnTime
			/**
			@brief Set the minimal and maximal return time.
			@param[in] minimalreturntime the minimal return time.
			@param[in] maximalreturntime the maximal return time.
			*/
			void setReturnTime(const size_t& minimalreturntime,const size_t& maximalreturntime);
			// DocString: FMTOperatingAreaScheme::getThreshold
			/**
			@brief Get the threshold.
			@return the threshold.
			*/
			double getThreshold() const;
			// DocString: FMTOperatingAreaScheme::getOpeningTime
			/**
			@brief Get the opening time.
			@return the opening time.
			*/
			size_t getOpeningTime() const;
			// DocString: FMTOperatingAreaScheme::getMinimalReturnTime
			/**
			@brief Get the minimal return time.
			@return the minimal return time.
			*/
			size_t getMinimalReturnTime() const;
			// DocString: FMTOperatingAreaScheme::getRepetition
			/**
			@brief Get the repetition of the pattern.
			@return the repetition.
			*/
			size_t getRepetition() const;
			// DocString: FMTOperatingAreaScheme::getMaximalReturnTime
			/**
			@brief Get the maximal return time.
			@return the maximal return time.
			*/
			size_t getMaximalReturnTime() const;
			// DocString: FMTOperatingAreaScheme::empty
			/**
			@brief Check if the operating area is empty, which is possible if there is no potential development to operate.
			@return true if the operating area is empty else false.
			*/
			bool empty() const;
			// DocString: FMTOperatingAreaScheme::getOpeningBinaries
			/**
			@brief Return the variable indexes of the binaries of all the potential schemes of the operating area.
			@return the opening binaries.
			*/
			const std::vector<int>& getOpeningBinaries() const;
			// DocString: FMTOperatingAreaScheme::getStartingPeriod
			/**
			@brief Return the starting period at which all the schemes of the operating area start.
			@return the starting period.
			*/
			size_t getStartingPeriod() const;
			// DocString: FMTOperatingAreaScheme::getBinariesSum
			/**
			@brief Summarize the value of all the binary variables for all the potential schemes from the primal solution.
			@param[in] primalsolution the primal solution.
			@return the sum of the binaries.
			*/
			double getBinariesSum(const double* primalsolution) const;
			// DocString: FMTOperatingAreaScheme::getActivitySum
			/**
			@brief Summarize the value of all the constraints for all the potential schemes from the dual solution.
			@param[in] dualsolution the dual solution.
			@return the sum of the activities.
			*/
			double getActivitySum(const double* dualsolution) const;
			// DocString: FMTOperatingAreaScheme::getthresholdactivity
			/**
			@brief Return true if every constraint activity is above the threshold.
			@param[in] dualsolution the dual solution.
			@return true if every activity is above the threshold else false.
			*/
			bool isThresholdActivity(const double* dualsolution) const;
			// DocString: FMTOperatingAreaScheme::getCommonBinaries
			/**
			@brief Return a map with the binary indexes of this operating area as keys and the binary indexes of the neighbor as elements, using the green up members.
			@details Used to validate which schemes of an operating area constrain the usage of the schemes of a neighbor.
			@param[in] neighbor the neighbor operating area.
			@return the common binaries.
			*/
			std::map<int, std::vector<int>> getCommonBinaries(const FMTOperatingAreaScheme& neighbor) const;
			// DocString: FMTOperatingAreaScheme::getPrimalSolutionIndex
			/**
			@brief Return the index of the scheme the operating area is bounded to, from the primal solution.
			@param[in] primalsolution the primal solution.
			@return the bounded scheme index.
			*/
			size_t getPrimalSolutionIndex(const double* primalsolution) const;
			// DocString: FMTOperatingAreaScheme::getDualSolutionIndex
			/**
			@brief Return the index of the scheme used by the operating area from the rows upper bounds.
			@param[in] upperbound the rows upper bounds.
			@param[out] locid the scheme index.
			@return true if a scheme index is found else false.
			*/
			bool getDualSolutionIndex(const double* upperbound, size_t& locid) const;
			// DocString: FMTOperatingAreaScheme::havePotentialSolution
			/**
			@brief Check if a scheme binary has a value greater than 0 in the primal solution, indicating a potential scheme to choose.
			@param[in] primalsolution the primal solution.
			@return true if there is a potential primal scheme else false.
			*/
			bool havePotentialSolution(const double* primalsolution) const;
			// DocString: FMTOperatingAreaScheme::haveActivitySolution
			/**
			@brief Check if a set of scheme constraints has an activity value greater than 0 in the dual solution, indicating a potential scheme to choose.
			@param[in] dualsolution the dual solution.
			@return true if there is a potential dual scheme else false.
			*/
			bool haveActivitySolution(const double* dualsolution) const;
			// DocString: FMTOperatingAreaScheme::isAllPrimalBounded
			/**
			@brief Return false if any binary lower or upper bound is not set to 1.
			@param[in] lowerbounds the lower bounds.
			@param[in] upperbounds the upper bounds.
			@return true if all the binaries are bounded to 1 else false.
			*/
			bool isAllPrimalBounded(const double* lowerbounds, const double* upperbounds) const;
			// DocString: FMTOperatingAreaScheme::isAllDualBounded
			/**
			@brief Return false if any constraint upper bound is different from 0.
			@param[in] upperbounds the upper bounds.
			@return true if all the constraints are bounded to 0 else false.
			*/
			bool isAllDualBounded(const double* upperbounds) const;
			// DocString: FMTOperatingAreaScheme::isPrimalBounded
			/**
			@brief Return true if any binary lower or upper bound is set to 1.
			@param[in] lowerbounds the lower bounds.
			@param[in] upperbounds the upper bounds.
			@return true if a binary is bounded to 1 else false.
			*/
			bool isPrimalBounded(const double* lowerbounds, const double* upperbounds) const;
			// DocString: FMTOperatingAreaScheme::isDualBounded
			/**
			@brief Return true if any constraint upper bound is set to 0.
			@param[in] upperbounds the upper bounds.
			@return true if a constraint is bounded to 0 else false.
			*/
			bool isDualBounded(const double* upperbounds) const;
			// DocString: FMTOperatingAreaScheme::getPotentialPrimalSchemes
			/**
			@brief Return the potential scheme indexes from the primal solution, its bounds and the neighboring operating areas, ordered from the most to the least area used but greater than 0.
			@param[in] primalsolution the primal solution.
			@param[in] lowerbounds the lower bounds.
			@param[in] upperbounds the upper bounds.
			@param[in] neighbors the neighboring operating areas.
			@return the potential primal scheme indexes.
			*/
			std::vector<size_t>getPotentialPrimalSchemes(const double* primalsolution, const double* lowerbounds, const double* upperbounds,const std::vector<FMTOperatingAreaScheme>& neighbors) const;
			// DocString: FMTOperatingAreaScheme::getPotentialDualSchemes
			/**
			@brief Return the potential scheme indexes from the dual solution, the rows upper bounds and the neighboring operating areas, ordered from the most to the least area used but greater than 0.
			@param[in] dualsolution the dual solution.
			@param[in] upperbound the rows upper bounds.
			@param[in] neighbors the neighboring operating areas.
			@return the potential dual scheme indexes.
			*/
			std::vector<size_t>getPotentialDualSchemes(const double* dualsolution, const double* upperbound, const std::vector<FMTOperatingAreaScheme>& neighbors) const;
			// DocString: FMTOperatingAreaScheme::getRessourcesToDelete
			/**
			@brief Push all the variables and constraints of the operating area into the columns and rows to delete vectors.
			@param[in,out] colstodelete the columns to delete.
			@param[in,out] rowstodelete the rows to delete.
			*/
			void getRessourcesToDelete(std::vector<int>& colstodelete, std::vector<int>& rowstodelete) const; //Remove all variable and constraints related to the operating area from the solver
			// DocString: FMTOperatingAreaScheme::pushBinaries
			/**
			@brief Push all the binaries into a targets vector.
			@param[in,out] targets the targets.
			*/
			void pushBinaries(std::vector<int>& targets) const;
			// DocString: FMTOperatingAreaScheme::unboundAllPrimalSchemes
			/**
			@brief Push the binary indexes into the targets vector and push 0 and 1 into the bounds.
			@param[in,out] targets the targets.
			@param[in,out] bounds the bounds.
			@return the number of binaries pushed.
			*/
			size_t unboundAllPrimalSchemes(std::vector<int>& targets, std::vector<double>& bounds) const;
			// DocString: FMTOperatingAreaScheme::unboundAllDualSchemes
			/**
			@brief Push the constraint indexes into the targets vector, push -inf and the area into the bounds, and return the id of the unbounded scheme.
			@param[in,out] targets the targets.
			@param[in,out] bounds the bounds.
			@return the unbounded scheme id.
			*/
			size_t unboundAllDualSchemes(std::vector<int>& targets, std::vector<double>& bounds) const;
			// DocString: FMTOperatingAreaScheme::boundAllPrimalSchemes
			/**
			@brief Push the variable indexes into the targets vector and push the bound value into the bounds.
			@param[in,out] targets the targets.
			@param[in,out] bounds the bounds.
			@param[in] boundvalue the bound value.
			@return the number of variables pushed.
			*/
			size_t boundAllPrimalSchemes(std::vector<int>& targets, std::vector<double>& bounds,double boundvalue = 1.0) const;
			// DocString: FMTOperatingAreaScheme::boundAllDualSchemes
			/**
			@brief Push the constraint indexes into the targets vector and push the bound value into the bounds.
			@param[in,out] targets the targets.
			@param[in,out] bounds the bounds.
			@return the number of constraints pushed.
			*/
			size_t boundAllDualSchemes(std::vector<int>& targets, std::vector<double>& bounds) const;
			// DocString: FMTOperatingAreaScheme::boundPrimalScheme
			/**
			@brief Push the variable index of a scheme binary into the targets vector and push 1 and 1 into the bounds.
			@param[in,out] targets the targets.
			@param[in,out] bounds the bounds.
			@param[in] schemeid the scheme id.
			@return true if the scheme is bounded else false.
			*/
			bool boundPrimalScheme(std::vector<int>& targets, std::vector<double>& bounds, const size_t& schemeid) const; 
			// DocString: FMTOperatingAreaScheme::unboundDualScheme
			/**
			@brief Push the constraints of a scheme into the targets vector with -inf and the area bounds, and push the constraints of the other schemes with 0 and 0 bounds.
			@param[in] rowactivities the row activities.
			@param[in,out] targets the targets.
			@param[in,out] bounds the bounds.
			@param[in] schemeid the scheme id.
			@param[in] looseset if true uses a loose set.
			@return true if the scheme is unbounded else false.
			*/
			bool unboundDualScheme(const double* rowactivities, std::vector<int>& targets, std::vector<double>& bounds, const size_t& schemeid, bool looseset=true) const;
			// DocString: FMTOperatingAreaScheme::getPrimalSolution
			/**
			@brief Return the yield solution of the primal problem by summing up all the binary variables into a single vector.
			@param[in] primalsolution the primal solution.
			@return the primal yield solution.
			*/
			std::vector<double> getPrimalSolution(const double* primalsolution) const;
			// DocString: FMTOperatingAreaScheme::getDualSolution
			/**
			@brief Return the yield solution of the dual problem by setting the selected scheme solution into a vector.
			@param[in] upperbounds the upper bounds.
			@param[in] dualsolution the dual solution.
			@param[out] canbreakneighboring true if the neighboring can be broken.
			@return the dual yield solution.
			*/
			std::vector<double> getDualSolution(const double* upperbounds, const double* dualsolution, bool& canbreakneighboring) const;
			// DocString: FMTOperatingAreaScheme::getDualLowerBounds
			/**
			@brief Return the lower bounds of the solution for bounding the minimal harvested area.
			@param[in] lowerbounds the lower bounds.
			@param[in] upperbounds the upper bounds.
			@return the dual lower bounds.
			*/
			std::vector<double> getDualLowerBounds(const double* lowerbounds,const double* upperbounds) const;
			// DocString: FMTOperatingAreaScheme::setConstraints
			/**
			@brief Set up the constraints and variables using a matrix build and a primal solution, filling all the matrix element members.
			@details The user has to synchronize the matrix build to the matrix after iterating on all the operating areas.
			@param[in] vertices the vertices.
			@param[in] totalareavertices the initial vertices.
			@param[in] graph the graph.
			@param[in,out] solver the solver.
			@param[in] primalsolution the primal solution.
			@param[in] actionIDS the action ids.
			*/
			void setConstraints(const std::vector<std::vector<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor>>& vertices,
				const std::vector<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor>& totalareavertices,
				const Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>& graph,Models::FMTLpSolver& solver,
				const double* primalsolution,
				const std::vector<int>& actionIDS);
			// DocString: FMTOperatingAreaScheme(const FMTOperatingArea,const size_t, const size_t,const size_t,const size_t,const size_t)
			/**
			@brief Main constructor for FMTOperatingAreaScheme.
			@details Before synchronizing to the solver interface, the user has to provide the green up, return time and other parameters for each operating area.
			@param[in] oparea the operating area.
			@param[in] lopeningtime the opening time.
			@param[in] lreturntime the return time.
			@param[in] lmaxreturntime the maximal return time.
			@param[in] lrepetition the repetition.
			@param[in] lgreenup the green up.
			@param[in] lstartingperiod the starting period.
			@param[in] minimalarearatio the minimal area ratio needed to open the operating area.
			*/
			FMTOperatingAreaScheme(const FMTOperatingArea& oparea,const size_t& lopeningtime, const size_t& lreturntime, const size_t& lmaxreturntime,
				const size_t& lrepetition, const size_t& lgreenup,const size_t& lstartingperiod,double minimalarearatio=0.0);
			// DocString: FMTOperatingAreaScheme()
			/**
			@brief Default constructor for FMTOperatingAreaScheme.
			*/
			FMTOperatingAreaScheme()=default;
			// DocString: FMTOperatingAreaScheme(const FMTOperatingArea)
			/**
			@brief Copy constructor for FMTOperatingAreaScheme.
			@param[in] rhs the FMTOperatingAreaScheme to copy.
			*/
			FMTOperatingAreaScheme(const FMTOperatingAreaScheme& rhs) = default;
			// DocString: FMTOperatingAreaScheme::operator=
			/**
			@brief Copy assignment operator for FMTOperatingAreaScheme.
			@param[in] rhs the FMTOperatingAreaScheme to copy.
			@return a reference to this FMTOperatingAreaScheme.
			*/
			FMTOperatingAreaScheme& operator = (const FMTOperatingAreaScheme& rhs)=default;
			// DocString: FMTOperatingAreaScheme::fillBoundsNVariables
			/**
			@brief For each scheme constraint, take the constraint bounds of the actual model.
			@param[in] lowerb the lower bounds.
			@param[in] upperb the upper bounds.
			@param[in,out] constraintstargets the constraints targets.
			@param[in,out] bounds the bounds.
			*/
			void fillBoundsNVariables(const double* lowerb, const double* upperb, std::vector<int>& constraintstargets,std::vector<double>&bounds) const;
			// DocString: FMTOperatingAreaScheme::operator==
			/**
			@brief Comparison operator for FMTOperatingAreaScheme.
			@param[in] rhs the FMTOperatingAreaScheme to compare to.
			@return true if both operating area schemes are equal else false.
			*/
			bool operator == (const FMTOperatingAreaScheme& rhs) const;
			// DocString: FMTOperatingAreaScheme::operator!=
			/**
			@brief Comparison operator for FMTOperatingAreaScheme.
			@param[in] rhs the FMTOperatingAreaScheme to compare to.
			@return true if both operating area schemes are different else false.
			*/
			bool operator != (const FMTOperatingAreaScheme& rhs) const;
			// DocString: ~FMTOperatingAreaScheme()
			/**
			@brief Default destructor for FMTOperatingAreaScheme.
			*/
			~FMTOperatingAreaScheme()=default;
			// DocString: FMTOperatingAreaScheme::presolve
			/**
			@brief Return a presolved FMTOperatingAreaScheme using a mask and a subset of the original themes.
			@param[in] selectedmask the selected mask.
			@param[in] presolvedthemes the presolved themes.
			@return the presolved operating area scheme.
			*/
			FMTOperatingAreaScheme presolve(const Core::FMTMask& selectedmask,const std::vector<Core::FMTTheme>&presolvedthemes) const;
			// DocString: FMTOperatingAreaScheme::getNumberofscheme
			/**
			@brief Return the number of schemes of the operating area, only usable after schemestoLP has been called.
			@return the number of schemes.
			*/
			size_t getNumberofscheme() const;
			// DocString: FMTOperatingAreaScheme::getNumberofsimplescheme
			/**
			@brief Return the number of simple schemes with a fixed return time and opening time.
			@return the number of simple schemes.
			*/
			size_t getNumberofsimplescheme() const;
			// DocString: FMTOperatingAreaScheme::getRejectedNodesCid
			/**
			@brief Return the constraint id of the rejected nodes.
			@return the rejected nodes constraint id.
			*/
			const int& getRejectedNodesCid() const;
			// DocString: FMTOperatingAreaScheme::getschemesperiods
			/**
			@brief Return the opening constraints of the operating area.
			@return the opening constraints.
			*/
			const std::vector<std::vector<int>>& getOpeningConstraints() const;
			// DocString: FMTOperatingAreaScheme::getMaximalSchemesConstraint
			/**
			@brief Return the constraint id ensuring that only one scheme is chosen.
			@return the maximal schemes constraint id.
			*/
			const int& getMaximalSchemesConstraint() const;

			// DocString: FMTOperatingAreaScheme::getGreenUp
			/**
			@brief Return the green up.
			@return the green up.
			*/
			const size_t& getGreenUp() const { return greenup; };
		
		private:
			// DocString: FMTOperatingAreaScheme::_maxNearThresholdActivityRows
			/**
			@brief Return the maximum threshold possible for a set of rows from the dual solution.
			@param[in] rows the rows.
			@param[in] dualsolution the dual solution.
			@return the maximum near threshold.
			*/
			double _maxNearThresholdActivityRows(const std::vector<int>& rows, const double* dualsolution) const;
			// DocString: FMTOperatingAreaScheme::_findIndexLocation
			/**
			@brief Return the location of a binary index in the opening binaries.
			@param[in] binary the binary index.
			@return the index location.
			*/
			size_t _findIndexLocation(int binary) const;
			// DocString: FMTOperatingAreaScheme::_addPotentialResults
			/**
			@brief Add the potential scheme indexes and values at an index location from the dual solution.
			@param[in] indexlocation the index location.
			@param[in,out] potentialindexes the potential indexes.
			@param[in,out] potentialValues the potential values.
			@param[in] dualsolution the dual solution.
			*/
			void _addPotentialResults(
				size_t indexlocation, 
				std::vector<size_t>& potentialindexes, 
				std::vector<double>& potentialValues,
				const double* dualsolution) const;
			// DocString: FMTOperatingAreaScheme::_checkDoubleIncludes
			/**
			@brief Return true if a scheme opening constraints double includes another one.
			@param[in] openingconstraints the opening constraints.
			@return true if there is a double include else false.
			*/
			static bool _checkDoubleIncludes(const std::vector<std::vector<int>>& openingconstraints);
		};

	// DocString: FMTOperatingAreaSchemeComparator
	/**
	@brief Comparator to find a FMTOperatingAreaScheme with std::find_if when operating area schemes are in an stl container.
	*/
	class FMTOperatingAreaSchemeComparator
		{
			// DocString: FMTOperatingAreaSchemeComparator::mask
			///FMTMask of the operating area we wish to find.
			Core::FMTMask mask;
		public:
			// DocString: FMTOperatingAreaSchemeComparator(const Core::FMTMask)
			/**
			@brief Construct the comparator from a mask reference.
			@param[in] lmask the mask.
			*/
			FMTOperatingAreaSchemeComparator(const Core::FMTMask& lmask);
			// DocString: FMTOperatingAreaSchemeComparator::operator()
			/**
			@brief Comparator for FMTOperatingAreaScheme.
			@param[in] oparea the operating area scheme to compare to.
			@return true if the operating area scheme matches else false.
			*/
			bool operator()(const FMTOperatingAreaScheme& oparea) const;

		};

	}
	BOOST_CLASS_EXPORT_KEY(Heuristics::FMTOperatingAreaScheme)
#endif
#endif
