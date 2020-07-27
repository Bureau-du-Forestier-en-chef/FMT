/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTSESM_H_INCLUDED
#define FMTSESM_H_INCLUDED

#include "FMTmodel.h"
#include "FMTevent.h"
#include "FMTcut.h"
#include "FMTspatialaction.h"
#include "FMTlayer.h"
#include "FMTforest.h"
#include "FMTdisturbancestack.h"
#include "FMTschedule.h"
#include "FMTspatialschedule.h"
#include <vector>
#include <memory>
#include <map>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>

#include <boost/serialization/export.hpp>

namespace Models
{
// DocString: FMTsesmodel
/**
This model is a spatialy explicit simulation (ses) model.
It uses simple cellular automaton to spatialy simulate FMTactions on
a raster stack for a given planning horizon following an harvest schedule.
The FMTaction ordering is realy important because the simulator will
attend to place the first action of the list on the map and so on.
*/
class FMTsesmodel : public FMTmodel
    {
	// DocString: FMTsesmodel::Serialize
	/**
	Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
		{
		ar & boost::serialization::make_nvp("model", boost::serialization::base_object<FMTmodel>(*this));
		ar & BOOST_SERIALIZATION_NVP(mapping);
		ar & BOOST_SERIALIZATION_NVP(operatedschedule);
		ar & BOOST_SERIALIZATION_NVP(disturbances);
		ar & BOOST_SERIALIZATION_NVP(spactions);
		}
    protected:
		// DocString: FMTsesmodel::mapping
		///Map of the FMTdevelopement spatialy located each raster cell has is own FMTdevelopement. Each call to simulate() will change those stades
        Spatial::FMTforest mapping;
		// DocString: FMTsesmodel::operatedschedule
		///Real schedules operated by the simulator on the map, some time the potential schedule will differ from the operated.
		std::vector<Core::FMTschedule> operatedschedule;
		// DocString: FMTsesmodel::disturbances
		///Disturbance stack which is the location of each cutblocks for the whole planning horizon.
        Spatial::FMTdisturbancestack disturbances;
		// DocString: FMTsesmodel::spactions
		///Spatialy explicit actions (actions with green up, adjacency distance etc...).
		std::vector<Spatial::FMTspatialaction>spactions;
    public:
		// DocString: FMTsesmodel()
		/**
		Default constructor of FMTsesmodel
		*/
        FMTsesmodel();
		// DocString: ~FMTsesmodel()
		/**
		Default destructor of FMTsesmodel
		*/
		~FMTsesmodel() = default;
		// DocString: FMTsesmodel(const FMTsesmodel)
		/**
		Copy constructor of FMTsesmodel
		*/
        FMTsesmodel(const FMTsesmodel& rhs);
		// DocString: FMTsesmodel(const FMTmodel)
		/**
		Parent constructor for FMTsesmodel (easiest way to get information from a FMTmodel)
		*/
        FMTsesmodel(const FMTmodel& rhs);
		// DocString: FMTsesmodel::operator=
		/**
		Copy assignment of FMTsesmodel
		*/
        FMTsesmodel& operator = (const FMTsesmodel& rhs);
		// DocString: FMTsesmodel::getmapping
		/**
		Getter returning a copy the actual spatial forest stades of each FMTdevelopement (map).
		*/
		inline Spatial::FMTforest getmapping() const
			{
			return mapping;
			}
		// DocString: FMTsesmodel::getdisturbances
		/**
		Getter returning a copy of the spatially explicit disturbances stack (realy usefull with GCBM).
		*/
        inline Spatial::FMTdisturbancestack getdisturbances() const
			{
			return disturbances;
			}
		// DocString: FMTsesmodel::getdisturbancestats
		/**
		Getter returning a string of patch stats (area,perimeter ....) that are ine the disturbances stack.
		*/
		std::string getdisturbancestats() const;
		// DocString: FMTsesmodel::getschedule
		/**
		Getter returning a copy of the operated schedules of the FMTsesmodel.
		The operated schedule can differ from the potential schedule provided by the user in the function
		simulate(). Which we call spatialisation impact.
		*/
		inline std::vector<Core::FMTschedule> getschedule() const
			{
			return operatedschedule;
			}
		// DocString: FMTsesmodel::setinitialmapping
		/**
		Setter of the initial forest stades (spatial map of FMTdevelopment)
		Has to be set before simulation() is called.
		*/
        bool setinitialmapping(const Spatial::FMTforest& forest);
		// DocString: FMTsesmodel::setspactions
		/**
		Setter of the spatialy explicit actions (green-up,adjacency etc...)
		Has to be set before simulation() is called.
		*/
        bool setspactions(const std::vector<Spatial::FMTspatialaction>& lspactions);
		// DocString: FMTsesmodel::presolve
		/**
		Presolve the ses model to get a more simple model call original presolve() and presolve the
		FMTforest map and the spatial acitons.
		*/
		std::unique_ptr<FMTmodel>presolve(int presolvepass = 10,
			std::vector<Core::FMTactualdevelopment> optionaldevelopments = std::vector<Core::FMTactualdevelopment>()) const final;
		// DocString: FMTsesmodel::postsolve
		/**
		Using the original FMTmodel it postsolve the actual ses model to turn it back into a complete model with all themes,
		actions and outputs of the original not presolved model.
		*/
		std::unique_ptr<FMTmodel>postsolve(const FMTmodel& originalbasemodel) const final;
		// DocString: FMTsesmodel::simulate
		/**
		This is the main function to simulate a schedule of actions (schedule) on the actual
		spatialy explicit forest. If the (schedule_only) switch is turned on the simulator wont try
		to find some operable developements (not present in the potential schedule)
		even if the area harvested target for that action is not reach. The user can also set the seed
		to get different solutions from the simulator.
		*/
		std::map<std::string,double> simulate(const Core::FMTschedule& schedule,
                        bool schedule_only = true,
                        unsigned int seed = 0);
        // DocString: FMTsesmodel::simulate
		/**
		This is the main function to simulate a schedule of actions (schedule) on the actual
		spatialy explicit forest. If the (schedule_only) switch is turned on the simulator wont try
		to find some operable developements (not present in the potential schedule)
		even if the area harvested target for that action is not reach. The user can also set the seed
		to get different solutions from the simulator.
		***************** Function only for testing the new architecture ***************************
		*/
		std::map<std::string,double> newsimulate(const Core::FMTschedule& schedule,
                        Spatial::FMTspatialschedule& spschedule,
                        bool schedule_only = true,
                        unsigned int seed = 0);
		// DocString: FMTsesmodel::montecarlosimulate
		/**
		This function call multiple time the simulate function to find the best possible spatialisation for
		a given schedule using random draw. It uses a schedule of actions (schedule) on the actual
		spatialy explicit forest. If the (schedule_only) switch is turned on the simulator wont try
		to find some operable developements (not present in the potential schedule)
		even if the area harvested target for that action is not reach. The user can also set the seed
		to get different solutions from the simulator.
		*/
		std::map<std::string, double> montecarlosimulate(const Core::FMTschedule& schedule,
										const size_t& randomiterations,
										bool schedule_only = true,
										unsigned int seed = 0,
										double tolerance = FMT_DBL_TOLERANCE);

    };

}

BOOST_CLASS_EXPORT_KEY(Models::FMTsesmodel)

#endif // FMTSESM_H_INCLUDED
