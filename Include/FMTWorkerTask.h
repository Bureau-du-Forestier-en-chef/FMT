#ifndef FMTWORKERTASK_Hm_included
#define FMTWORKERTASK_Hm_included
#include <boost/thread.hpp>
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <memory>
#include <mutex>

namespace Parallel
{
	class FMTTask;
 struct FMTWorkerTaskCompletionSignal
	{
		std::mutex m_mutex;
		std::condition_variable m_condition;
		std::uint64_t m_generation = 0;
	};

	// DocString: FMTWorkerTask
	/**
	@brief RAII wrapper running a FMTTask on its own boost thread and tracking its completion.
	*/
	class FMTWorkerTask
	{
	public:
		// DocString: FMTWorkerTask(std::unique_ptr<FMTTask>)
		/**
		@brief Construct a FMTWorkerTask from a task and start it on a worker thread.
		@param[in] t the task to run.
		*/
		FMTWorkerTask(std::unique_ptr<FMTTask> t);
		// DocString: FMTWorkerTask(std::unique_ptr<FMTTask>,FMTWorkerTaskCompletionSignal&)
		/**
		@brief Construct a worker that notifies when its task is complete.
		@param[in] p_task the task to run.
		@param[in] p_completion the signal to notify on completion.
		*/
		FMTWorkerTask(std::unique_ptr<FMTTask> p_task,
			FMTWorkerTaskCompletionSignal& p_completion);
		// DocString: FMTWorkerTask(const FMTWorkerTask&)
		/**
		@brief Deleted copy constructor for FMTWorkerTask.
		*/
		FMTWorkerTask(const FMTWorkerTask&) = delete;
		// DocString: FMTWorkerTask::operator=(const FMTWorkerTask&)
		/**
		@brief Deleted copy assignment operator for FMTWorkerTask.
		*/
		FMTWorkerTask& operator=(const FMTWorkerTask&) = delete;
		// DocString: FMTWorkerTask(FMTWorkerTask&&)
		/**
		@brief Deleted move constructor for FMTWorkerTask.
		*/
		FMTWorkerTask(FMTWorkerTask&&) = delete;
		// DocString: FMTWorkerTask::operator=(FMTWorkerTask&&)
		/**
		@brief Deleted move assignment operator for FMTWorkerTask.
		*/
		FMTWorkerTask& operator=(FMTWorkerTask&&) = delete;
		// DocString: ~FMTWorkerTask()
		/**
		@brief Destructor for FMTWorkerTask, joining the worker thread.
		*/
		~FMTWorkerTask();
		// DocString: FMTWorkerTask::isDone
		/**
		@brief Return true if the task is done.
		@return true if the task is done else false.
		*/
		bool isDone() const;
		// DocString: FMTWorkerTask::getThread
		/**
		@brief Return the worker thread.
		@return the worker thread.
		*/
		boost::thread& getThread();
	private:
		FMTWorkerTask(std::unique_ptr<FMTTask> p_task,
			FMTWorkerTaskCompletionSignal* p_completion);
		std::unique_ptr<FMTTask> m_task;
		boost::thread m_worker;
		std::atomic<bool> m_done;
	};
}
#endif // FMTWORKERTASK_Hm_included