

#ifndef FMTmatrixbuild_H_INCLUDED
#define FMTmatrixbuild_H_INCLUDED

#include "CoinBuild.hpp"
#include "OsiSolverInterface.hpp"
#include <memory>

namespace Models

{ 
	class FMTmatrixbuild
	{
		int lastcol, lastrow;
		CoinBuild colsbuild, rowsbuild;
	public:
		void synchronize(std::shared_ptr<OsiSolverInterface> solver);
		FMTmatrixbuild();
		FMTmatrixbuild(const FMTmatrixbuild& rhs) = default;
		FMTmatrixbuild& operator = (const FMTmatrixbuild& rhs) = default;
		~FMTmatrixbuild()=default;
		inline void addCol(int numberInColumn, const int * rows, const double * elements, double columnLower = 0.0,
			double columnUpper = COIN_DBL_MAX, double objectiveValue = 0.0)
			{
			colsbuild.addCol(numberInColumn, rows, elements, columnLower,
				columnUpper, objectiveValue);
			}
		inline void addRow(int numberInRow, const int * columns,const double * elements, double rowLower = -COIN_DBL_MAX,double rowUpper = COIN_DBL_MAX)
			{
			rowsbuild.addRow(numberInRow, columns, elements, rowLower, rowUpper);
			}
		inline int getlastrowindex() const
			{
			return lastrow;
			}
		inline int getlastcolindex() const
			{
			return lastcol;
			}
		void setlastrowindex(const int& index);
		void setlastcolindex(const int& index);
	};
}

#endif