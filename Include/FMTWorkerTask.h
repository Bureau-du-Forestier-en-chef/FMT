#ifndef FMTWORKERTASK_Hm_included
#define FMTWORKERTASK_Hm_included
#include <memory>
#include <boost/thread.hpp>

namespace Parallel
{
	class FMTtask;
	class FMTWorkerTask
	{
	public:
		FMTWorkerTask(std::unique_ptr<FMTtask> t);
		FMTWorkerTask(const FMTWorkerTask&) = delete;
		FMTWorkerTask& operator=(const FMTWorkerTask&) = delete;
		FMTWorkerTask(FMTWorkerTask&&) = delete;
		FMTWorkerTask& operator=(FMTWorkerTask&&) = delete;
		~FMTWorkerTask();
		bool isDone() const;
		boost::thread& getThread();
	private:
		std::unique_ptr<FMTtask> m_task;
		boost::thread m_worker;
		std::atomic<bool> m_done;
	};
}
#endif // FMTWORKERTASK_Hm_included