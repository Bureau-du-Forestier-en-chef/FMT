
#include "FMTyieldmodel.h"
#include <vector>
#include <string>
#include <memory>
#include <boost/thread/recursive_mutex.hpp>

#ifndef FMTYIELDMODELTSLA_Hm_included
#define FMTYIELDMODELTSLA_Hm_included

namespace Core
{
	class FMTtheme;
	// DocString: FMTyieldmodelTSLA
	/**
	FMTyieldmodelTSLA to get the time since last disturbance. This model is never written but always added by default by the yieldparser.
	*/
	class FMTyieldmodelTSLA : public FMTyieldmodel
	{
	
	public:
		// DocString: FMTyieldmodelTSLA::FMTyieldmodelTSLA()
		/**
		Default constructor.
		*/
		FMTyieldmodelTSLA();
		// DocString: FMTyieldmodelTSLA::FMTyieldmodelTSLA(const FMTyieldmodelTSLA& rhs)
		/**
		Copy constructor.
		*/
		FMTyieldmodelTSLA(const FMTyieldmodelTSLA& rhs)=default;
		// DocString: FMTyieldmodelTSLA::operator = (const FMTyieldmodelTSLA& rhs)
		/**
		Default equality operator.
		*/
		FMTyieldmodelTSLA& operator = (const FMTyieldmodelTSLA& rhs)=default;
		// DocString: FMTyieldmodelTSLA::~FMTyieldmodelTSLA()
		/**
		Default destructor.
		*/
		~FMTyieldmodelTSLA()=default;
		// DocString: FMTyieldmodelTSLA::Clone()
		/**
		Implements FMTyieldmodel::Clone().
		*/
		std::unique_ptr<FMTyieldmodel>Clone() const;
		// DocString: FMTyieldmodelTSLA::predict
		/**
		Will give the value of time since last action.
		*/
		const std::vector<double>predict(const Core::FMTyieldrequest& request) const;
		// DocString: FMTyieldmodelTSLA::getModelType()
		/**
		Return the modeltype of the FMTyieldmodel.
		*/
		static std::string getModelType();
		// DocString: FMTyieldmodelTSLA::presolve
		/**
		Return a presolved FMTyieldmodelTSLA
		*/
		virtual std::unique_ptr<FMTyieldmodel> presolve(const FMTmaskfilter& filter,
			const std::vector<FMTtheme>& newthemes) const;
		// DocString: FMTyieldmodelTSLA::postSolve
		/**
		Change the output and constraints mask to return to the original constraints and outputs.
		*/
		virtual std::unique_ptr<FMTyieldmodel> postSolve(const FMTmaskfilter& filter,
			const std::vector<FMTtheme>& basethemes) const;
		// DocString: FMTyieldmodelTSLAl::std::string()
		/**
		When it comes to write down in a string the yield model.
		*/
		virtual  operator std::string() const;
	};
}

#endif
