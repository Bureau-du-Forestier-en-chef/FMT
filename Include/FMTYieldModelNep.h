#ifdef FMTWITHONNXR
#include "FMTYieldModelNn.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>

#ifndef FMTYIELDMODELNEP_Hm_included
#define FMTYIELDMODELNEP_Hm_included

namespace Core
{
	// DocString: FMTYieldModelPools
	/**
	@brief Machine learning yield model predicting carbon NEP.
	*/
	class FMTYieldModelNep : public FMTYieldModelNn
	{
	public:
		// DocString: FMTYieldModelNep::FMTYieldModelNep()
		/**
		@brief Default constructor for FMTYieldModelNep.
		*/
		FMTYieldModelNep() = default;
		// DocString: FMTYieldModelNep::FMTYieldModelNep(const FMTYieldModelNep& rhs)
		/**
		@brief Copy constructor for FMTYieldModelNep.
		@param[in] rhs the FMTYieldModelNep to copy.
		*/
		FMTYieldModelNep(const FMTYieldModelNep& rhs)=default;
		// DocString: FMTYieldModelNep::operator = (const FMTYieldModelNep& rhs)
		/**
		@brief Copy assignment operator for FMTYieldModelNep.
		@param[in] rhs the FMTYieldModelNep to copy.
		@return a reference to this FMTYieldModelNep.
		*/
		FMTYieldModelNep& operator = (const FMTYieldModelNep& rhs) = default;
		// DocString: FMTYieldModelNep::~FMTYieldModelNep()
		/**
		@brief Default destructor for FMTYieldModelNep.
		*/
		~FMTYieldModelNep() override=default;
		// DocString: FMTYieldModelNep::FMTYieldModelNep(const boost::property_tree::ptree& jsonProps, std::vector<std::string>& inputYields)
		/**
		@brief Construct a FMTYieldModelNep from a JSON tree and an input yield name list.
		@param[in] jsonProps the JSON properties.
		@param[in,out] inputYields the input yield names.
		*/
		FMTYieldModelNep(const boost::property_tree::ptree& jsonProps, std::vector<std::string>& inputYields);
		// DocString: FMTYieldModelNep::clone()
		/**
		@brief clone the yield model.
		@return a unique pointer to the cloned yield model.
		*/
		std::unique_ptr<FMTYieldModel>clone() const override;
		// DocString: FMTYieldModelNep::getModelOutputNames()
		/**
		@brief Return the input values for a predictor.
		@param[in] predictor the predictor.
		@return the input values.
		*/
		const std::vector<double> getInputValues(const Graph::FMTPredictor& predictor) const override;
		// DocString: FMTYieldModelNep::getModelType()
		/**
		@brief Return the model type of the yield model.
		@return the model type.
		*/
		static std::string getModelType();
	private:
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& boost::serialization::make_nvp("FMTYieldModelNn",
				boost::serialization::base_object<FMTYieldModelNn>(*this));
		}
	};
}
BOOST_CLASS_EXPORT_KEY(Core::FMTYieldModelNep)
#endif

#endif