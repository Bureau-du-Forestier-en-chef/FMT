/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#include "FMTobject.hpp"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>


#ifndef FMTYIELDMODEL_H_INCLUDED
#define FMTYIELDMODEL_H_INCLUDED

namespace Graph
{
	class FMTpredictor;
}


namespace boost
{
	namespace property_tree
	{
		template < class Key, class Data, class KeyCompare >
		class basic_ptree;
		typedef basic_ptree< std::string, std::string, std::less<std::string> > ptree;
	}
}

namespace Core 
{
	class FMTyieldrequest;

	// DocString: FMTyieldmodel
	/**
	FMTyieldmodel is an abstract class to be implemented as a machine learning model.
	*/
	class FMTyieldmodel : public FMTobject
	{
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<FMTobject>(*this));
		}
	protected:
		std::string modelName;
		std::vector<std::string> modelYields = {};
	public:
		// DocString: FMTyieldmodel::Predict
		/**
		Runs the machine learning model to predict its outputs.
		*/
		const std::vector<double>Predict(const Core::FMTyieldrequest& request) const;
		// DocString: FMTyieldmodel::~FMTyieldmodel()
		/**
		Destructor for FMTyieldmodel.
		*/
		virtual ~FMTyieldmodel();
		// DocString: FMTyieldmodel::FMTyieldmodel()
		/**
		Constructor for FMTyieldmodel.
		*/
		FMTyieldmodel();
		// DocString: FMTyieldmodel::FMTyieldmodel()
		/**
		Copy constructor for FMTyieldmodel.
		*/
		FMTyieldmodel(const FMTyieldmodel& rhs)=default;
		// DocString: FMTyieldmodel::GetModelName()
		/**
		Returns the model name.
		*/
		const std::string& GetModelName() const;
		// DocString: FMTyieldmodelnn::GetModelYields()
		/**
		Return model yields' names.
		*/
		const std::vector<std::string>& GetModelYields() const;
		// DocString: FMTyieldmodel::Clone()
		/**
		Implements FMTyieldmodel::Clone().
		*/
		virtual std::unique_ptr<FMTyieldmodel>Clone() const = 0;
		// DocString: FMTyieldmodel::Predict
		/**
		Predict the yield
		*/
		virtual const std::vector<double>Predict(const Core::FMTyieldrequest& request) const = 0;
	};
}

#endif