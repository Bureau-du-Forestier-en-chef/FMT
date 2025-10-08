#include "FMTWorkerTask.h"
#include "FMTlogger.h"
#include "FMTtask.h"

namespace Parallel
{
	FMTWorkerTask::FMTWorkerTask(std::unique_ptr<FMTtask> t)
		: m_task(std::move(t)), m_done(false)
	{
		m_worker = boost::thread([this]() {
			this->m_task->run();
			this->m_done.store(true, std::memory_order_release);
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
