/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTfunctioncall_Hm_included
#define FMTfunctioncall_Hm_included

#include <string>



namespace Core
{
	// DocString: FMTfkey
	/**
	@brief Enumerator of the functions usable in an FMTFunctionCall (exponential, natural log).
	*/
	enum class FMTfkey
		{
		expo = 1,
		ln = 2,
		notvalid = 3
		};

	// DocString: FMTFunctionCall
	/**
	@brief Function call usable in yield or output expressions, such as exponential or natural log.
	*/
	class FMTFunctionCall
	{
		FMTfkey key;
	public:
		// DocString: FMTFunctionCall()
		/**
		@brief Default constructor for FMTFunctionCall.
		*/
		FMTFunctionCall();
		// DocString: FMTFunctionCall(const std::string&)
		/**
		@brief Construct a FMTFunctionCall from a string key.
		@param[in] base the string representation of the function.
		*/
		FMTFunctionCall(const std::string& base);
		// DocString: FMTFunctionCall::call
		/**
		@brief Apply the function to a value.
		@param[in] rhs the value to apply the function to.
		@return the result of the function.
		*/
		double call(const double& rhs) const;
		// DocString: FMTFunctionCall(const FMTFunctionCall&)
		/**
		@brief Copy constructor for FMTFunctionCall.
		@param[in] rhs the FMTFunctionCall to copy.
		*/
		FMTFunctionCall(const FMTFunctionCall& rhs);
		// DocString: FMTFunctionCall::operator=
		/**
		@brief Copy assignment operator for FMTFunctionCall.
		@param[in] rhs the FMTFunctionCall to copy.
		@return a reference to this FMTFunctionCall.
		*/
		FMTFunctionCall& operator = (const FMTFunctionCall& rhs);
		// DocString: FMTFunctionCall::valid
		/**
		@brief Return true if the function call is valid.
		@return true if the function is valid else false.
		*/
		bool valid() const;
		// DocString: ~FMTFunctionCall()
		/**
		@brief Default destructor for FMTFunctionCall.
		*/
		~FMTFunctionCall() = default;
	};
}

#endif