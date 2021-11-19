#ifndef FMTreplanningtask_H_INCLUDED
#define FMTreplanningtask_H_INCLUDED
#include "FMTtask.hpp"
#include "FMTmodel.hpp"
#include "FMTmodelparser.hpp"
#include "FMToutput.hpp"
#include "FMTconstraint.hpp"
#include "FMTschedule.hpp"
#include "FMTactualdevelopment.hpp"
#include <vector>
#include <memory>

/// Namespace for parallel tasking may include multithreading / multiprocessing
namespace Parallel
{
	class FMTreplanningtask : public FMTtask
	{
		std::unique_ptr<Models::FMTmodel>global;
		std::unique_ptr<Models::FMTmodel>stochastic;
		std::unique_ptr<Models::FMTmodel>local;
		std::shared_ptr<Parser::FMTmodelparser>modelparser;
		//OGRlayer* results;
		std::vector<Core::FMToutput>targetoutputs;
		std::vector<Core::FMTactualdevelopment>dynamicarea;
		Core::FMTschedule iterationglobalschedule;
		std::vector<Core::FMTconstraint>localconstraints;
		int replanningperiodssize;
		size_t iteration;
	public:
		FMTreplanningtask() = default;
		FMTreplanningtask(const FMTreplanningtask& rhs) = default;
		FMTreplanningtask& operator = (const FMTreplanningtask& rhs) = default;
		/*std::unique_ptr<FMTtask>clone() const;
		virtual void join();
		virtual void work();*/

	};

}



#endif
