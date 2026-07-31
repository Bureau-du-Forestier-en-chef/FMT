/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTSEM_Hm_included
#define FMTSEM_Hm_included

#include "FMTModel.h"
#include "FMTSpatialSchedule.h"
#include <boost/serialization/vector.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include "FMTSpatialGraphs.h"

namespace Spatial
{
	template <typename T>
	class FMTLayer;
}

namespace Models
{
// DocString: FMTSeModel
/**
@brief Abstract class for spatially explicit models, parent of FMTSesModel and FMTSaModel.
@details Contains a spatial schedule holding the best solution in optimization or the latest solution in simulation.
*/
class FMTEXPORT FMTSeModel : public FMTModel
    {
    public:
		// DocString: FMTSeModel()
		/**
		@brief Default constructor for FMTSeModel.
		*/
        FMTSeModel();
		// DocString: ~FMTSeModel()
		/**
		@brief Default virtual destructor for FMTSeModel.
		*/
		virtual ~FMTSeModel() = default;
		// DocString: FMTSeModel(const FMTSeModel)
		/**
		@brief Copy constructor for FMTSeModel.
		@param[in] rhs the FMTSeModel to copy.
		*/
        FMTSeModel(const FMTSeModel& rhs);
		// DocString: FMTSeModel(const FMTModel, const FMTForest)
		/**
		@brief Construct a FMTSeModel from a model and a forest.
		@param[in] rhs the model.
		@param[in] forest the forest.
		*/
        FMTSeModel(const FMTModel& rhs,const Spatial::FMTForest& forest);
		// DocString: FMTSeModel(const FMTModel, const FMTForest)
		/**
		@brief Construct a FMTSeModel from a model.
		@param[in] rhs the model.
		*/
		FMTSeModel(const FMTModel& rhs);
		// DocString: FMTSeModel::operator=
		/**
		@brief Copy assignment operator for FMTSeModel.
		@param[in] rhs the FMTSeModel to copy.
		@return a reference to this FMTSeModel.
		*/
        FMTSeModel& operator = (const FMTSeModel& rhs);
		// DocString: FMTSeModel::getMapping
		/**
		@brief Return a copy of the actual spatial forest state of each development.
		@return the forest mapping.
		*/
		Spatial::FMTForest getMapping() const;
		// DocString: FMTSeModel::getSpSchedule
		/**
		@brief Return a copy of the spatially explicit solution.
		@return the spatial schedule.
		*/
		inline Spatial::FMTSpatialSchedule getSpSchedule() const
		{
			return m_BestSolution;
		}
		// DocString: FMTSeModel::getDisturbanceStats
		/**
		@brief Return a string of patch statistics (area, perimeter) of the disturbances stack.
		@return the disturbance statistics.
		*/
		std::string getDisturbanceStats() const;
		// DocString: FMTSeModel::getSchedule
		/**
		@brief Return a copy of the operated schedules of the model, which can differ from the potential schedule.
		@param[in] withlock if true includes the locked developments.
		@return the operated schedules.
		*/
		std::vector<Core::FMTSchedule> getSchedule(bool withlock=false) const;
		// DocString: FMTSeModel::setInitialMapping
		/**
		@brief Set the initial forest state, to be set before greedyReferenceBuild is called.
		@param[in] forest the initial forest.
		@return true if the mapping is set else false.
		*/
        bool setInitialMapping(const Spatial::FMTForest& forest);
		// DocString: FMTSeModel::logConstraintsInfeasibilities
		/**
		@brief Log the constraint infeasibilities, spatial or not.
		*/
		void logConstraintsInfeasibilities() const;
		// DocString: FMTSeModel::logConstraintsFactors
		/**
		@brief Log the constraint factors.
		*/
		void logConstraintsFactors() const;
		// DocString: FMTSeModel::preSolve
		/**
		@brief Return a presolved copy of the model, presolving the forest map and the spatial actions.
		@param[in] optionaldevelopments the optional developments.
		@return the presolved model.
		*/
		virtual std::unique_ptr<FMTModel>preSolve(
			std::vector<Core::FMTActualDevelopment> optionaldevelopments = std::vector<Core::FMTActualDevelopment>()) const;
		// DocString: FMTSeModel::postSolve
		/**
		@brief Postsolve the model back into a complete model using the original model.
		@param[in] originalbasemodel the original base model.
		*/
		virtual void postSolve(const FMTModel& originalbasemodel);
		// DocString: FMTSeModel::getOutput
		/**
		@brief Get the output value for a period using the spatial solution.
		@details The map key is the output name when level is standard or totalonly, or the development name when level is developpement.
		@param[in] output the output.
		@param[in] period the period.
		@param[in] level the output level.
		@return a map of names to values.
		*/
		virtual std::map<std::string, double> getOutput(const Core::FMTOutput& output,
			int period, Core::FMToutputlevel level = Core::FMToutputlevel::standard) const;
		// DocString: FMTSeModel::getOutput
		/**
		@brief Get the spatial output value based on the spatial solution.
		@param[in] output the output.
		@param[in] period the period.
		@return the spatial output layer.
		*/
		virtual Spatial::FMTLayer<double> getSpatialOutput(const Core::FMTOutput& output,int period) const;
		// DocString: FMTSeModel::getSolution
		/**
		@brief Get the standard solution for a period, without the natural growth solution.
		@param[in] period the period.
		@param[in] withlock if true includes the locked developments.
		@return the solution schedule.
		*/
		virtual Core::FMTSchedule getSolution(int period, bool withlock = false) const;

		
		// DocString: FMTSeModel::clone
		/**
		@brief Get a clone of the FMTSeModel.
		@return a unique pointer to the cloned model.
		*/
		virtual std::unique_ptr<FMTModel>clone() const;
		// DocString: FMTSeModel::getArea
		/**
		@brief Get the area of a period based on the solution.
		@param[in] period the period.
		@param[in] beforegrowanddeath if true returns before growth and death.
		@return the actual developments.
		*/
		virtual std::vector<Core::FMTActualDevelopment>getArea(int period = 0, bool beforegrowanddeath = false) const;
		// DocString: FMTSeModel::getCopy
		/**
		@brief Return a copy of the model for a period, cleaning the constraints and keeping the objective.
		@param[in] period the period.
		@return the copied model.
		*/
		virtual std::unique_ptr<FMTModel> getCopy(int period = 0) const;
		// DocString: FMTModel::getObjectiveValue
		/**
		@brief Return the global objective value of the actual solution.
		@return the objective value.
		*/
		virtual double getObjectiveValue() const;
		// DocString: FMTSeModel::getSchedules
		/**
		@brief Get the schedules of a spatial solution.
		@param[in] p_SpatialSchedule the spatial schedule.
		@param[in] withlock if true includes the locked developments.
		@return the schedules.
		*/
		std::vector<Core::FMTSchedule> getSchedules(const Spatial::FMTSpatialSchedule& p_SpatialSchedule,
			bool withlock = false) const;
		// DocString: FMTSeModel::getSolutionStatus
		/**
		@brief Get the status of a spatial solution.
		@param[in] p_SpatialSchedule the spatial schedule.
		@param[out] p_Objective the objective value.
		@param[out] p_PrimalInFeasibility the primal infeasibility.
		@param[in] withsense if true applies the sense.
		@param[in] withfactorization if true applies the factorization.
		@param[in] withspatial if true includes the spatial part.
		*/
		void getSolutionStatus(const Spatial::FMTSpatialSchedule& p_SpatialSchedule,
			double& p_Objective, double& p_PrimalInFeasibility,
			bool withsense = true, bool withfactorization = false, bool withspatial = true) const;
		// DocString: FMTSeModel::getConstraintEvaluation
		/**
		@brief Evaluate a constraint with the actual solution.
		@param[in] p_Constraint the constraint index.
		@return the evaluation value.
		*/
		double getConstraintEvaluation(size_t p_Constraint) const;
		
