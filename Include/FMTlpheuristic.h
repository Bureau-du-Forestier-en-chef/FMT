/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI

#ifndef FMTlpheuristic_H_INCLUDED
#define FMTlpheuristic_H_INCLUDED


#include "FMTlpsolver.h"
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
	// DocString: FMTlpheuristic
	/**
	FMToperatingareaheuristic is a heuristics made to solve spatialization problem
	across landscape for multiple operating areas. MIP Forest planning problem tend to be
	symmetrical and tought to solve using a regular MIP solver. This heuristics is made to quickly
	generate good enought starting solution for those kind of Forest management problem.
	Before using this class the user must have sets all parameters of a vector of FMToperatingarea
	so that the heuristic can sets the constraints and variables of each operating area into
	the matrix. It's up to the user to decide to just generate a good initialsolution or
	generate a good initialsolution and then try to find the optimaly using a BnB solver.
	*/
	class FMTEXPORT FMTlpheuristic : public Models::FMTlpsolver
	{
		// DocString: FMTlpheuristic::save
		/**
		Save function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void save(Archive& ar, const unsigned int version) const
		{
			ar & boost::serialization::make_nvp("lpsolve", boost::serialization::base_object<FMTlpsolver>(*this));
			ar & BOOST_SERIALIZATION_NVP(seed);
			ar & BOOST_SERIALIZATION_NVP(usingsolvercopy);
		}
		// DocString: FMTlpheuristic::load
		/**
		Load function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		template<class Archive>
		void load(Archive& ar, const unsigned int version)
		{
			ar & boost::serialization::make_nvp("lpsolve", boost::serialization::base_object<FMTlpsolver>(*this));
			this->passinmessagehandler(*this->_logger);
			ar & BOOST_SERIALIZATION_NVP(seed);
			this->setgeneratorseed(seed);
			ar & BOOST_SERIALIZATION_NVP(usingsolvercopy);
		}
		BOOST_SERIALIZATION_SPLIT_MEMBER()
	protected:
		// DocString: FMTlpheuristic::generator
		///std random number generator of the heuristic each heuristic has it's own generator to generate different solution
		std::default_random_engine generator;
		// DocString: FMTlpheuristic::seed
		///The random engine seed need to be a data member when serializing the class
		size_t seed;
		// DocString: FMTlpheuristic::usingsolvercopy
		///If true the heuristic will work from it's own copy of solverinterface, else it's going to work on the FMTlpmodel solverinterface.
		bool usingsolvercopy;
	public:
		// DocString: FMTlpheuristic::setgeneratorseed
		/**
		Setter for the seed data member
		*/
		void setgeneratorseed(const size_t& lseed);
		// DocString: FMTlpheuristic(Models::FMTlpsolver&,size_t lseed,bool copysolver)
		/**
		Main constructor used to initialize a FMToperatingareaheuristic, the constructor needs
		alot of information comming from a FMTlpmodel. Also constructing a FMToperatingareaheuristic is
		not a small task if copysolver = true because the solverinterface beging the FMTlpmodel needs
		to be copied. After constructing the object the operatingareaheuristic doesn't need any information
		about the FMTlpmodel.
		*/
		FMTlpheuristic(Models::FMTlpsolver& basesolve, size_t lseed = 0,bool copysolver = true);
		// DocString: FMTlpheuristic()
		/**
		Constructor based on a simple solver type and seed.
		*/
		FMTlpheuristic(const Models::FMTsolverinterface& interfacetype,const size_t& lseed);
		// DocString: FMTlpheuristic()
		/**
		Default FMTlpheuristic constructor
		*/
		FMTlpheuristic()=default;
		// DocString: FMTlpheuristic(const FMTlpheuristic&)
		/**
		FMTlpheuristic copy constructor
		*/
		FMTlpheuristic(const FMTlpheuristic& rhs);
		// DocString: FMTlpheuristic(const FMTlpheuristic&&)
		/**
		FMTlpheuristic move constructor
		*/
		FMTlpheuristic(FMTlpheuristic&& rhs)=default;
		// DocString: FMTlpheuristic::operator=
		/**
		FMTlpheuristic copy assignment
		*/
		FMTlpheuristic& operator = (const FMTlpheuristic& rhs);
		// DocString: FMTlpheuristic::branchnboundsolve
		/**
		Solve problem using Branch and bound on the primal formulation. If the function is called after a call to initialsolve()
		it's going to use the heuristic solution has a starting MIP solution, if not it's going to directly use the BnB on the formulated problem.
		*/
		virtual bool branchnboundsolve();
		// DocString: FMTlpheuristic::greedypass
		/**

		*/
		virtual bool greedypass(const double& initsol,const unsigned int& iteration);
		virtual void paralleloptimize(const double& initbestsolution, const unsigned int& iterations,const double& maxtime, const std::chrono::steady_clock::time_point& Starttime);
		// DocString: FMTlpheuristic::isfeasible
		/**
		Return true if the actual solution of the heuristic is feasible.
		*/
		virtual ~FMTlpheuristic() = default;

	};

}
BOOST_CLASS_EXPORT_KEY(Heuristics::FMTlpheuristic)
#endif
#endif
