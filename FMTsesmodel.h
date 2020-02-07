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

#ifndef FMTSESM_H_INCLUDED
#define FMTSESM_H_INCLUDED

#include "FMTmodel.h"
#include "FMTevent.h"
#include "FMTcut.h"
#include "FMTspatialaction.h"
#include "FMTlayer.h"
#include "FMTforest.h"
#include "FMTdisturbancestack.h"
#include <vector>
#include <map>

namespace Models
{
	/**
	This model is a spatialy explicit simulation (ses) model.
	It uses simple cellular automaton to spatialy simulate FMTactions on 
	a raster stack for a given planning horizon following an harvest schedule.
	The FMTaction ordering is realy important because the simulator will
	attend to place the first action of the list on the map and so on.
	*/
class FMTsesmodel : public FMTmodel
    {
	/**
	Save and load functions are for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
		{
		ar & boost::serialization::make_nvp("model", boost::serialization::base_object<FMTmodel>(*this));
		ar & BOOST_SERIALIZATION_NVP(mapping);
		ar & BOOST_SERIALIZATION_NVP(disturbances);
		ar & BOOST_SERIALIZATION_NVP(spactions);
		}
    protected:
		///Map of the FMTdevelopement spatialy located each raster cell has is own FMTdevelopement. Each call to simulate() will change those stades
        Spatial::FMTforest mapping;
		///Real schedules operated by the simulator on the map, some time the potential schedule will differ from the operated.
		std::vector<Core::FMTschedule> operatedschedule;
		///Disturbance stack which is the location of each cutblocks for the whole planning horizon.
        Spatial::FMTdisturbancestack disturbances;
		///Spatialy explicit actions (actions with green up, adjacency distance etc...).
		std::vector<Spatial::FMTspatialaction>spactions;
    public:
		/**
		Default constructor of FMTsesmodel
		*/
        FMTsesmodel();
		/**
		Copy constructor of FMTsesmodel
		*/
        FMTsesmodel(const FMTsesmodel& rhs);
		/**
		Parent constructor for FMTsesmodel (easiest way to get information from a FMTmodel)
		*/
        FMTsesmodel(const FMTmodel& rhs);
		/**
		Copy assignment of FMTsesmodel
		*/
        FMTsesmodel& operator = (const FMTsesmodel& rhs);
		/**
		Getter returning a copy the actual spatial forest stades of each FMTdevelopement (map).
		*/
        Spatial::FMTforest getmapping() const;
		/**
		Getter returning a copy of the spatially explicit disturbances stack (realy usefull with GCBM).
		*/
        Spatial::FMTdisturbancestack getdisturbances() const;
		/**
		Getter returning a string of patch stats (area,perimeter ....) that are ine the disturbances stack.
		*/
		std::string getdisturbancestats() const;
		/**
		Getter returning a copy of the operated schedules of the FMTsesmodel.
		The operated schedule can differ from the potential schedule provided by the user in the function
		simulate(). Which we call spatialisation impact.
		*/
		std::vector<Core::FMTschedule> getschedule() const;
		/**
		Setter of the initial forest stades (spatial map of FMTdevelopment)
		Has to be set before simulation() is called.
		*/
        bool setinitialmapping(const Spatial::FMTforest& forest);
		/**
		Setter of the spatialy explicit actions (green-up,adjacency etc...)
		Has to be set before simulation() is called.
		*/
        bool setspactions(const std::vector<Spatial::FMTspatialaction>& lspactions);
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
    };

}

#endif // FMTSESM_H_INCLUDED
