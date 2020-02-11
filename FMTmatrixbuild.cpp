#ifdef FMTWITHOSI

#include "FMTmatrixbuild.h"

namespace Models

{

	void FMTmatrixbuild::synchronize(std::shared_ptr<OsiSolverInterface> solver)
		{
		if (colsbuild.numberColumns()>0)
			{
			solver->addCols(colsbuild);
			}
		if (rowsbuild.numberRows() > 0)
			{
			solver->addRows(rowsbuild);
			}
		}
	FMTmatrixbuild::FMTmatrixbuild() : lastcol(-1),lastrow(-1), colsbuild(1), rowsbuild(0)
		{

		}

	void FMTmatrixbuild::setlastrowindex(const int& index)
		{
		lastrow = index;
		}

	void FMTmatrixbuild::setlastcolindex(const int& index)
		{
		lastcol = index;
		}


}

#endif