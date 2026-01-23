/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTSEM_Hm_included
#define FMTSEM_Hm_included

#include "FMTmodel.h"
#include "FMTspatialschedule.h"
#include <boost/serialization/vector.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include "FMTSpatialGraphs.h"

namespace Spatial
{
	template <typename T>
	class FMTlayer;
}

namespace Models
{
// DocString: FMTsemodel
/**
This model is an abstract class for spatialy explicit model. It's the parent of FMTsesmodel and FMTsamodel.
It contains a spatialschedule (the best solution) in case of optimization or the latest solution in term of
simulation.
*/
class FMTEXPORT FMTsemodel : public FMTmodel
    {
    public:
		// DocString: FMTsemodel()
		/**
		Default constructor of FMTsemodel
		*/
        FMTsemodel();
		// DocString: ~FMTsemodel()
		/**
		Default destructor of FMTsemodel
		*/
		virtual ~FMTsemodel() = default;
		// DocString: FMTsemodel(const FMTsemodel)
		/**
		Copy constructor of FMTsemodel
		*/
        FMTsemodel(const FMTsemodel& rhs);
		// DocString: FMTsemodel(const FMTmodel, const FMTforest)
		/**
		Parent constructor for FMTsemodel (easiest way to get information from a FMTmodel) and with an FMTforest.
		*/
        FMTsemodel(const FMTmodel& rhs,const Spatial::FMTforest& forest);
		// DocString: FMTsemodel(const FMTmodel, const FMTforest)
		/**
		Parent constructor for FMTsemodel (easiest way to get information from a FMTmodel)
		*/
		FMTsemodel(const FMTmodel& rhs);
		// DocString: FMTsemodel::operator=
		/**
		Copy assignment of FMTsemodel
		*/
        FMTsemodel& operator = (const FMTsemodel& rhs);
		// DocString: FMTsemodel::getmapping
		/**
		Getter returning a copy the actual spatial forest stades of each FMTdevelopement (map).
		*/
		Spatial::FMTforest getmapping() const;
		// DocString: FMTsemodel::getspschedule
		/**
		Getter returning a copy of the spatially explicit solution.
		*/
		inline Spatial::FMTSpatialSchedule getspschedule() const
		{
			return m_BestSolution;
		}
		// DocString: FMTsemodel::getdisturbancestats
		/**
		Getter returning a string of patch stats (area,perimeter ....) that are ine the disturbances stack.
		*/
		std::string getdisturbancestats() const;
		// DocString: FMTsemodel::getschedule
		/**
		Getter returning a copy of the operated schedules of the FMTsemodel.
		The operated schedule can differ from the potential schedule provided by the user in the function
		greedyreferencedbuild(). Which we call spatialisation impact.
		*/
		std::vector<Core::FMTschedule> getschedule(bool withlock=false) const;
		// DocString: FMTsemodel::setinitialmapping
		/**
		Setter of the initial forest state (spatial map of FMTdevelopment)
		Has to be set before greedyreferencedbuild() is called.
		*/
        bool setinitialmapping(const Spatial::FMTforest& forest);
		// DocString: FMTsemodel::LogConstraintsInfeasibilities
		/**
		Log the constraints infeasibilities spatial or not spatial
		*/
		void LogConstraintsInfeasibilities() const;
		// DocString: FMTsemodel::LogConstraintsFactors
		/**
		Log the constraints factors
		*/
		void LogConstraintsFactors() const;
		// DocString: FMTsemodel::presolve
		/**
		Presolve the semodel to get a more simple model call original presolve() and presolve the
		FMTforest map and the spatial acitons.
		*/
		virtual std::unique_ptr<FMTmodel>presolve(
			std::vector<Core::FMTactualdevelopment> optionaldevelopments = std::vector<Core::FMTactualdevelopment>()) const;
		// DocString: FMTsemodel::postsolve
		/**
		Using the original FMTmodel it postsolve the actual ses model to turn it back into a complete model with all themes,
		actions and outputs of the original not presolved model.
		*/
		virtual void postsolve(const FMTmodel& originalbasemodel);
		// DocString: FMTsemodel::getoutput
		/**
		Get the output value of a output for a given period using the spatial solution.
		the map key returned consist of output name
		if level == FMToutputlevel::standard || level == FMToutputlevel::totalonly,
		or developement name if level == FMToutputlevel::developpement
		*/
		virtual std::map<std::string, double> getoutput(const Core::FMToutput& output,
			int period, Core::FMToutputlevel level = Core::FMToutputlevel::standard) const;
		// DocString: FMTsemodel::getoutput
		/**
		Get the spatial output value based on the spatial solution.
		*/
		virtual Spatial::FMTlayer<double> getspatialoutput(const Core::FMToutput& output,int period) const;
		// DocString: FMTsemodel::getsolution
		/**
		Get the standard solution for a given period (FMTschedule dont have natural growth solution included).
		If with lock is true then the schedule will contain locked developement.
		*/
		virtual Core::FMTschedule getsolution(int period, bool withlock = false) const;

		
		// DocString: FMTsemodel::clone
		/**
		Get a clone of the FMTsemodel
		*/
		virtual std::unique_ptr<FMTmodel>clone() const;
		// DocString: FMTsemodel::getarea
		/**
		@brief Get the area of a given period based on the solution of the model.
		@param[in] period the period selected
		@param[in] beforegrowanddeath true if we want before the growth (true) or after (false)
		@return the vector of actualdevelopment...
		*/
		virtual std::vector<Core::FMTactualdevelopment>getarea(int period = 0, bool beforegrowanddeath = false) const;
		// DocString: FMTsemodel::getcopy
		/**
		This function returns a copy of the FMTmodel of the selected period.
		The function is going to clean the FMTconstraints and keep the objective.
		*/
		virtual std::unique_ptr<FMTmodel> getcopy(int period = 0) const;
		// DocString: FMTmodel::getobjectivevalue
		/**
		Return the value of the globalobjective of the actual solution
		*/
		virtual double getobjectivevalue() const;
		// DocString: FMTsemodel::GetSchedules
		/**
		@brief Get the schedules of the spatial solution
		@param[in] p_SpatialSchedule spatial schedule
		@param[in] withlock lock in schedule
		@return the vector of schedules
		*/
		std::vector<Core::FMTschedule> GetSchedules(const Spatial::FMTSpatialSchedule& p_SpatialSchedule,
			bool withlock = false) const;
		// DocString: FMTsemodel::GetSolutionStatus
		/**
		@brief Get the solution status
		*/
		void GetSolutionStatus(const Spatial::FMTSpatialSchedule& p_SpatialSchedule,
			double& p_Objective, double& p_PrimalInFeasibility,
			bool withsense = true, bool withfactorization = false, bool withspatial = true) const;
		// DocString: FMTsemodel::GetConstraintEvaluation
		/**
		@brief Evaluate the constraint with the actual solution
		@return the evaluation value.
		*/
		double GetConstraintEvaluation(size_t p_Constraint) const;
		
