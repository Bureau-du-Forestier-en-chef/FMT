
#include "FMTyieldmodel.h"
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
	FMTYieldModelTsla to get the time since last disturbance. This model is never written but always added by default by the yieldparser.
	*/
	class FMTYieldModelTsla : public FMTYieldModel
	{
	
	public:
		// DocString: FMTYieldModelTsla::FMTYieldModelTsla()
		/**
		Default constructor.
		*/
		FMTYieldModelTsla();
		// DocString: FMTYieldModelTsla::FMTYieldModelTsla(const FMTYieldModelTsla& rhs)
		/**
		Copy constructor.
		*/
		FMTYieldModelTsla(const FMTYieldModelTsla& rhs)=default;
		// DocString: FMTYieldModelTsla::operator = (const FMTYieldModelTsla& rhs)
		/**
		Default equality operator.
		*/
		FMTYieldModelTsla& operator = (const FMTYieldModelTsla& rhs)=default;
		// DocString: FMTYieldModelTsla::~FMTYieldModelTsla()
		/**
		Default destructor.
		*/
		~FMTYieldModelTsla()=default;
		// DocString: FMTYieldModelTsla::Clone()
		/**
		Implements FMTYieldModel::Clone().
		*/
		std::unique_ptr<FMTYieldModel>Clone() const;
		// DocString: FMTYieldModelTsla::predict
		/**
		Will give the value of time since last action.
		*/
		const std::vector<double>predict(const Core::FMTYieldRequest& request) const;
		// DocString: FMTYieldModelTsla::getModelType()
		/**
		Return the modeltype of the FMTYieldModel.
		*/
		static std::string getModelType();
		// DocString: FMTYieldModelTsla::presolve
		/**
		Return a presolved FMTYieldModelTsla
		*/
		virtual std::unique_ptr<FMTYieldModel> presolve(const FMTMaskFilter& filter,
			const std::vector<FMTTheme>& newthemes) const;
		// DocString: FMTYieldModelTsla::postSolve
		/**
		Change the output and constraints mask to return to the original constraints and outputs.
		*/
		virtual std::unique_ptr<FMTYieldModel> postSolve(const FMTMaskFilter& filter,
			const std::vector<FMTTheme>& basethemes) const;
		// DocString: FMTyieldmodelTSLAl::std::string()
		/**
		When it comes to write down in a string the yield model.
		*/
		virtual  operator std::string() const;
	};
}

#endif
