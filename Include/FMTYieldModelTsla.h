
#include "FMTYieldModel.h"
#include <vector>
#include <string>
#include <memory>
#include <boost/thread/recursive_mutex.hpp>

#ifndef FMTYIELDMODELTSLA_Hm_included
#define FMTYIELDMODELTSLA_Hm_included

namespace Core
{
	class FMTTheme;
	// DocString: FMTYieldModelTsla
	/**
	@brief Yield model returning the time since last disturbance, always added by default by the yield parser.
	*/
	class FMTYieldModelTsla : public FMTYieldModel
	{
	
	public:
		// DocString: FMTYieldModelTsla::FMTYieldModelTsla()
		/**
		@brief Default constructor for FMTYieldModelTsla.
		*/
		FMTYieldModelTsla();
		// DocString: FMTYieldModelTsla::FMTYieldModelTsla(const FMTYieldModelTsla& rhs)
		/**
		@brief Copy constructor for FMTYieldModelTsla.
		@param[in] rhs the FMTYieldModelTsla to copy.
		*/
		FMTYieldModelTsla(const FMTYieldModelTsla& rhs)=default;
		// DocString: FMTYieldModelTsla::operator = (const FMTYieldModelTsla& rhs)
		/**
		@brief Copy assignment operator for FMTYieldModelTsla.
		@param[in] rhs the FMTYieldModelTsla to copy.
		@return a reference to this FMTYieldModelTsla.
		*/
		FMTYieldModelTsla& operator = (const FMTYieldModelTsla& rhs)=default;
		// DocString: FMTYieldModelTsla::~FMTYieldModelTsla()
		/**
		@brief Default destructor for FMTYieldModelTsla.
		*/
		~FMTYieldModelTsla()=default;
		// DocString: FMTYieldModelTsla::Clone()
		/**
		@brief Clone the yield model.
		@return a unique pointer to the cloned yield model.
		*/
		std::unique_ptr<FMTYieldModel>Clone() const;
		// DocString: FMTYieldModelTsla::predict
		/**
		@brief Predict the time since last action for a request.
		@param[in] request the yield request.
		@return the predicted values.
		*/
		const std::vector<double>predict(const Core::FMTYieldRequest& request) const;
		// DocString: FMTYieldModelTsla::getModelType()
		/**
		@brief Return the model type of the yield model.
		@return the model type.
		*/
		static std::string getModelType();
		// DocString: FMTYieldModelTsla::presolve
		/**
		@brief Return a presolved copy of the yield model.
		@param[in] filter the mask filter.
		@param[in] newthemes the presolved themes.
		@return the presolved yield model.
		*/
		virtual std::unique_ptr<FMTYieldModel> presolve(const FMTMaskFilter& filter,
			const std::vector<FMTTheme>& newthemes) const;
		// DocString: FMTYieldModelTsla::postSolve
		/**
		@brief Return a postsolved copy of the yield model, restoring the original output and constraint masks.
		@param[in] filter the mask filter.
		@param[in] basethemes the original themes.
		@return the postsolved yield model.
		*/
		virtual std::unique_ptr<FMTYieldModel> postSolve(const FMTMaskFilter& filter,
			const std::vector<FMTTheme>& basethemes) const;
		// DocString: FMTyieldmodelTSLAl::std::string()
		/**
		@brief Return the string representation of the yield model.
		@return the string representation.
		*/
		virtual  operator std::string() const;
	};
}

#endif