	protected:
		// DocString: FMTSeModel::spschedule
		///Contains the builded spatialsolution latest or best one.
		Spatial::FMTSpatialSchedule m_BestSolution;
		// DocString: FMTSeModel::FMTSpatialGraphs
		///Contains all the SpatialGraphs
		Spatial::FMTSpatialGraphs m_SpatialGraphs;

		// DocString: FMTSeModel::_getGlobalObjective
		/**
		@brief Return the global objective of a spatial schedule.
		@param[in] p_Schedule the spatial schedule.
		@return the global objective.
		*/
		double _getGlobalObjective(const Spatial::FMTSpatialSchedule& p_Schedule) const;

		// DocString: FMTSeModel::_getConstraintsValues
		/**
		@brief Return the constraint values of a spatial schedule.
		@param[in] p_SpatialSchedule the spatial schedule.
		@return the constraint values.
		*/
		std::vector<double> _getConstraintsValues(const Spatial::FMTSpatialSchedule& p_SpatialSchedule) const;

		// DocString: FMTSeModel::_doRefactorization
		/**
		@brief Refactorize a spatial schedule.
		@param[in,out] p_SpatialSchedule the spatial schedule.
		*/
		void _doRefactorization(Spatial::FMTSpatialSchedule& p_SpatialSchedule) const;
		// DocString: FMTSeModel::_getNewSolution
		/**
		@brief Return a new solution from an existing one.
		@param[in] p_FromSolution the solution to start from.
		@return the new solution.
		*/
		Spatial::FMTSpatialSchedule _getNewSolution(const Spatial::FMTSpatialSchedule& p_FromSolution) const;
		// DocString: FMTSeModel::greedyReferenceBuild
		/**
		@brief Find the best spatialization for a schedule using random draws.
		@param[in,out] p_SpatialSchedule the spatial schedule.
		@param[in] schedule the schedule.
		@param[in] randomiterations the number of random iterations.
		@param[in] seed the seed.
		@param[in] tolerance the tolerance.
		@param[in] log if true logs the process.
		@return a map of statistics.
		*/
		std::map<std::string, double> greedyReferenceBuild(
			Spatial::FMTSpatialSchedule& p_SpatialSchedule,
			const Core::FMTSchedule& schedule,
			const size_t& randomiterations,
			unsigned int seed = 0,
			double tolerance = FMT_DBL_TOLERANCE,
			bool log = true) const;
		// DocString: FMTSeModel::_getConstraintFactor
		/**
		@brief Return the factor of a constraint for a gross value.
		@param[in] p_constraint the constraint index.
		@param[in] p_GrossValue the gross value.
		@return the constraint factor.
		*/
		double _getConstraintFactor(size_t p_constraint, double p_GrossValue) const;
		// DocString: FMTSeModel::_isValidFactor
		/**
		@brief Return true if a gross value is a valid factor.
		@param[in] p_GrossValue the gross value.
		@return true if the factor is valid else false.
		*/
		bool _isValidFactor(double p_GrossValue)const;
	private:
		// DocString: FMTSeModel::Serialize
		/**
		@brief Serialize the FMTSeModel through its base FMTModel for multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to serialize to or from.
		@param[in] version the serialization version.
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& boost::serialization::make_nvp("model", boost::serialization::base_object<FMTModel>(*this));
			//ar& BOOST_SERIALIZATION_NVP(m_BestSolution);
		}
		// DocString: FMTSeModel::swapPtr
		/**
		@brief Swap this model for the element at the end of the unique pointer.
		@param[in,out] rhs the unique pointer to swap with.
		*/
		virtual void swapPtr(std::unique_ptr<FMTModel>& rhs);
		// DocString: FMTSeModel::_buildArea
		/**
		@brief Build the area from a forest.
		@param[in] p_Forest the forest.
		*/
		void _buildArea(const Spatial::FMTForest& p_Forest);
		// DocString: FMTSeModel::_buildGraphs
		/**
		@brief Build the spatial graphs.
		@param[in] p_cellSize the cell size.
		*/
		void _buildGraphs(double p_cellSize);
		// DocString: FMTSeModel::_buildSolution
		/**
		@brief Build the solution from a forest.
		@param[in] p_Forest the forest.
		*/
		void _buildSolution(const Spatial::FMTForest& p_Forest);
		// DocString: FMTSeModel::_copyGraphs
		/**
		@brief Copy the spatial graphs.
		@param[in] pToCopy the graphs to copy.
		*/
		void _copyGraphs(const Spatial::FMTSpatialGraphs& pToCopy);
		// DocString: FMTSeModel::_copySolution
		/**
		@brief Copy the solution.
		@param[in] pToCopy the solution to copy.
		*/
		void _copySolution(const Spatial::FMTSpatialSchedule& pToCopy);
		// DocString: FMTSeModel::_getConstraintNumerator
		/**
		@brief Return the numerator of a constraint.
		@param[in] p_constraint the constraint index.
		@return the constraint numerator.
		*/
		double _getConstraintNumerator(size_t p_constraint) const;
		static const double MAX_FACTOR;
		
    };

}

BOOST_CLASS_EXPORT_KEY(Models::FMTSeModel)

#endif // FMTSEM_Hm_included
