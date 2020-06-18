/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTNSSMODEL_H
#define FMTNSSMODEL_H

#include "FMTmodel.h"
#include <sstream>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/export.hpp>


namespace Models
{
	// DocString: FMTnssmodel
	/**
	FMTnssmodel stands for non spatial simulation model. This model is used to simulate
	stochastics actions during the local replanning phase. Before using any kind of spatialy explicit model
	sometime using a simple non spatial model can help to understand the basic idea of simulation.
	*/
	class FMTnssmodel : public FMTmodel
		{
		// DocString: FMTnssmodel::save
		/**
		Save function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void save(Archive& ar, const unsigned int version) const
		{
			ar & boost::serialization::make_nvp("model", boost::serialization::base_object<FMTmodel>(*this));
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
			ar & boost::serialization::make_nvp("model", boost::serialization::base_object<FMTmodel>(*this));
			std::string basegeneratorstring;
			ar & BOOST_SERIALIZATION_NVP(basegeneratorstring);
			std::stringstream(basegeneratorstring) >> generator;
			
		}
		BOOST_SERIALIZATION_SPLIT_MEMBER()
		// DocString: FMTnssmodel::generator
		///This simulation model need to have it's own random number generator
		std::default_random_engine generator;
		public:
			// DocString: FMTnssmodel()
			/**
			Default constructor of FMTnssmodel.
			*/
			FMTnssmodel()=default;
			// DocString: ~FMTnssmodel()
			/**
			Default destructor of FMTnssmodel.
			*/
			~FMTnssmodel()=default;
			// DocString: FMTnssmodel(const FMTnssmodel&)
			/**
			Default copy constructor of FMTnssmodel.
			*/
			FMTnssmodel(const FMTnssmodel& rhs) = default;
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
			This functions simulate the actions vector using the actionsproportions vector to fix the area to simulate for
			each action like totalarea * actionsproportions[x] = area to simulate for action id x.
			If grow = true then all developement are grown to the next period in the area section.
			*/
			Core::FMTschedule simulate(const std::vector<double>& actionsproportions,bool grow=false);
		};
}

BOOST_CLASS_EXPORT_KEY(Models::FMTnssmodel)
#endif 

