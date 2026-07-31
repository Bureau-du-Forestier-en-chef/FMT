/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTSESM_Hm_included
#define FMTSESM_Hm_included

#include "FMTSeModel.h"
#include <boost/serialization/vector.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>

namespace Models
{
// DocString: FMTSesModel
/**
@brief Spatially explicit simulation model using a cellular automaton to simulate actions on a raster stack following a harvest schedule.
@details The action ordering is important because the simulator places the first action of the list on the map first.
*/
class FMTEXPORT FMTSesModel final: public FMTSeModel
    {
	// DocString: FMTSesModel::Serialize
	/**
	@brief Serialize the FMTSesModel through its base FMTSeModel for multiprocessing across multiple cpus (pickle in Python).
	@tparam Archive the archive type.
	@param[in,out] ar the archive to serialize to or from.
	@param[in] version the serialization version.
	*/
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
		{
		ar & boost::serialization::make_nvp("semodel", boost::serialization::base_object<FMTSeModel>(*this));
		}
	// DocString: FMTSesModel::swapPtr
	/**
	@brief Swap this model for the element at the end of the unique pointer.
	@param[in,out] rhs the unique pointer to swap with.
	*/
	virtual void swapPtr(std::unique_ptr<FMTModel>& rhs);
	// DocString: FMTSesModel()
	/**
	@brief Construct a FMTSesModel from a FMTSeModel, for preSolve use.
	@param[in] rhs the FMTSeModel.
	*/
	FMTSesModel(const FMTSeModel& rhs);
    public:
		// DocString: FMTSesModel()
		/**
		@brief Default constructor for FMTSesModel.
		*/
		FMTSesModel() = default;
		// DocString: ~FMTSesModel()
		/**
		@brief Default destructor for FMTSesModel.
		*/
		~FMTSesModel() = default;
		// DocString: FMTSesModel(const FMTSesModel)
		/**
		@brief Copy constructor for FMTSesModel.
		@param[in] rhs the FMTSesModel to copy.
		*/
        FMTSesModel(const FMTSesModel& rhs)=default;
		// DocString: FMTSesModel(const FMTModel)
		/**
		@brief Construct a FMTSesModel from a FMTModel.
		@param[in] rhs the model.
		*/
        FMTSesModel(const FMTModel& rhs);
		// DocString: FMTSesModel(const FMTModel)
		/**
		@brief Construct a FMTSesModel from a FMTModel and a forest.
		@param[in] rhs the model.
		@param[in] forest the forest.
		*/
		FMTSesModel(const FMTModel& rhs, const Spatial::FMTForest& forest);
		// DocString: FMTSesModel::operator=
		/**
		@brief Copy assignment operator for FMTSesModel.
		@param[in] rhs the FMTSesModel to copy.
		@return a reference to this FMTSesModel.
		*/
        FMTSesModel& operator = (const FMTSesModel& rhs)=default;
		// DocString: FMTSesModel(FMTSesModel&&)
		/**
		@brief Default move constructor for FMTSesModel.
		@param[in,out] rhs the model to move from.
		*/
		FMTSesModel(FMTSesModel&& rhs)=default;
		// DocString: FMTSesModel::operator=(FMTSesModel&& rhs) 
		/**
		@brief Default move assignment for FMTSesModel.
		@param[in,out] rhs the model to move from.
		@return a reference to this FMTSesModel.
		*/
		FMTSesModel& operator =(FMTSesModel&& rhs) =default;
		// DocString: FMTSesModel::getDisturbanceStats
		/**
		@brief Return a string of patch statistics (area, perimeter) of the disturbances stack.
		@return the disturbance statistics.
		*/
		std::string getDisturbanceStats() const;
		// DocString: FMTSesModel::greedyReferenceBuild
		/**
		@brief Find the best spatialization for a schedule using random draws over several iterations.
		@param[in] p_schedule the schedule of actions.
		@param[in] p_randomIterations the number of random iterations.
		@param[in] p_seed the seed.
		@param[in] p_tolerance the tolerance.
		@return a map of statistics.
		*/
		std::map<std::string, double> greedyReferenceBuild(
			const Core::FMTSchedule& p_schedule,
			size_t p_randomIterations,
			int p_seed = 0,
			double p_tolerance = FMT_DBL_TOLERANCE);
		// DocString: FMTSesModel::clone
		/**
		@brief Get a clone of the FMTSesModel.
		@return a unique pointer to the cloned model.
		*/
		virtual std::unique_ptr<FMTModel>clone() const;
		// DocString: FMTSesModel::build
		/**
		@brief Build the model by calling greedyReferenceBuild over the number of periods set as LENGTH.
		@param[in] schedules the schedules.
		@return true if the build succeeded else false.
		*/
		virtual bool build(std::vector<Core::FMTSchedule> schedules=std::vector<Core::FMTSchedule>());
		// DocString: FMTSesModel::solve
		/**
		@brief Return true; there is no solve since this is a simulation.
		@return true.
		*/
		virtual bool solve()
		{
			return true;
		}
		// DocString: FMTSesModel::preSolve
		/**
		@brief Return a presolved copy of the model.
		@param[in] optionaldevelopments the optional developments.
		@return the presolved model.
		*/
		virtual std::unique_ptr<FMTModel>preSolve(std::vector<Core::FMTActualDevelopment> optionaldevelopments = std::vector<Core::FMTActualDevelopment>()) const;
		// DocString: FMTSesModel::getCopy
		/**
		@brief Return a copy of the model for the selected period.
		@param[in] period the period.
		@return the copied model.
		*/
		virtual std::unique_ptr<FMTModel> getCopy(int period = 0) const;
    };

}

BOOST_CLASS_EXPORT_KEY(Models::FMTSesModel)

#endif // FMTSESM_Hm_included
