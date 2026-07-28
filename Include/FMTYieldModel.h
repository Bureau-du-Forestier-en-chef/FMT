/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#include "FMTObject.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>


#ifndef FMTYIELDMODEL_Hm_included
#define FMTYIELDMODEL_Hm_included

namespace Graph
{
	class FMTPredictor;
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
	@brief Abstract class to be implemented as a machine learning yield model.
	*/
	class FMTYieldModel : public FMTObject
	{
	public:
		// DocString: FMTYieldModel::setModel
		/**
		@brief Set the model linked to the yield model.
		@param[in] p_modelPtr the pointer to the model.
		*/
		virtual void setModel(Models::FMTModel* p_modelPtr);
		// DocString: FMTYieldModel::~FMTYieldModel()
		/**
		@brief Destructor for FMTYieldModel.
		*/
		virtual ~FMTYieldModel();
		// DocString: FMTYieldModel::FMTYieldModel()
		/**
		@brief Constructor for FMTYieldModel.
		*/
		FMTYieldModel();
		// DocString: FMTYieldModel::FMTYieldModel()
		/**
		@brief Copy constructor for FMTYieldModel.
		@param[in] rhs the FMTYieldModel to copy.
		*/
		FMTYieldModel(const FMTYieldModel& rhs)=default;
		// DocString: FMTYieldModel::operator = (const FMTYieldModel& rhs)
		/**
		@brief Default copy assignment operator for FMTYieldModel.
		@param[in] rhs the FMTYieldModel to copy.
		@return a reference to this FMTYieldModel.
		*/
		FMTYieldModel& operator = (const FMTYieldModel& rhs)=default;
		// DocString: FMTYieldModel::getModelName()
		/**
		@brief Return the model name.
		@return the model name.
		*/
		const std::string& getModelName() const;
		// DocString: FMTYieldModelNn::getModelYields()
		/**
		@brief Return the model yield names.
		@return the model yield names.
		*/
		const std::vector<std::string>& getModelYields() const;
		// DocString: FMTYieldModel::Clone()
		/**
		@brief Clone the yield model.
		@return a unique pointer to the cloned yield model.
		*/
		virtual std::unique_ptr<FMTYieldModel>Clone() const = 0;
		// DocString: FMTYieldModel::predict
		/**
		@brief Predict the yield for a request.
		@param[in] request the yield request.
		@return the predicted yield values.
		*/
		virtual const std::vector<double>predict(const Core::FMTYieldRequest& request) const = 0;
		// DocString: FMTYieldModel::presolve
		/**
		@brief Return a presolved copy of the yield model, or the same model by default.
		@param[in] filter the mask filter.
		@param[in] newthemes the presolved themes.
		@return the presolved yield model.
		*/
		virtual std::unique_ptr<FMTYieldModel> presolve(const FMTMaskFilter& filter,
				const std::vector<FMTTheme>& newthemes) const;
		// DocString: FMTYieldModel::postSolve
		/**
		@brief Return a postsolved copy of the yield model, a clone by default.
		@param[in] filter the mask filter.
		@param[in] basethemes the original themes.
		@return the postsolved yield model.
		*/
		virtual std::unique_ptr<FMTYieldModel> postSolve(const FMTMaskFilter& filter,
			const std::vector<FMTTheme>& basethemes) const;
		// DocString: FMTYieldModel::std::string()
		/**
		@brief Return the string representation of the yield model.
		@return the string representation.
		*/
		virtual  operator std::string() const;
		// DocString: FMTYieldModel::getPeriodicValues
		/**
		@brief Try to turn the yield model into periodic constant values.
		@return a vector where the first dimension is the yield id and the second is the periodic values, empty if not possible.
		*/
		virtual std::vector<std::vector<double>>getPeriodicValues() const;
		// DocString: FMTYieldModel::clearRandomYieldsCache
		/**
		@brief Clear the cache of all random yield models.
		*/
		virtual void clearRandomYieldsCache();
	protected:
		std::string m_modelName;
		std::vector<std::string> m_modelYields = {};
		const std::string m_JSON_PROP_MODEL_NAME = "modelFileName";
		Models::FMTModel* m_modelPtr=nullptr;
	private:
		friend class boost::serialization::access;
		// DocString: FMTYieldModel::serialize
		/**
		@brief Serialize the FMTYieldModel through its base FMTObject for multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to serialize to or from.
		@param[in] version the serialization version.
		*/
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<FMTObject>(*this));
		}
	};
}

#endif