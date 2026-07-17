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
	FMToperatingareaheuristic is a heuristics made to solve spatialization problem
	across landscape for multiple operating areas. MIP Forest planning problem tend to be
	symmetrical and tought to solve using a regular MIP solver. This heuristics is made to quickly
	generate good enought starting solution for those kind of Forest management problem.
	Before using this class the user must have sets all parameters of a vector of FMTOperatingArea
	so that the heuristic can sets the constraints and variables of each operating area into
	the matrix. It's up to the user to decide to just generate a good initialsolution or
	generate a good initialsolution and then try to find the optimaly using a BnB solver.
	*/
	class FMTEXPORT FMTLpHeuristic : public Models::FMTLpSolver
	{
		// DocString: FMTLpHeuristic::save
		/**
		Save function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
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
		Load function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
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
		Setter for the seed data member
		*/
		void setGeneratorSeed(const size_t& lseed);
		// DocString: FMTLpHeuristic(Models::FMTLpSolver&,size_t lseed,bool copysolver)
		/**
		Main constructor used to initialize a FMToperatingareaheuristic, the constructor needs
		alot of information comming from a FMTLpModel. Also constructing a FMToperatingareaheuristic is
		not a small task if copysolver = true because the solverinterface beging the FMTLpModel needs
		to be copied. After constructing the object the operatingareaheuristic doesn't need any information
		about the FMTLpModel.
		*/
		FMTLpHeuristic(Models::FMTLpSolver& basesolve, size_t lseed = 0,bool copysolver = true);
		// DocString: FMTLpHeuristic()
		/**
		Constructor based on a simple solver type and seed.
		*/
		FMTLpHeuristic(const Models::FMTsolverinterface& interfacetype,const size_t& lseed);
		// DocString: FMTLpHeuristic()
		/**
		Default FMTLpHeuristic constructor
		*/
		FMTLpHeuristic()=default;
		// DocString: FMTLpHeuristic(const FMTLpHeuristic&)
		/**
		FMTLpHeuristic copy constructor
		*/
		FMTLpHeuristic(const FMTLpHeuristic& rhs);
		// DocString: FMTLpHeuristic(const FMTLpHeuristic&&)
		/**
		FMTLpHeuristic move constructor
		*/
		FMTLpHeuristic(FMTLpHeuristic&& rhs)=default;
		// DocString: FMTLpHeuristic::operator=
		/**
		FMTLpHeuristic copy assignment
		*/
		FMTLpHeuristic& operator = (const FMTLpHeuristic& rhs);
		// DocString: FMTLpHeuristic::branchNBoundSolve
		/**
		Solve problem using Branch and bound on the primal formulation. If the function is called after a call to initialSolve()
		it's going to use the heuristic solution has a starting MIP solution, if not it's going to directly use the BnB on the formulated problem.
		*/
		virtual bool branchNBoundSolve();
		// DocString: FMTLpHeuristic::greedyPass
		/**

		*/
		virtual bool greedyPass(const double& initsol,const unsigned int& iteration);
		virtual void parallelOptimize(const double& initbestsolution, const unsigned int& iterations,const double& maxtime, const std::chrono::steady_clock::time_point& Starttime);
		// DocString: FMTLpHeuristic::isfeasible
		/**
		Return true if the actual solution of the heuristic is feasible.
		*/
		virtual ~FMTLpHeuristic() = default;

	};

}
BOOST_CLASS_EXPORT_KEY(Heuristics::FMTLpHeuristic)
#endif
#endif