	protected:
		// DocString: FMTsemodel::spschedule
		///Contains the builded spatialsolution latest or best one.
		Spatial::FMTSpatialSchedule m_BestSolution;
		// DocString: FMTsemodel::FMTSpatialGraphs
		///Contains all the SpatialGraphs
		Spatial::FMTSpatialGraphs m_SpatialGraphs;

		double GetGlobalObjective(const Spatial::FMTSpatialSchedule& p_Schedule) const;

		std::vector<double> GetConstraintsValues(const Spatial::FMTSpatialSchedule& p_SpatialSchedule) const;

		void DoReFactortorization(Spatial::FMTSpatialSchedule& p_SpatialSchedule) const;
		Spatial::FMTSpatialSchedule GetNewSolution(const Spatial::FMTSpatialSchedule& p_FromSolution) const;
		std::map<std::string, double> GreedyReferenceBuild(
			Spatial::FMTSpatialSchedule& p_SpatialSchedule,
			const Core::FMTschedule& schedule,
			const size_t& randomiterations,
			unsigned int seed = 0,
			double tolerance = FMT_DBL_TOLERANCE,
			bool log = true) const;
	private:
		// DocString: FMTsemodel::Serialize
		/**
		Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& boost::serialization::make_nvp("model", boost::serialization::base_object<FMTmodel>(*this));
			//ar& BOOST_SERIALIZATION_NVP(m_BestSolution);
		}
		virtual void swap_ptr(std::unique_ptr<FMTmodel>& rhs);
		void _BuildArea(const Spatial::FMTforest& p_Forest);
		void _BuildGraphs(double p_cellSize);
		void _BuildSolution(const Spatial::FMTforest& p_Forest);
		void _CopyGraphs(const Spatial::FMTSpatialGraphs& pToCopy);
		void _CopySolution(const Spatial::FMTSpatialSchedule& pToCopy);
    };

}

BOOST_CLASS_EXPORT_KEY(Models::FMTsemodel)

#endif // FMTSEM_Hm_included
