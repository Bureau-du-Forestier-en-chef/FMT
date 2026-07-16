/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#include "FMTobject.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>


#ifndef FMTYIELDMODEL_Hm_included
#define FMTYIELDMODEL_Hm_included

namespace Graph
{
	class FMTpredictor;
}

namespace Models
{
	class FMTModel;
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
	class FMTYieldRequest;
	class FMTTheme;
	class FMTMaskFilter;
	// DocString: FMTYieldModel
	/**
	FMTYieldModel is an abstract class to be implemented as a machine learning model.
	*/
	class FMTYieldModel : public FMTObject
	{
	public:
		// DocString: FMTYieldModel::setModel
		/**
		@brief set The model to the yielmodel.
		@param[in] p_modelPtr the pointer to the actual model. This can be cast to different type of model...
		*/
		virtual void setModel(Models::FMTModel* p_modelPtr);
		// DocString: FMTYieldModel::~FMTYieldModel()
		/**
		Destructor for FMTYieldModel.
		*/
		virtual ~FMTYieldModel();
		// DocString: FMTYieldModel::FMTYieldModel()
		/**
		Constructor for FMTYieldModel.
		*/
		FMTYieldModel();
		// DocString: FMTYieldModel::FMTYieldModel()
		/**
		Copy constructor for FMTYieldModel.
		*/
		FMTYieldModel(const FMTYieldModel& rhs)=default;
		// DocString: FMTYieldModel::operator = (const FMTYieldModel& rhs)
		/**
		Default equality operator.
		*/
		FMTYieldModel& operator = (const FMTYieldModel& rhs)=default;
		// DocString: FMTYieldModel::getModelName()
		/**
		Returns the model name.
		*/
		const std::string& getModelName() const;
		// DocString: FMTYieldModelNn::getModelYields()
		/**
		Return model yields' names.
		*/
		const std::vector<std::string>& getModelYields() const;
		// DocString: FMTYieldModel::Clone()
		/**
		Implements FMTYieldModel::Clone().
		*/
		virtual std::unique_ptr<FMTYieldModel>Clone() const = 0;
		// DocString: FMTYieldModel::predict
		/**
		predict the yield
		*/
		virtual const std::vector<double>predict(const Core::FMTYieldRequest& request) const = 0;
		// DocString: FMTYieldModel::presolve
		/**
		If the FMTYieldModel contains Core classes it also need to be presolved when presolved is called on the FMTModel.
		By default it will return the same FMTYieldModel.
		*/
		virtual std::unique_ptr<FMTYieldModel> presolve(const FMTMaskFilter& filter,
				const std::vector<FMTTheme>& newthemes) const;
		// DocString: FMTYieldModel::postSolve
		/**
		Postsolve the yieldmodel by default it will return a clone.
		*/
		virtual std::unique_ptr<FMTYieldModel> postSolve(const FMTMaskFilter& filter,
			const std::vector<FMTTheme>& basethemes) const;
		// DocString: FMTYieldModel::std::string()
		/**
		When it comes to write down in a string the yield model.
		*/
		virtual  operator std::string() const;
		// DocString: FMTYieldModel::getPeriodicValues
		/**
		Try to turn the FMTYieldModel into periodic constant values. if returns an non empty vector then
		each first dimension is the yield id and each second dimension are the periodic value calculated by the yield.
		*/
		virtual std::vector<std::vector<double>>getPeriodicValues() const;
		// DocString: FMTYieldModel::clearRandomYieldsCache
		/**
		@brief clear the cache of all random yield model.
		*/
		virtual void clearRandomYieldsCache();
	protected:
		std::string modelName;
		std::vector<std::string> modelYields = {};
		const std::string JSON_PROP_MODEL_NAME = "modelFileName";
		Models::FMTModel* m_modelPtr=nullptr;
	private:
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<FMTObject>(*this));
		}
	};
}

#endif