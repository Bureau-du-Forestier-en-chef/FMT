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
	class FMTtheme;
	class FMTmaskfilter;
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
		const std::string JSON_PROP_MODEL_NAME = "modelFileName";
	public:
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
		// DocString: FMTyieldmodel::operator = (const FMTyieldmodel& rhs)
		/**
		Default equality operator.
		*/
		FMTyieldmodel& operator = (const FMTyieldmodel& rhs)=default;
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
		// DocString: FMTyieldmodel::presolve
		/**
		If the FMTyieldmodel contains Core classes it also need to be presolved when presolved is called on the FMTmodel.
		By default it will return the same FMTyieldmodel.
		*/
		virtual std::unique_ptr<FMTyieldmodel> presolve(const FMTmaskfilter& filter,
				const std::vector<FMTtheme>& newthemes) const;
		// DocString: FMTyieldmodel::postsolve
		/**
		Postsolve the yieldmodel by default it will return a clone.
		*/
		virtual std::unique_ptr<FMTyieldmodel> postsolve(const FMTmaskfilter& filter,
			const std::vector<FMTtheme>& basethemes) const;
		// DocString: FMTyieldmodel::std::string()
		/**
		When it comes to write down in a string the yield model.
		*/
		virtual  operator std::string() const=0;
		// DocString: FMTyieldmodel::getperiodicvalues
		/**
		Try to turn the FMTyieldmodel into periodic constant values. if returns an non empty vector then
		each first dimension is the yield id and each second dimension are the periodic value calculated by the yield.
		*/
		virtual std::vector<std::vector<double>>getperiodicvalues() const;
	};
}

#endif