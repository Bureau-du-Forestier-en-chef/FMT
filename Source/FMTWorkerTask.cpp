#include "FMTWorkerTask.h"
#include "FMTLogger.h"
#include "FMTTask.h"

namespace Parallel
{
	FMTWorkerTask::FMTWorkerTask(std::unique_ptr<FMTTask> t)
		: FMTWorkerTask(std::move(t), static_cast<FMTWorkerTaskCompletionSignal*>(nullptr))
	{
	}
	FMTWorkerTask::FMTWorkerTask(std::unique_ptr<FMTTask> p_task,
		FMTWorkerTaskCompletionSignal& p_completion)
		: FMTWorkerTask(std::move(p_task), &p_completion)
	{
	}
	FMTWorkerTask::FMTWorkerTask(std::unique_ptr<FMTTask> p_task,
		FMTWorkerTaskCompletionSignal* p_completion)
		: m_task(std::move(p_task)), m_done(false)
	{
		m_worker = boost::thread([this, p_completion]() {
			this->m_task->run();
			this->m_done.store(true, std::memory_order_release);
			if (p_completion)
			{
				{
					std::lock_guard<std::mutex> lock(p_completion->m_mutex);
					++p_completion->m_generation;
				}
				p_completion->m_condition.notify_one();
			}
			});
	}
	FMTWorkerTask::~FMTWorkerTask()
	{
		if (m_worker.joinable())
			m_worker.join();
	}
	bool FMTWorkerTask::isDone() const
	{
		return m_done.load(std::memory_order_acquire);
	}
	boost::thread& FMTWorkerTask::getThread()
	{
		return m_worker;
	}
}
