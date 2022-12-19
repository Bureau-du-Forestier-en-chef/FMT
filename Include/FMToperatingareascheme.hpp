/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI

#ifndef FMToperatingareascheme_H_INCLUDED
#define FMToperatingareascheme_H_INCLUDED

#include "FMToperatingarea.hpp"
#include <vector>
#include <map>
#include <memory>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include "FMTgraph.hpp"


namespace Core
{
	class FMTmask;
}

namespace Models
{
	class FMTlpsolver;
}


namespace Heuristics
	{
	// DocString: FMToperatingareascheme
	/**
	FMToperatingareascheme gives informations about a given operating area scheme it's
	openingtime,greenup,returntime.
	FMToperatingscheme area have to be based on static themes (not used within transitions).
	Multiple potential schedules will be generated for the FMToperatingarea formulated as a heuristic and/or MIP.
	*/
	class FMTEXPORT FMToperatingareascheme: public FMToperatingarea
		{
		// DocString: FMToperatingareascheme::serialize
		/**
		serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
			{
			ar & boost::serialization::make_nvp("operatingarea", boost::serialization::base_object<FMToperatingarea>(*this));
			ar & BOOST_SERIALIZATION_NVP(openingconstraints);
			ar & BOOST_SERIALIZATION_NVP(openingbinaries);
			ar & BOOST_SERIALIZATION_NVP(maximalschemesconstraint);
			ar & BOOST_SERIALIZATION_NVP(schemesperiods);
			ar & BOOST_SERIALIZATION_NVP(openingtime);
			ar & BOOST_SERIALIZATION_NVP(returntime);
			ar & BOOST_SERIALIZATION_NVP(repetition);
			ar & BOOST_SERIALIZATION_NVP(greenup);
			}
		// DocString: FMToperatingareascheme::openingconstraints
		///Opening constraints indexes first vector is the potential scheme second one is the indexes related to this scheme.
		std::vector<std::vector<int>>openingconstraints;
		// DocString: FMToperatingareascheme::openingbinaries
		///Each potential scheme of the operating area has a binary index.
		std::vector<int>openingbinaries;
		// DocString: FMToperatingareascheme::maximalschemesconstraint
		///potentialscheme1 + potentialscheme2 <= 1 constraint index to make sure only one scheme in choose
		int maximalschemesconstraint;
		// DocString: FMToperatingareascheme::schemesperiods
		///The first vector is the scheme index and the second vector is the period id related to each opening constraint index
		std::vector<std::vector<int>>schemesperiods;
		// DocString: FMToperatingareascheme::openingtime
		///opening time is the time that a operating area will be available for harvest
		size_t openingtime;
		// DocString: FMToperatingareascheme::returntime
		///return time is the time the opening area wont be available for harvest after being harvested.
		size_t returntime;
		// DocString: FMToperatingareascheme::repetition
		///repetition is the repetition of the open + close pattern only use when generating the solution.
		size_t repetition;
		// DocString: FMToperatingareascheme::greenup
		///The greenup is the number of periods before a neighbor of a newly harvested block can be harvested.
		size_t greenup;
		// DocString: FMToperatingareascheme::startingperiod
		///The starting period is the period from where the heuristic needs to fin a schedule (in case of model update).
		size_t startingperiod;
		// DocString: FMToperatingareascheme::threshold
		///Area threshold the proportion of area harvested need to be at least this number
		double threshold; 
		// DocString: FMToperatingareascheme::maxreturntime
		///is the maximum time the opening area wont be available for harvest after being harvested. If it's different than returntime, it will enumerate all possibilities 
		//between return time and maxreturntime for each closing
		size_t maxreturntime;
		// DocString: FMToperatingareascheme::rejectednodescid
		///In schemestolp, we create a constraint for rejectednodes, this is the id.
		int rejectednodescid;
		// DocString: FMToperatingareascheme::getarea
		/**
			Get the area of the operating area base on a (primalsolution) 
			for the initial (verticies) of the FMTlpmodel graph.
		*/
		double getprimalarea(const double* primalsolution, const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>& maingraph, const std::vector<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor>& verticies) const;
		// DocString: FMToperatingareascheme::getbestschemeid
		/**
			Get the best possible scheme looking at the primal solution,
			return the index of that scheme.
		*/
		size_t getbestschemeid(const double* primalsolution) const;
		// DocString: FMToperatingareascheme::generateschemes
		/**
			Generate potential schemes for the operating area
			here is 2 different schemes for a returntime = 4 and a openingtime = 2:
			00011000011000
			11000011000011
			This fonction generate all potential schemes of the operating area if the "harvest stade" exist within the matrix.
			If full enumeration is set to true then it will enumerate all possible patterns....
		*/
		std::vector<std::vector<std::vector<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor>>> generateschemes(const std::vector<std::vector<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor>>& verticies);
		// DocString: FMToperatingareascheme::schemestoLP
		/**
		Using the generated (schemes) from the generateschemes functions, and all periodic verticies (periodics),
		initial verticies (totalareaverticies) and the (primalsolution) for calculating the _area, solverinterface, graph and actionids.
		The function fills the opening constraints and opening binairies member data and also add the newly formulated
		constraints and binairies to the solverinterface. But the model is kept linear.
		*/
		void schemestoLP(const std::vector<std::vector<std::vector<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor>>>& schemes,
			const std::vector<std::vector<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor>>& periodictargetednodes,
			const std::vector<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor>& totalareaverticies,
			Models::FMTlpsolver& solver,
			const double* primalsolution,
			const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>& maingraph, const std::vector<int>& actionIDS);

		// DocString: FMToperatingareascheme::getignoredverticies
		/**
		Get a vector with the verticies that are not in the schemes, they must be set to 0 in the matrix
		*/
		std::vector<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor> getignoredverticies(const std::vector<std::vector<std::vector<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor>>>& schemes,
			const std::vector<std::vector<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor>>& targetedperiodsverticies) const;

		// DocString: FMToperatingareascheme::schemestoperiods
		/**
		When generating the yield solution we need to keep track of the periods used in each potential scheme.
		This function just get the corresponding periods of the different schemes from the graph.
		*/
		std::vector<std::vector<int>> schemestoperiods(const std::vector<std::vector<std::vector<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor>>>& schemes, const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>& maingraph) const;
		// DocString: FMToperatingareascheme::getrowsactivitysum
		/**
		Looking at the (dualsolution) for a given sets of (rows) we summarize the total amount of activities of those rows.
		The more activity we have on a row the more the schemes with this row is the one we need to choose.
		*/
		double getrowsactivitysum(const std::vector<int>& rows, const double* dualsolution) const;
		// DocString: FMToperatingareascheme::isthresholdactivityrows
		/**
		Return true if the rows activities are above threshold
		*/
		bool isthresholdactivityrows(const std::vector<int>& rows, const double* dualsolution) const;
		// DocString: FMToperatingareascheme::fillpattern
		/**
		We use this function to generate the final yield solution using a (pattern) like : 110000,
		with the starting period and repetition data members we buil a complete pattern like:
		0110000110000110000
		*/
		std::vector<double>fillpattern(const std::vector<double>& pattern, const int& startat) const;
		// DocString: FMToperatingareascheme::closenoactivity
		/**
		If no activity is detected in the opened OA then close it with no cost to the LP problem.
		*/
		void closenoactivity(std::vector<double>& filleduppattern, const size_t& selected, const double* dualsolution) const;
		// DocString: FMToperatingareascheme::getmaxperiod
		/**
		Get the maximal period of all potential schemes
		*/
		int getmaxperiod() const;
		public:
			// DocString: FMToperatingareascheme::empty
			/**
			Check if the operating area is empty can be possible if there's no potential developpement to operate.
			*/
			bool empty() const;
			// DocString: FMToperatingareascheme::getopeningbinaries
			/**
			Getter returning variables index of the binaries of all potential schemes of the operatingarea.
			*/
			const std::vector<int>& getopeningbinaries() const;
			// DocString: FMToperatingareascheme::getstartingperiod
			/**
			Getter returning starting period at which all schemes of the operating area starts.
			*/
			size_t getstartingperiod() const;
			// DocString: FMToperatingareascheme::getbinariessum
			/**
			Looking at the (primalsolution) we summarize the value of all binary variables for all potential schemes.
			*/
			double getbinariessum(const double* primalsolution) const;
			// DocString: FMToperatingareascheme::getactivitysum
			/**
			Looking at the (dualsolution) we summarize the value of all constraints for all potential schemes.
			*/
			double getactivitysum(const double* dualsolution) const;
			// DocString: FMToperatingareascheme::getthresholdactivity
			/**
			Will return true if every constraint has above threshold
			*/
			bool isthresholdactivity(const double* dualsolution) const;
			// DocString: FMToperatingareascheme::getcommonbinairies
			/**
			Using the greenup data member of each operating area (neighbor and this) we get the map<>
			key = this binary index and elements are a vector of binary indexes of the neighbor.
			We need this function to validate which schemes of an operating area is constraining the usage of 
			the schemes of a other (neighbor).
			*/
			std::map<int, std::vector<int>> getcommonbinairies(const FMToperatingareascheme& neighbor) const;
			// DocString: FMToperatingareascheme::getprimalsolutionindex
			/**
			Only when a operating area is bounded to a given scheme we can use this function that gets
			the index of the bounded scheme looking at the (primalsolution)
			*/
			size_t getprimalsolutionindex(const double* primalsolution) const;
			// DocString: FMToperatingareascheme::getdualsolutionindex
			/**
			This function gets the index of the scheme used (locid) by the operating area
			using the (rows upperbound), is no scheme index is found the function returns false.
			*/
			bool getdualsolutionindex(const double* upperbound, size_t& locid) const;
			// DocString: FMToperatingareascheme::havepotentialsolution
			/**
			Checks if by any chance we have a scheme binary with a value > 0 using the (primalsolution)
			indicating that there's a potential scheme to choose for primal.
			*/
			bool havepotentialsolution(const double* primalsolution) const;
			// DocString: FMToperatingareascheme::haveactivitysolution
			/**
			Checks if by any chance we have a set of scheme constraint with a activity value > 0 using the (dualsolution)
			indicating that there's a potential scheme to choose for dual.
			*/
			bool haveactivitysolution(const double* dualsolution) const;
			// DocString: FMToperatingareascheme::isallprimalbounded
			/**
			Will return false if any binary (lowerbounds) or (upperbounds) is not set to 1.
			*/
			bool isallprimalbounded(const double* lowerbounds, const double* upperbounds) const;
			// DocString: FMToperatingareascheme::isalldualbounded
			/**
			Will return false if any constraint (upperbounds) is different from 0.
			*/
			bool isalldualbounded(const double* upperbounds) const;
			// DocString: FMToperatingareascheme::isprimalbounded
			/**
			Will return true if any binary (lowerbounds) or (upperbounds) is set to 1.
			*/
			bool isprimalbounded(const double* lowerbounds, const double* upperbounds) const;
			// DocString: FMToperatingareascheme::isdualbounded
			/**
			Will return true if any constraints (upperbounds) is set to 0.
			*/
			bool isdualbounded(const double* upperbounds) const;
			// DocString: FMToperatingareascheme::getpotentialprimalschemes
			/**
			Using the (primalsolution), variables (lowerbounds)(upperbounds) and the neighboring operating area,
			this function get the potential sheme indexes. The order begin with the scheme with more area used to
			the scehme with the less area used but > 0.
			*/
			std::vector<size_t>getpotentialprimalschemes(const double* primalsolution, const double* lowerbounds, const double* upperbounds,const std::vector<FMToperatingareascheme>& neighbors) const;
			// DocString: FMToperatingareascheme::getpotentialdualschemes
			/**
			Using the (dualsolution), rows (upperbounds) and the neighboring operating area,
			this function get the potential sheme indexes. The order begin with the scheme with more area used to
			the scehme with the less area used but > 0.
			*/
			std::vector<size_t>getpotentialdualschemes(const double* dualsolution, const double* upperbound, const std::vector<FMToperatingareascheme>& neighbors) const;
			// DocString: FMToperatingareascheme::getressourcestodelete
			/**
			Push all the variables of this operating area to the (colstodelete) vector and
			push all the co of this operating area to the (colstodelete) vector and
			*/
			void getressourcestodelete(std::vector<int>& colstodelete, std::vector<int>& rowstodelete) const; //Remove all variable and constraints related to the operating area from the solver
			// DocString: FMToperatingareascheme::pushbinaries
			/**
			Push all all binaries to a vector<> (targets)
			*/
			void pushbinaries(std::vector<int>& targets) const;
			// DocString: FMToperatingareascheme::unboundallprimalschemes
			/**
			Push binary index into vector (targets) and push into (bounds) 0 and 1.
			*/
			size_t unboundallprimalschemes(std::vector<int>& targets, std::vector<double>& bounds) const;
			// DocString: FMToperatingareascheme::unboundalldualschemes
			/**
			Push constraints index into vector (targets) and push into (bounds) -inf and _area and return the id of the scheme unbounded
			*/
			size_t unboundalldualschemes(std::vector<int>& targets, std::vector<double>& bounds) const;
			// DocString: FMToperatingareascheme::boundallprimalschemes
			/**
			Push variables index into vector (targets) and push (boundvalue ) into (bounds).
			*/
			size_t boundallprimalschemes(std::vector<int>& targets, std::vector<double>& bounds,double boundvalue = 1.0) const;
			// DocString: FMToperatingareascheme::boundalldualschemes
			/**
			Push constraints index into vector (targets) and push (boundvalue ) into (bounds).
			*/
			size_t boundalldualschemes(std::vector<int>& targets, std::vector<double>& bounds) const;
			// DocString: FMToperatingareascheme::boundprimalscheme
			/**
			Using the scheme binary variable of a given (schemeid) push it's variable index into (targets) and push into (bounds) (1 and 1)
			*/
			bool boundprimalscheme(std::vector<int>& targets, std::vector<double>& bounds, const size_t& schemeid) const; 
			// DocString: FMToperatingareascheme::unbounddualscheme
			/**
			Using the constraints of a given (schemeid) push it's constraints index into (targets) and push into (bounds) (-inf and _area)
			and for all other constraints from the other schemes push them into (targets) and push into (bounds) (0 and 0)
			*/
			bool unbounddualscheme(const double* rowactivities, std::vector<int>& targets, std::vector<double>& bounds, const size_t& schemeid, bool looseset=true) const;
			// DocString: FMToperatingareascheme::getprimalsolution
			/**
			Gets the yield solution of the primal problem using (primalsolution) it sums up all binary variables
			into a single vector of double.
			*/
			std::vector<double> getprimalsolution(const double* primalsolution) const;
			// DocString: FMToperatingareascheme::getdualsolution
			/**
			Gets the yield solution of the dual problem using (dualsolution) it set the selected
			scheme solution into a vector of double.
			*/
			std::vector<double> getdualsolution(const double* upperbounds, const double* dualsolution, bool& canbreakneighboring) const;
			// DocString: FMToperatingareascheme::getduallowerbounds
			/**
			Gets the lowerbounds of the solution for bounding the minimal harvested area.
			*/
			std::vector<double> getduallowerbounds(const double* lowerbounds,const double* upperbounds) const;
			// DocString: FMToperatingareascheme::setconstraints
			/**
			Main function setting up constraints and variables using a (matrixbuild) and a primal solution.
			This function will fill-up all data member related to matrix elements (constraints and variables).
			The user has to synchronize matrixbuild to the matrix after iterating on all operatingarea.
			*/
			void setconstraints(const std::vector<std::vector<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor>>& verticies,
				const std::vector<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor>& totalareaverticies,
				const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>& graph,Models::FMTlpsolver& solver,
				const double* primalsolution,
				const std::vector<int>& actionIDS);
			// DocString: FMToperatingareascheme(const FMToperatingarea,const size_t, const size_t,const size_t,const size_t,const size_t)
			/**
			Main FMToperatingareascheme constructor targeting the user. Before synchronizing everything to the solverinterface,
			the user has to provide to the heuristics all the green-up, returntime etc.... for each operating area.
			The minimalarearatio is the minimal ratio needed to open the COS...
			The fullenum is to set the class parameter fullenumeration
			*/
			FMToperatingareascheme(const FMToperatingarea& oparea,const size_t& lopeningtime, const size_t& lreturntime, const size_t& lmaxreturntime,
				const size_t& lrepetition, const size_t& lgreenup,const size_t& lstartingperiod,double minimalarearatio=0.0);
			// DocString: FMToperatingareascheme()
			/**
			Default FMToperatingareascheme constructor
			*/
			FMToperatingareascheme()=default;
			// DocString: FMToperatingareascheme(const FMToperatingarea)
			/**
			FMToperatingareascheme copy constructor
			*/
			FMToperatingareascheme(const FMToperatingareascheme& rhs) = default;
			// DocString: FMToperatingareascheme::operator=
			/**
			FMToperatingareascheme copy assignment
			*/
			FMToperatingareascheme& operator = (const FMToperatingareascheme& rhs)=default;
			// DocString: FMToperatingareascheme::fillboundsnvariables
			/**
			For each scheme consttraints go take the constraints bounds of the actual model.
			*/
			void fillboundsnvariables(const double* lowerb, const double* upperb, std::vector<int>& constraintstargets,std::vector<double>&bounds) const;
			// DocString: FMToperatingareascheme::operator==
			/**
			Comparison operator of FMToperatingareascheme
			*/
			bool operator == (const FMToperatingareascheme& rhs) const;
			// DocString: FMToperatingareascheme::operator!=
			/**
			Comparison operator of FMToperatingareascheme
			*/
			bool operator != (const FMToperatingareascheme& rhs) const;
			// DocString: ~FMToperatingareascheme()
			/**
			Default FMToperatingareascheme destructor
			*/
			~FMToperatingareascheme()=default;
			// DocString: FMToperatingareascheme::presolve
			/**
			Using a FMTmask (selectedmask) and a subset of the original FMTthemes used to construct the FMTmask,
			it returns a presolved FMTmask with potentialy less data.
			*/
			FMToperatingareascheme presolve(const Core::FMTmask& selectedmask,const std::vector<Core::FMTtheme>&presolvedthemes) const;
			// DocString: FMToperatingareascheme::getnumberofscheme
			/**
			Return the number of scheme for the OA. Can only be used if schemestoLP has been set called...
			*/
			size_t getnumberofscheme() const;
			// DocString: FMToperatingareascheme::getnumberofsimplescheme
			/**
			Return the number of simple scheme with fixed returntime + openingtime
			*/
			size_t getnumberofsimplescheme() const;
			// DocString: FMToperatingareascheme::getrejectednodescid
			/**
			
			*/
			const int& getrejectednodescid() const;
			// DocString: FMToperatingareascheme::getschemesperiods
			/**
			
			*/
			const std::vector<std::vector<int>>& getopeningconstraints() const;
			// DocString: FMToperatingareascheme::maximalschemesconstraint
			/**
			
			*/
			const int& getmaximalschemesconstraint() const;

		};

	// DocString: FMToperatingareaschemecomparator
	/**
	This class is made to compare FMToperatingareascheme using the std::find_if() function when FMToperatingarea are in a stl container.
	*/
	class FMToperatingareaschemecomparator
		{
			// DocString: FMToperatingareaschemecomparator::mask
			///FMTmask of the operating area we wish to find.
			Core::FMTmask mask;
		public:
			// DocString: FMToperatingareaschemecomparator(const Core::FMTmask)
			/**
			Constructor of the comparator using the FMTmask as reference.
			*/
			FMToperatingareaschemecomparator(const Core::FMTmask& lmask);
			// DocString: FMToperatingareaschemecomparator::operator()
			/**
			Comparator for FMToperatingareascheme area.
			*/
			bool operator()(const FMToperatingareascheme& oparea) const;

		};

	}
	BOOST_CLASS_EXPORT_KEY(Heuristics::FMToperatingareascheme)
#endif
#endif
