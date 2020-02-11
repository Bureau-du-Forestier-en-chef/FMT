#ifndef FMTsolverinterface_H
#define FMTsolverinterface_H

#ifdef FMTWITHOSI

namespace Models
{ 
	enum class FMTsolverinterface
	{
		CLP = 1,
		#ifdef  FMTWITHMOSEK
			MOSEK = 2,
		#endif
		CPLEX = 3,
		GUROBI = 4,
	};

}

#endif
#endif 