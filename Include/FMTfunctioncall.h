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
	enum class FMTfkey
		{
		expo = 1,
		ln = 2,
		notvalid = 3
		};

	class FMTFunctionCall
	{
		FMTfkey key;
	public:
		FMTFunctionCall();
		FMTFunctionCall(const std::string& base);
		double call(const double& rhs) const;
		FMTFunctionCall(const FMTFunctionCall& rhs);
		FMTFunctionCall& operator = (const FMTFunctionCall& rhs);
		bool valid() const;
		~FMTFunctionCall() = default;
	};
}

#endif