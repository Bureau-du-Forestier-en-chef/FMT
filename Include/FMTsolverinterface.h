/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTSOLVERINTERFACE
#define FMTSOLVERINTERFACE

#ifdef FMTWITHOSI

namespace Models
{ 
    
    // DocString: FMTSolverInterface
    /**
    @brief Enumerator used to specify the solver to use when solving the FMTLpModel.
    */
	enum class FMTSolverInterface
	{
		CLP = 1,/**< Coin-or linear programming solver : https://github.com/coin-or/Clp */
		MOSEK = 2,/**< MOSEK optimization software : https://www.mosek.com */
		GLPK = 3,
		CPLEX = 4,/**< IBM CPLEX Optimizer : https://www.ibm.com/analytics/cplex-optimizer */
		GUROBI = 5,/**< Gurobi optimizer : https://www.gurobi.com/ */
		COUNT = 6
	};

}

#endif
#endif 