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
#ifdef FMTWITHOSI

#ifndef FMToperatingarea_H_INCLUDED
#define FMToperatingarea_H_INCLUDED

#include "FMTgraphdescription.h"
#include "FMTgraph.h"
#include "FMTmask.h"
#include "OsiSolverInterface.hpp"
#include <vector>
#include <map>
#include <memory>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include "FMTmatrixbuild.h"

namespace Heuristics
	{
	/**
	FMToperatingarea gives informations about a given operating area it's mask,neighbors,
	openingtime,greenup,returntime, neighborsperimeter.
	FMToperating area have to be based on static themes (not used within transitions).
	Multiple potential schedules will be generated for the FMToperatingarea formulated as a heuristic and/or MIP.
	*/
	class FMToperatingarea
		{
		/**
		serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
			{
			ar & BOOST_SERIALIZATION_NVP(mask);
			ar & BOOST_SERIALIZATION_NVP(neighbors);
			ar & BOOST_SERIALIZATION_NVP(openingconstraints);
			ar & BOOST_SERIALIZATION_NVP(openingbinaries);
			ar & BOOST_SERIALIZATION_NVP(maximalschemesconstraint);
			ar & BOOST_SERIALIZATION_NVP(schemesperiods);
			ar & BOOST_SERIALIZATION_NVP(openingtime);
			ar & BOOST_SERIALIZATION_NVP(returntime);
			ar & BOOST_SERIALIZATION_NVP(repetition);
			ar & BOOST_SERIALIZATION_NVP(greenup);
			ar & BOOST_SERIALIZATION_NVP(neihgborsperimeter);
			ar & BOOST_SERIALIZATION_NVP(_area);
			}
		///The mask describing the operating area
		Core::FMTmask mask;
		///Neighbors mask of the operating area 
		std::vector<Core::FMTmask>neighbors;
		///Opening constraints indexes first vector is the potential scheme second one is the indexes related to this scheme.
		std::vector<std::vector<int>>openingconstraints;
		///Each potential scheme of the operating area has a binary index.
		std::vector<int>openingbinaries;
		///potentialscheme1 + potentialscheme2 <= 1 constraint index to make sure only one scheme in choose
		int maximalschemesconstraint;
		///The first vector is the scheme index and the second vector is the period id related to each opening constraint index
		std::vector<std::vector<int>>schemesperiods;
		///opening time is the time that a operating area will be available for harvest
		///return time is the time the opening area wont be available for harvest after being harvested.
		///repetition is the repetition of the open + close pattern only use when generating the solution 
		///The greenup is the number of periods before a neighbor of a newly harvested block can be harvested.
		///The starting period is the period from where the heuristic needs to fin a schedule (in case of model update).
		size_t openingtime,returntime,repetition,greenup,startingperiod;
		///neighborsperimeter is the ratio a operatingarea needs to share to a other operatingarea to be considered neighbor.
		double neihgborsperimeter;
		///The initial area of the operating area used as big M for the MIP.
		double _area;
		/**
			Get the area of the operating area base on a (primalsolution) 
			for the initial (verticies) of the FMTlpmodel graph.
		*/
		double getarea(const double* primalsolution, const Graph::FMTgraph& maingraph, const std::vector<Graph::FMTvertex_descriptor>& verticies) const;
		/**
			Get the best possible scheme looking at the primal solution,
			return the index of that scheme.
		*/
		size_t getbestschemeid(const double* primalsolution) const;
		/**
			Generate potential schemes for the operating area
			here is 2 different schemes for a returntime = 4 and a openingtime = 2:
			00011000011000
			11000011000011
			This fonction generate all potential schemes of the operating area if the "harvest stade" exist within the matrix.
		*/
		std::vector<std::vector<std::vector<Graph::FMTvertex_descriptor>>> generateschemes(const std::vector<std::vector<Graph::FMTvertex_descriptor>>& verticies); 
		/*void schemestoLP(const std::vector<std::vector<std::vector<Graph::FMTvertex_descriptor>>>& schemes,
						const std::vector<std::vector<Graph::FMTvertex_descriptor>>& periodics,
						const std::vector<Graph::FMTvertex_descriptor>& totalareaverticies,
						std::shared_ptr<OsiSolverInterface> solverinterface,
						const Graph::FMTgraph& maingraph,const std::vector<int>& actionIDS); */
		/**
		Using the generated (schemes) from the generateschemes functions, and all periodic verticies (periodics),
		initial verticies (totalareaverticies) and the (primalsolution) for calculating the _area, solverinterface, graph and actionids.
		The function fills the opening constraints and opening binairies member data and also add the newly formulated
		constraints and binairies to the solverinterface. But the model is kept linear.
		*/
		void schemestoLP(const std::vector<std::vector<std::vector<Graph::FMTvertex_descriptor>>>& schemes,
			const std::vector<std::vector<Graph::FMTvertex_descriptor>>& periodics,
			const std::vector<Graph::FMTvertex_descriptor>& totalareaverticies,
			Models::FMTmatrixbuild& matrixbuild,
			const double* primalsolution,
			const Graph::FMTgraph& maingraph, const std::vector<int>& actionIDS); 
		/**
		When generating the yield solution we need to keep track of the periods used in each potential scheme.
		This function just get the corresponding periods of the different schemes from the graph.
		*/
		std::vector<std::vector<int>> schemestoperiods(const std::vector<std::vector<std::vector<Graph::FMTvertex_descriptor>>>& schemes, const Graph::FMTgraph& maingraph) const;
		/**
		Looking at the (dualsolution) for a given sets of (rows) we summarize the total amount of activities of those rows.
		The more activity we have on a row the more the schemes with this row is the one we need to choose.
		*/
		double getrowsactivitysum(const std::vector<int>& rows, const double* dualsolution) const;
		/**
		We use this function to generate the final yield solution using a (pattern) like : 110000,
		with the starting period and repetition data members we buil a complete pattern like:
		0110000110000110000
		*/
		std::vector<double>fillpattern(const std::vector<double>& pattern, const int& startat) const;
		public:
			/**
			Getter returning a copy of the neighbors masks data of the operatingarea.
			*/
			std::vector<Core::FMTmask> getneighbors() const;
			/**
			Check if the operating area is empty can be possible if there's no potential developpement to operate.
			*/
			bool empty() const;
			/**
			Getter returning variables index of the binaries of all potential schemes of the operatingarea.
			*/
			const std::vector<int>& getopeningbinaries() const;
			/**
			Getter returning the percentage of perimeter the oprating area needs to share with a other operating area,
			to be considered neighbor.
			*/
			double getneihgborsperimeter() const;
			/**
			Getter returning mask of the operating area.
			*/
			Core::FMTmask getmask() const;
			/**
			Getter returning starting period at which all schemes of the operating area starts.
			*/
			size_t getstartingperiod() const;
			/**
			Looking at the (primalsolution) we summarize the value of all binary variables for all potential schemes.
			*/
			double getbinariessum(const double* primalsolution) const;
			/**
			Looking at the (dualsolution) we summarize the value of all constraints for all potential schemes.
			*/
			double getactivitysum(const double* dualsolution) const;
			/**
			Using the greenup data member of each operating area (neighbor and this) we get the map<>
			key = this binary index and elements are a vector of binary indexes of the neighbor.
			We need this function to validate which schemes of an operating area is constraining the usage of 
			the schemes of a other (neighbor).
			*/
			std::map<int, std::vector<int>> getcommonbinairies(const FMToperatingarea& neighbor) const;
			/**
			Only when a operating area is bounded to a given scheme we can use this function that gets
			the index of the bounded scheme looking at the (primalsolution)
			*/
			size_t getprimalsolutionindex(const double* primalsolution) const; 
			/**
			This function gets the index of the scheme used (locid) by the operating area
			using the (rows upperbound), is no scheme index is found the function returns false.
			*/
			bool getdualsolutionindex(const double* upperbound, size_t& locid) const;
			/**
			Checks if by any chance we have a scheme binary with a value > 0 using the (primalsolution)
			indicating that there's a potential scheme to choose for primal.
			*/
			bool havepotentialsolution(const double* primalsolution) const;
			/**
			Checks if by any chance we have a set of scheme constraint with a activity value > 0 using the (dualsolution)
			indicating that there's a potential scheme to choose for dual.
			*/
			bool haveactivitysolution(const double* dualsolution) const;
			/**
			Will return false if any binary (lowerbounds) or (upperbounds) is not set to 1.
			*/
			bool isallprimalbounded(const double* lowerbounds, const double* upperbounds) const;
			/**
			Will return false if any constraint (upperbounds) is different from 0.
			*/
			bool isalldualbounded(const double* upperbounds) const;
			/**
			Will return true if any binary (lowerbounds) or (upperbounds) is set to 1.
			*/
			bool isprimalbounded(const double* lowerbounds, const double* upperbounds) const;
			/**
			Will return true if any constraints (upperbounds) is set to 0.
			*/
			bool isdualbounded(const double* upperbounds) const;
			/**
			Using the (primalsolution), variables (lowerbounds)(upperbounds) and the neighboring operating area,
			this function get the potential sheme indexes. The order begin with the scheme with more area used to
			the scehme with the less area used but > 0.
			*/
			std::vector<size_t>getpotentialprimalschemes(const double* primalsolution, const double* lowerbounds, const double* upperbounds,const std::vector<FMToperatingarea>& neighbors) const;
			/**
			Using the (dualsolution), rows (upperbounds) and the neighboring operating area,
			this function get the potential sheme indexes. The order begin with the scheme with more area used to
			the scehme with the less area used but > 0.
			*/
			std::vector<size_t>getpotentialdualschemes(const double* dualsolution, const double* upperbound, const std::vector<FMToperatingarea>& neighbors) const;
			/**
			Push all the variables of this operating area to the (colstodelete) vector and
			push all the co of this operating area to the (colstodelete) vector and
			*/
			void getressourcestodelete(std::vector<int>& colstodelete, std::vector<int>& rowstodelete) const; //Remove all variable and constraints related to the operating area from the solver
			/**
			Push all all binaries to a vector<> (targets)
			*/
			void pushbinaries(std::vector<int>& targets) const; 
			/**
			Push binary index into vector (targets) and push into (bounds) 0 and 1.
			*/
			size_t unboundallprimalschemes(std::vector<int>& targets, std::vector<double>& bounds) const;
			/**
			Push constraints index into vector (targets) and push into (bounds) -inf and _area.
			*/
			size_t unboundalldualschemes(std::vector<int>& targets, std::vector<double>& bounds) const;
			/**
			Push variables index into vector (targets) and push (boundvalue ) into (bounds).
			*/
			size_t boundallprimalschemes(std::vector<int>& targets, std::vector<double>& bounds,double boundvalue = 1.0) const;
			/**
			Push constraints index into vector (targets) and push (boundvalue ) into (bounds).
			*/
			size_t boundalldualschemes(std::vector<int>& targets, std::vector<double>& bounds) const;
			/**
			Using the scheme binary variable of a given (schemeid) push it's variable index into (targets) and push into (bounds) (1 and 1)
			*/
			bool boundprimalscheme(std::vector<int>& targets, std::vector<double>& bounds, const size_t& schemeid) const; 
			/**
			Using the constraints of a given (schemeid) push it's constraints index into (targets) and push into (bounds) (-inf and _area)
			and for all other constraints from the other schemes push them into (targets) and push into (bounds) (0 and 0)
			*/
			bool unbounddualscheme(std::vector<int>& targets, std::vector<double>& bounds, const size_t& schemeid) const; 
			/**
			Gets the yield solution of the primal problem using (primalsolution) it sums up all binary variables
			into a single vector of double.
			*/
			std::vector<double> getprimalsolution(const double* primalsolution) const;
			/**
			Gets the yield solution of the dual problem using (dualsolution) it set the selected
			scheme solution into a vector of double.
			*/
			std::vector<double> getdualsolution(const double* upperbounds) const;
			/**
			Setter for the neighbors mask member data.
			*/
			void setneighbors(const std::vector<Core::FMTmask>& lneighbors);
			/**
			Main function setting up constraints and variables using a (matrixbuild) and a primal solution.
			This function will fill-up all data member related to matrix elements (constraints and variables).
			The user has to synchronize matrixbuild to the matrix after iterating on all operatingarea.
			*/
			void setconstraints(const std::vector<std::vector<Graph::FMTvertex_descriptor>>& verticies,
				const std::vector<Graph::FMTvertex_descriptor>& totalareaverticies,
				const Graph::FMTgraph& graph,Models::FMTmatrixbuild& matrixbuild,
				const double* primalsolution,
				const std::vector<int>& actionIDS);
			/**
			Main FMToperatingarea constructor targeting the user. Before synchronizing everything to the solverinterface,
			the user has to provide to the heuristics all the green-up, returntime etc.... for each operating area.
			*/
			FMToperatingarea(const Core::FMTmask& lmask,const size_t& lopeningtime, const size_t& lreturntime,
				const size_t& lrepetition, const size_t& lgreenup,const size_t& lstartingperiod,
				const double& lneihgborsperimeter);
			/**
			Default FMToperatingarea constructor
			*/
			FMToperatingarea()=default;
			/**
			FMToperatingarea copy constructor
			*/
			FMToperatingarea(const FMToperatingarea&) = default;
			/**
			FMToperatingarea copy assignment
			*/
			FMToperatingarea& operator = (const FMToperatingarea& )=default;
			/**
			Comparison operator of FMToperatingarea
			*/
			bool operator == (const FMToperatingarea& rhs) const;
			/**
			Comparison operator of FMToperatingarea
			*/
			bool operator != (const FMToperatingarea& rhs) const;
			/**
			Default FMToperatingarea destructor
			*/
			~FMToperatingarea()=default;

		};

	/**
	This class is made to compare FMToperatingarea using the std::find_if() function when FMToperatingarea are in a stl container.
	*/
	class FMToperatingareacomparator
		{
			///FMTmask of the operating area we wish to find.
			Core::FMTmask mask;
		public:
			/**
			Constructor of the comparator using the FMTmask as reference.
			*/
			FMToperatingareacomparator(const Core::FMTmask& lmask);
			bool operator()(const FMToperatingarea& oparea) const;

		};

	}

#endif
#endif