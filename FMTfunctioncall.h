/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTfunctioncall_H_INCLUDED
#define FMTfunctioncall_H_INCLUDED

#include <string>
#include <math.h>


namespace Core
{
	enum class FMTfkey
		{
		expo = 1,
		ln = 2,
		notvalid = 3
		};

	class FMTfunctioncall
	{
		FMTfkey key;
	public:
		FMTfunctioncall();
		FMTfunctioncall(const std::string& base);
		double call(const double& rhs) const;
		FMTfunctioncall(const FMTfunctioncall& rhs);
		FMTfunctioncall& operator = (const FMTfunctioncall& rhs);
		bool valid() const;
		~FMTfunctioncall() = default;
	};
}

#endif