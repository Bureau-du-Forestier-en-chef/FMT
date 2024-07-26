/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTSESM_H_INCLUDED
#define FMTSESM_H_INCLUDED

#include "FMTsemodel.h"
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
class FMTEXPORT FMTsesmodel final: public FMTsemodel
    {
	// DocString: FMTsesmodel::Serialize
	/**
	Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
		{
		ar & boost::serialization::make_nvp("semodel", boost::serialization::base_object<FMTsemodel>(*this));
		}
	virtual void swap_ptr(std::unique_ptr<FMTmodel>& rhs);
	// DocString: FMTsesmodel()
	/**
	Constructor for presolve use
	*/
	FMTsesmodel(const FMTsemodel& rhs);
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
		// DocString: FMTsesmodel(const FMTmodel)
		/**
		Parent constructor for FMTsesmodel (easiest way to get information from a FMTmodel)
		*/
		FMTsesmodel(const FMTmodel& rhs, const Spatial::FMTforest& forest);
		// DocString: FMTsesmodel::operator=
		/**
		Copy assignment of FMTsesmodel
		*/
        FMTsesmodel& operator = (const FMTsesmodel& rhs);
		// DocString: FMTsesmodel(FMTsesmodel&&)
		/**
		Default move constructor for FMTsesmodel.
		*/
		FMTsesmodel(FMTsesmodel&& rhs)=default;
		// DocString: FMTsesmodel::operator=(FMTsesmodel&& rhs) 
		/**
		Default move assignment for FMTsesmodel.
		*/
		FMTsesmodel& operator =(FMTsesmodel&& rhs) =default;
		// DocString: FMTsesmodel::getdisturbancestats
		/**
		Getter returning a string of patch stats (area,perimeter ....) that are ine the disturbances stack.
		*/
		std::string getdisturbancestats() const;
		// DocString: FMTsesmodel::greedyreferencebuild
		/**
		This function call multiple time the function FMTspatialschedule::referencebuild function to 
		find the best possible spatialisation for a given schedule using random draw. It uses a schedule of actions (schedule) on the actual
		spatialy explicit forest.  The user can also set the seed to get different solutions from the simulator.
		*/
		std::map<std::string, double> greedyreferencebuild(const Core::FMTschedule& schedule,
			const size_t& randomiterations,
			unsigned int seed = 0,
			double tolerance = FMT_DBL_TOLERANCE);
		// DocString: FMTsesmodel::clone
		/**
		Get a clone of the FMTsesmodel
		*/
		virtual std::unique_ptr<FMTmodel>clone() const;
		// DocString: FMTsesmodel::build
		/**
		This function will use the function greedyreferencebuild over the number of period set as LENGTH in model parameters.
		*/
		virtual bool build(std::vector<Core::FMTschedule> schedules=std::vector<Core::FMTschedule>());
		// DocString: FMTsesmodel::solve
		/**
		There is no solve since it's only a simulation. The build phase simulate over the LENGTH given in model parameters. 
		*/
		virtual bool solve()
		{
			return true;
		}
		// DocString: FMTsesmodel::presolve
		/**
		This function use a vector of developments and the actual transitions of the model and return new unique pointer to presolved FMTmodel.
		The function can reduce the number of global themes/actions/transitions/yields/lifespans/outputs/constraints data if the model is badly formulated.
		*/
		virtual std::unique_ptr<FMTmodel>presolve(std::vector<Core::FMTactualdevelopment> optionaldevelopments = std::vector<Core::FMTactualdevelopment>()) const;
		// DocString: FMTsesmodel::getcopy
		/**
		This function returns a copy of the FMTmodel of the selected period.
		If period = 0 it returns the FMTmodel::getcopy if period > 0 then it returns
		a copy of the FMTmodel based on the developements of the FMTgraph of the FMTlpmodel.
		Need to have a builded graph with a solution to use this function.
		*/
		virtual std::unique_ptr<FMTmodel> getcopy(int period = 0) const;
    };

}

BOOST_CLASS_EXPORT_KEY(Models::FMTsesmodel)

#endif // FMTSESM_H_INCLUDED
