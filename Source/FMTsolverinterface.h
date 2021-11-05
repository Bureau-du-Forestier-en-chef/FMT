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
    
    //Docstring: FMTsolverinterface
    /**
    *Enumerator used to specify the solver to use when solving the FMTlpmodel
    */
	enum class FMTsolverinterface
	{
		CLP = 1,/**< Coin-or linear programming solver : https://github.com/coin-or/Clp */
		//#ifdef  FMTWITHMOSEK
			MOSEK = 2,/**< MOSEK optimization software : https://www.mosek.com */
		//#endif
		CPLEX = 3,/**< IBM CPLEX Optimizer : https://www.ibm.com/analytics/cplex-optimizer */
		GUROBI = 4,/**< Gurobi optimizer : https://www.gurobi.com/ */
	};

}

#endif
#endif 