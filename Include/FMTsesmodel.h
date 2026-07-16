/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTSESM_Hm_included
#define FMTSESM_Hm_included

#include "FMTsemodel.h"
#include <boost/serialization/vector.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>

namespace Models
{
// DocString: FMTSesModel
/**
This model is a spatially explicit simulation (ses) model.
It uses simple cellular automaton to spatially simulate FMTactions on
a raster stack for a given planning horizon following an harvest schedule.
The FMTAction ordering is realy important because the simulator will
attend to place the first action of the list on the map and so on.
*/
class FMTEXPORT FMTSesModel final: public FMTSeModel
    {
	// DocString: FMTSesModel::Serialize
	/**
	Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
		{
		ar & boost::serialization::make_nvp("semodel", boost::serialization::base_object<FMTSeModel>(*this));
		}
	virtual void swapPtr(std::unique_ptr<FMTModel>& rhs);
	// DocString: FMTSesModel()
	/**
	Constructor for presolve use
	*/
	FMTSesModel(const FMTSeModel& rhs);
    public:
		// DocString: FMTSesModel()
		/**
		Default constructor of FMTSesModel
		*/
		FMTSesModel() = default;
		// DocString: ~FMTSesModel()
		/**
		Default destructor of FMTSesModel
		*/
		~FMTSesModel() = default;
		// DocString: FMTSesModel(const FMTSesModel)
		/**
		Copy constructor of FMTSesModel
		*/
        FMTSesModel(const FMTSesModel& rhs)=default;
		// DocString: FMTSesModel(const FMTModel)
		/**
		Parent constructor for FMTSesModel (easiest way to get information from a FMTModel)
		*/
        FMTSesModel(const FMTModel& rhs);
		// DocString: FMTSesModel(const FMTModel)
		/**
		Parent constructor for FMTSesModel (easiest way to get information from a FMTModel)
		*/
		FMTSesModel(const FMTModel& rhs, const Spatial::FMTforest& forest);
		// DocString: FMTSesModel::operator=
		/**
		Copy assignment of FMTSesModel
		*/
        FMTSesModel& operator = (const FMTSesModel& rhs)=default;
		// DocString: FMTSesModel(FMTSesModel&&)
		/**
		Default move constructor for FMTSesModel.
		*/
		FMTSesModel(FMTSesModel&& rhs)=default;
		// DocString: FMTSesModel::operator=(FMTSesModel&& rhs) 
		/**
		Default move assignment for FMTSesModel.
		*/
		FMTSesModel& operator =(FMTSesModel&& rhs) =default;
		// DocString: FMTSesModel::getDisturbanceStats
		/**
		Getter returning a string of patch stats (area,perimeter ....) that are ine the disturbances stack.
		*/
		std::string getDisturbanceStats() const;
		// DocString: FMTSesModel::greedyReferenceBuild
		/**
		This function call multiple time the function FMTspatialschedule::referenceBuild function to 
		find the best possible spatialisation for a given schedule using random draw. It uses a schedule of actions (schedule) on the actual
		spatially explicit forest.  The user can also set the seed to get different solutions from the simulator.
		*/
		std::map<std::string, double> greedyReferenceBuild(
			const Core::FMTSchedule& p_schedule,
			size_t p_randomIterations,
			int p_seed = 0,
			double p_tolerance = FMT_DBL_TOLERANCE);
		// DocString: FMTSesModel::clone
		/**
		Get a clone of the FMTSesModel
		*/
		virtual std::unique_ptr<FMTModel>clone() const;
		// DocString: FMTSesModel::build
		/**
		This function will use the function greedyReferenceBuild over the number of period set as LENGTH in model parameters.
		*/
		virtual bool build(std::vector<Core::FMTSchedule> schedules=std::vector<Core::FMTSchedule>());
		// DocString: FMTSesModel::solve
		/**
		There is no solve since it's only a simulation. The build phase simulate over the LENGTH given in model parameters. 
		*/
		virtual bool solve()
		{
			return true;
		}
		// DocString: FMTSesModel::presolve
		/**
		This function use a vector of developments and the actual transitions of the model and return new unique pointer to presolved FMTModel.
		The function can reduce the number of global themes/actions/transitions/yields/lifespans/outputs/constraints data if the model is badly formulated.
		*/
		virtual std::unique_ptr<FMTModel>presolve(std::vector<Core::FMTActualDevelopment> optionaldevelopments = std::vector<Core::FMTActualDevelopment>()) const;
		// DocString: FMTSesModel::getCopy
		/**
		This function returns a copy of the FMTModel of the selected period.
		If period = 0 it returns the FMTModel::getCopy if period > 0 then it returns
		a copy of the FMTModel based on the developments of the FMTgraph of the FMTLpModel.
		Need to have a builded graph with a solution to use this function.
		*/
		virtual std::unique_ptr<FMTModel> getCopy(int period = 0) const;
    };

}

BOOST_CLASS_EXPORT_KEY(Models::FMTSesModel)

#endif // FMTSESM_Hm_included
