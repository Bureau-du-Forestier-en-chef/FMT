/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTNSSMODEL_H
#define FMTNSSMODEL_H

#ifdef FMTWITHOSI

#include "FMTsrmodel.h"
#include <sstream>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/export.hpp>
#include <random>


namespace Models
{
	// DocString: FMTnssmodel
	/**
	FMTnssmodel stands for non spatial simulation model. This model is used to simulate
	stochastics actions during the local replanning phase. Before using any kind of spatialy explicit model
	sometime using a simple non spatial model can help to understand the basic idea of simulation.
	*/
	class FMTEXPORT FMTnssmodel : public FMTsrmodel
		{
		// DocString: FMTnssmodel::save
		/**
		Save function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void save(Archive& ar, const unsigned int version) const
		{
			ar & boost::serialization::make_nvp("model", boost::serialization::base_object<FMTsrmodel>(*this));
			std::stringstream basegenerator;
			basegenerator << generator;
			const std::string basegeneratorstring(basegenerator.str());
			ar & BOOST_SERIALIZATION_NVP(basegeneratorstring);

		}
		// DocString: FMTnssmodel::load
		/**
		Load function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		template<class Archive>
		void load(Archive& ar, const unsigned int version)
		{
			ar & boost::serialization::make_nvp("model", boost::serialization::base_object<FMTsrmodel>(*this));
			std::string basegeneratorstring;
			ar & BOOST_SERIALIZATION_NVP(basegeneratorstring);
			std::stringstream(basegeneratorstring) >> generator;
			
		}
		BOOST_SERIALIZATION_SPLIT_MEMBER()
		// DocString: FMTnssmodel::generator
		///This simulation model need to have it's own random number generator
		std::default_random_engine generator;
		// DocString: FMTnssmodel::constraintstotarget
		/**
		Using the constraints generate random level or determinist level of output to generate
		*/
		std::vector<const Core::FMToutput*> constraintstotarget(std::vector<double>& targets);
		// DocString: FMTnssmodel::getoperabilities
		/**
		Get potential operabilities of an actualdevelopment
		*/
		std::vector<std::pair<size_t, const Core::FMTaction*>> getoperabilities(const Core::FMTactualdevelopment& development,
			std::vector<std::vector<const Core::FMTaction*>> targets,
			const std::vector<const Core::FMToutput*>& alloutputs) const;
		// DocString: FMTnssmodel::etactionstargets
		/**
		Get the potential actions of each output.
		*/
		std::vector<std::vector<const Core::FMTaction*>> getactionstargets(const std::vector<const Core::FMToutput*>& alloutputs) const;
		// DocString: FMTnssmodel::operate
		/**
		Operate and fill FMTschedule
		*/
		std::vector<Core::FMTdevelopmentpath> operate(const Core::FMTactualdevelopment& development,const double& areatarget,const Core::FMTaction* target,Core::FMTschedule& schedule) const;
		// DocString: FMTnssmodel::updatearea
		/**
		update the actual developments in the area vector.
		*/
		void updatearea(std::vector<Core::FMTactualdevelopment>& basearea, std::vector<Core::FMTactualdevelopment>::iterator developmentit,const std::vector<Core::FMTdevelopmentpath>& paths, const double& operatedarea);
		// DocString: FMTnssmodel::updateareatargets
		/**
		Update all the stuff related to the targeted area, knowing that an area has been operated on a given location.
		*/
		void updateareatargets(const double& areaoperated,const size_t& outtarget,
			std::vector<const Core::FMToutput*>& alloutputs,std::vector<double>& targets,
			std::vector<std::vector<const Core::FMTaction*>>& actiontargets) const;
		public:
			// DocString: FMTnssmodel()
			/**
			Default constructor of FMTnssmodel.
			*/
			FMTnssmodel();
			// DocString: ~FMTnssmodel()
			/**
			Default destructor of FMTnssmodel.
			*/
			~FMTnssmodel()=default;
			// DocString: FMTnssmodel(const FMTnssmodel&)
			/**
			Default copy constructor of FMTnssmodel.
			*/
			FMTnssmodel(const FMTnssmodel& rhs);
			// DocString: FMTnssmodel::operator=
			/**
			Default copy assignment of MTnssmodel.
			*/
			FMTnssmodel& operator=(const FMTnssmodel& rhs) = default;
			// DocString: FMTnssmodel(const FMTmodel&,unsigned int)
			/**
			Constructor for FMTnssmodel taking a FMTmodel and a seed to initialize the random number generator.
			*/
			FMTnssmodel(const FMTmodel& rhs, unsigned int seed);
			// DocString: FMTnssmodel::simulate
			/**
			This function do a non spatial simulation based on the area constraints in the optimize section.
			*/
			void simulate();
			// DocString: FMTnssmodel::clone
			/**
			Get a clone of the FMTnssmodel
			*/
			virtual std::unique_ptr<FMTmodel>clone() const final;
			// DocString: FMTmodel::doplanning
			/**
			Build the model and do the initialsolve or simulate.
			*/
			virtual bool doplanning(const std::vector<Core::FMTschedule>&schedules,
				bool forcepartialbuild = false, Core::FMTschedule objectiveweight = Core::FMTschedule());
			
		};
}

BOOST_CLASS_EXPORT_KEY(Models::FMTnssmodel)
#endif 
#endif

