/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI

#ifndef FMTlpheuristic_Hm_included
#define FMTlpheuristic_Hm_included


#include "FMTLpSolver.h"
#include <random>
#include <memory>
#include <map>
#include <vector>
#include <chrono>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include "FMTsolverinterface.h"

/// Namespace regrouping all tools for solving LP/MIP problems using an heuristic.
namespace Heuristics
{
	// DocString: FMTLpHeuristic
	/**
	@brief Base class for solving LP and MIP problems using a heuristic, holding a random generator and its own copy of the solver interface.
	@details The user can decide to only generate a good initial solution or to generate a good initial solution and then try to find the optimum using a branch and bound solver.
	*/
	class FMTEXPORT FMTLpHeuristic : public Models::FMTLpSolver
	{
		// DocString: FMTLpHeuristic::save
		/**
		@brief Save function used for serialization to do multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to save to.
		@param[in] version the serialization version.
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void save(Archive& ar, const unsigned int version) const
		{
			ar & boost::serialization::make_nvp("lpsolve", boost::serialization::base_object<FMTLpSolver>(*this));
			ar & BOOST_SERIALIZATION_NVP(seed);
			ar & BOOST_SERIALIZATION_NVP(usingsolvercopy);
		}
		// DocString: FMTLpHeuristic::load
		/**
		@brief Load function used for serialization to do multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to load from.
		@param[in] version the serialization version.
		*/
		template<class Archive>
		void load(Archive& ar, const unsigned int version)
		{
			ar & boost::serialization::make_nvp("lpsolve", boost::serialization::base_object<FMTLpSolver>(*this));
			this->passInMessageHandler(*this->_logger);
			ar & BOOST_SERIALIZATION_NVP(seed);
			this->setGeneratorSeed(seed);
			ar & BOOST_SERIALIZATION_NVP(usingsolvercopy);
		}
		BOOST_SERIALIZATION_SPLIT_MEMBER()
	protected:
		// DocString: FMTLpHeuristic::generator
		///std random number generator of the heuristic each heuristic has it's own generator to generate different solution
		std::default_random_engine generator;
		// DocString: FMTLpHeuristic::seed
		///The random engine seed need to be a data member when serializing the class
		size_t seed;
		// DocString: FMTLpHeuristic::usingsolvercopy
		///If true the heuristic will work from it's own copy of solverinterface, else it's going to work on the FMTLpModel solverinterface.
		bool usingsolvercopy;
	public:
		// DocString: FMTLpHeuristic::setGeneratorSeed
		/**
		@brief Set the seed of the random generator.
		@param[in] lseed the seed.
		*/
		void setGeneratorSeed(const size_t& lseed);
		// DocString: FMTLpHeuristic(Models::FMTLpSolver&,size_t lseed,bool copysolver)
		/**
		@brief Main constructor for FMTLpHeuristic from a base solver, taking the information coming from a FMTLpModel.
		@details Constructing the heuristic copies the solver interface when copysolver is true; after construction the heuristic no longer needs the FMTLpModel.
		@param[in,out] basesolve the base solver.
		@param[in] lseed the seed.
		@param[in] copysolver if true copies the solver.
		*/
		FMTLpHeuristic(Models::FMTLpSolver& basesolve, size_t lseed = 0,bool copysolver = true);
		// DocString: FMTLpHeuristic(const Models::FMTsolverinterface&,const size_t&)
		/**
		@brief Construct a FMTLpHeuristic from a solver type and a seed.
		@param[in] interfacetype the solver interface type.
		@param[in] lseed the seed.
		*/
		FMTLpHeuristic(const Models::FMTsolverinterface& interfacetype,const size_t& lseed);
		// DocString: FMTLpHeuristic()
		/**
		@brief Default constructor for FMTLpHeuristic.
		*/
		FMTLpHeuristic()=default;
		// DocString: FMTLpHeuristic(const FMTLpHeuristic&)
		/**
		@brief Copy constructor for FMTLpHeuristic.
		@param[in] rhs the FMTLpHeuristic to copy.
		*/
		FMTLpHeuristic(const FMTLpHeuristic& rhs);
		// DocString: FMTLpHeuristic(const FMTLpHeuristic&&)
		/**
		@brief Move constructor for FMTLpHeuristic.
		@param[in,out] rhs the FMTLpHeuristic to move from.
		*/
		FMTLpHeuristic(FMTLpHeuristic&& rhs)=default;
		// DocString: FMTLpHeuristic::operator=
		/**
		@brief Copy assignment operator for FMTLpHeuristic.
		@param[in] rhs the FMTLpHeuristic to copy.
		@return a reference to this FMTLpHeuristic.
		*/
		FMTLpHeuristic& operator = (const FMTLpHeuristic& rhs);
		// DocString: FMTLpHeuristic::branchNBoundSolve
		/**
		@brief Solve the problem using branch and bound on the primal formulation, using the heuristic solution as a starting MIP solution when initialSolve has been called.
		@return true if a solution is found else false.
		*/
		virtual bool branchNBoundSolve();
		// DocString: FMTLpHeuristic::greedyPass
		/**
		@brief Do a greedy pass to try to improve the solution.
		@param[in] initsol the initial solution value.
		@param[in] iteration the iteration.
		@return true if the pass improved the solution else false.
		*/
		virtual bool greedyPass(const double& initsol,const unsigned int& iteration);
		// DocString: FMTLpHeuristic::parallelOptimize
		/**
		@brief Optimize the heuristic in parallel until the maximum number of iterations or the maximum time is reached.
		@param[in] initbestsolution the initial best solution value.
		@param[in] iterations the number of iterations.
		@param[in] maxtime the maximum time.
		@param[in] Starttime the start time.
		*/
		virtual void parallelOptimize(const double& initbestsolution, const unsigned int& iterations,const double& maxtime, const std::chrono::steady_clock::time_point& Starttime);
		// DocString: ~FMTLpHeuristic()
		/**
		@brief Default virtual destructor for FMTLpHeuristic.
		*/
		virtual ~FMTLpHeuristic() = default;

	};

}
BOOST_CLASS_EXPORT_KEY(Heuristics::FMTLpHeuristic)
#endif
#endif
