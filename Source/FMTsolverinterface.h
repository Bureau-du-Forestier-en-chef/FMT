/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTsolverinterface_H
#define FMTsolverinterface_H

#ifdef FMTWITHOSI


namespace Models
{ 
	enum class FMTsolverinterface
	{
		CLP = 1,
		//#ifdef  FMTWITHMOSEK
			MOSEK = 2,
		//#endif
		CPLEX = 3,
		GUROBI = 4,
	};

}

#endif
#endif 