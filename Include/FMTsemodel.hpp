/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTSEM_H_INCLUDED
#define FMTSEM_H_INCLUDED

#include "FMTmodel.hpp"
#include "FMTspatialschedule.hpp"
#include <boost/serialization/vector.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>

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
	// DocString: FMTsemodel::Serialize
	/**
	Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
		{
		ar & boost::serialization::make_nvp("model", boost::serialization::base_object<FMTmodel>(*this));
		ar & BOOST_SERIALIZATION_NVP(solution);
		}
	virtual void swap_ptr(const std::unique_ptr<FMTmodel>& rhs);
    protected:
		// DocString: FMTsemodel::spschedule
		///Contains the builded spatialsolution latest or best one.
		Spatial::FMTspatialschedule solution;
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
		// DocString: FMTsemodel(FMTsemodel&&)
		/**
		Default move constructor for FMTsemodel.
		*/
		FMTsemodel(FMTsemodel&& rhs)=default;
		// DocString: FMTsemodel::operator=(FMTsemodel&& rhs) 
		/**
		Default move assignment for FMTsemodel.
		*/
		FMTsemodel& operator =(FMTsemodel&& rhs) =default;
		// DocString: FMTsemodel::getmapping
		/**
		Getter returning a copy the actual spatial forest stades of each FMTdevelopement (map).
		*/
		Spatial::FMTforest getmapping() const;
		// DocString: FMTsemodel::getspschedule
		/**
		Getter returning a copy of the spatially explicit solution.
		*/
		inline Spatial::FMTspatialschedule getspschedule() const
		{
			return solution;
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
        bool setinitialmapping(Spatial::FMTforest forest);
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
		// DocString: FMTsemodel::greedyreferencebuild
		/**
		This function call multiple time the function FMTspatialschedule::referencebuild function to 
		find the best possible spatialisation for a given schedule using random draw. It uses a schedule of actions (schedule) on the actual
		spatialy explicit forest.  The user can also set the seed to get different solutions from the simulator.
		*/
		std::map<std::string, double> greedyreferencebuild(const Core::FMTschedule& schedule,
			const size_t& randomiterations,
			unsigned int seed = 0,
			double tolerance = FMT_DBL_TOLERANCE);
		// DocString: FMTsemodel::getoutput
		/**
		Get the output value of a output for a given period using the spatial solution.
		the map key returned consist of output name
		if level == FMToutputlevel::standard || level == FMToutputlevel::totalonly,
		or developement name if level == FMToutputlevel::developpement
		*/
		virtual std::map<std::string, double> getoutput(const Core::FMToutput& output,
			int period, Core::FMToutputlevel level = Core::FMToutputlevel::standard) const;
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
		Get area with spatialschedule
		*/
		virtual std::vector<Core::FMTactualdevelopment>getarea(int period = 0, bool beforegrowanddeath = false) const;
		// DocString: FMTsemodel::getcopy
		/**
		This function returns a copy of the FMTmodel of the selected period.
		The function is going to clean the FMTconstraints and keep the objective.
		*/
		virtual FMTmodel getcopy(int period = 0) const;
    };

}

BOOST_CLASS_EXPORT_KEY(Models::FMTsemodel)

#endif // FMTSEM_H_INCLUDED