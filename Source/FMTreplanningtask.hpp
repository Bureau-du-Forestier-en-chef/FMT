#ifndef FMTreplanningtask_H_INCLUDED
#define FMTreplanningtask_H_INCLUDED

#include "FMTtask.hpp"
#include "FMTactualdevelopment.hpp"
#include "FMTschedule.hpp"
#include <vector>
#include <queue>
#include <memory>
#include <string>


namespace Models
{
	class FMTmodel;
}

namespace Core
{
	class FMToutput;
}



/// Namespace for parallel tasking may include multithreading / multiprocessing
namespace Parallel
{
	class FMTparallelwriter;
	class FMTEXPORT FMTreplanningtask : public FMTtask
	{
		////////////////setbaseinfos()/////////////////
		///////////////////////////////////////////////
		std::shared_ptr<FMTparallelwriter>resultswriter;
		std::shared_ptr<Core::FMTschedule>baseschedule;
		///////////////////////////////////////////////
		//////////////Non build models////////////////
		////////////////////////////////////////
		///////////////////////////////////////
		std::shared_ptr<Models::FMTmodel>global;//base area is in here.
		std::shared_ptr<Models::FMTmodel>stochastic;
		std::shared_ptr<Models::FMTmodel>local;
		///////////////////////////////////////
		//////////////////////////////////////
		std::queue<size_t>replicateids;
		std::vector<Core::FMTactualdevelopment>dynamicarea;
		Core::FMTschedule iterationglobalschedule;
		std::vector<Core::FMTconstraint>localconstraints;
		int replanningperiods;
		double globalsolutionweight;
		std::unique_ptr<Models::FMTmodel>domodelplanning(
			const std::shared_ptr<Models::FMTmodel>model,
			const int& replanningperiod,
			bool getsolutionandlocal = false,
			bool applyscheduleweight = false);
		void writeresults(const std::unique_ptr<Models::FMTmodel>& modelptr,bool onlyfirstperiod = false);
		const size_t getiteration() const;
	protected:
		virtual void work();
	public:
		FMTreplanningtask() = default;
		FMTreplanningtask(const FMTreplanningtask& rhs) = default;
		FMTreplanningtask& operator = (const FMTreplanningtask& rhs) = default;
		std::unique_ptr<FMTtask>clone() const;
		//unique constructor!
		FMTreplanningtask(const Models::FMTmodel& globalm,
						const Models::FMTmodel& stochasticm,
						const Models::FMTmodel& localm,
						const std::vector<Core::FMToutput>& outputs,
						const std::string& outputlocation,
						const std::string& gdaldriver,
						const size_t& replicates,
						const int& replanningperiodssize,
						const double& globalwweight);
		virtual std::vector<std::unique_ptr<FMTtask>>split(const size_t& numberoftasks) const;

	};

}



#endif
