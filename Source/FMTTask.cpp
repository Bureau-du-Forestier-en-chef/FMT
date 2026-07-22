#include "FMTTask.h"
#include <functional>
#include "FMTTaskLogger.h"
#include "FMTExceptionHandler.h"
#include <boost/lexical_cast.hpp>

#if defined (_MSC_VER)
	#include <processthreadsapi.h>
#endif


namespace Parallel
{

	boost::recursive_mutex FMTTask::m_taskmutex;

	boost::mutex FMTTask::m_checkpointMutex;

	size_t FMTTask::m_workingThreads = 0;

	size_t FMTTask::m_allThreads = 0;

	boost::condition_variable FMTTask::m_checkpoint;

	void FMTTask::setTotalThreads(const size_t& p_threads)
	{
		boost::mutex::scoped_lock guard(m_checkpointMutex);
		m_allThreads = p_threads;
		m_workingThreads = p_threads;
	}


	void FMTTask::_decrementWorkingThread()
	{
		boost::mutex::scoped_lock guard(m_checkpointMutex);
		--m_workingThreads;
	}

	void FMTTask::_checkpoint()
	{
		_decrementWorkingThread();
		if (m_workingThreads == 0)
			{
			m_checkpoint.notify_all();
			boost::mutex::scoped_lock guard(m_checkpointMutex);
			m_workingThreads = m_allThreads;
		}else {
			boost::mutex::scoped_lock guard(m_checkpointMutex);
			while (m_workingThreads != m_allThreads) m_checkpoint.wait(guard);
			}
		//*_logger << "out " << getThreadId() << "\n";
	}
	


	void FMTTask::_setCrashHandlers()
	{
		try {
				setTerminateStack();
				setAbortStack();
				#if defined _MSC_VER
					m_SeTranslator = Exception::FMTScopedSeTranslator(Exception::FMTExceptionHandler::translateStructuralWIN32Exceptions);
				#endif
		}catch (...)
			{
				_exhandler->raiseFromCatch("", " FMTtask::_setCrashHandlers", __LINE__, __FILE__);
			}
	}


	FMTTask::FMTTask() :
		Core::FMTObject(),
		done(false),
		m_tasklogger(std::unique_ptr<Logging::FMTLogger>(new Logging::FMTTaskLogger()))
		#if defined _MSC_VER
			,m_SeTranslator(Exception::FMTExceptionHandler::translateStructuralWIN32Exceptions)
		#endif
	{
		
	}

	FMTTask::FMTTask(const FMTTask& rhs) :
		Core::FMTObject(rhs),
		done(false),
		m_tasklogger(std::unique_ptr<Logging::FMTLogger>(new Logging::FMTTaskLogger()))
		#if defined _MSC_VER
			,m_SeTranslator(Exception::FMTExceptionHandler::translateStructuralWIN32Exceptions)
		#endif
	{
		
	}

	FMTTask& FMTTask::operator = (const FMTTask& rhs)
	{
		if (this!=&rhs)
		{
			Core::FMTObject::operator=(rhs);
			done = rhs.done;
			m_tasklogger = std::unique_ptr<Logging::FMTLogger>(new Logging::FMTTaskLogger());

		}
		return *this;
	}

	std::vector<std::unique_ptr<FMTTask>>FMTTask::split(const unsigned int& numberoftasks) const
	{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTTask::split", __LINE__, __FILE__);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTTask::split", __LINE__, __FILE__);
		}
		return std::vector<std::unique_ptr<FMTTask>>();
	}

	std::unique_ptr<FMTTask>FMTTask::spawn()
	{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTTask::spawn", __LINE__, __FILE__);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTTask::spawn", __LINE__, __FILE__);
		}
		return std::move(std::unique_ptr<FMTTask>());
	}


	std::unique_ptr<FMTTask>FMTTask::clone() const
	{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTTask::clone", __LINE__, __FILE__);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTTask::clone", __LINE__, __FILE__);
		}
		return std::move(std::unique_ptr<FMTTask>(new FMTTask()));
	}

	std::vector<size_t>FMTTask::_splitWork(int numberoftasks, const int& totalworksize) const
		{
		std::vector<size_t>taskssize;
		try {
			numberoftasks = std::min(totalworksize, numberoftasks);
			if (numberoftasks<=0||
				totalworksize<=0)
				{
				numberoftasks = totalworksize;
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Cannot split task of size "+ std::to_string(totalworksize) +" in "+std::to_string(numberoftasks)
					,"FMTTask::_splitWork", __LINE__, __FILE__);
				}
			const int zp = numberoftasks - (totalworksize % numberoftasks);
			const int equaltask = totalworksize / numberoftasks;
			taskssize.reserve(numberoftasks);
			for (int taskid = 0; taskid < numberoftasks; ++taskid)
			{
				size_t load = static_cast<size_t>(equaltask);
				if (!(totalworksize % numberoftasks == 0) && taskid >= zp)
				{
					++load;
				}
				taskssize.push_back(load);
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTTask::_splitWork", __LINE__, __FILE__);
			}
		return taskssize;
		}

	void FMTTask::run()
		{
		try {
			_setCrashHandlers();
			work();
		}catch (...)
			{
			_exhandler->raiseFromThreadCatch("", "FMTTask::run", __LINE__, __FILE__);
			}
		}

	void FMTTask::work()
	{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTTask::work", __LINE__, __FILE__);
		}catch (...)
		{
			_exhandler->raiseFromThreadCatch("", "FMTTask::work", __LINE__, __FILE__);
		}
	}

	void FMTTask::finalize()
	{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTTask::finalize", __LINE__, __FILE__);
		}
		catch (...)
		{
			_exhandler->raiseFromThreadCatch("", "FMTTask::finalize", __LINE__, __FILE__);
		}
	}
	
	/*
	void FMTTask::setstatus(bool status)
	{
		checkSignals();
		boost::lock_guard<boost::recursive_mutex> guard(m_taskmutex);
		done = status;
	}

	bool FMTTask::isdone() const
	{
		checkSignals();
		boost::lock_guard<boost::recursive_mutex> guard(m_taskmutex);
		const bool isdone = (done );
		return done;
	}
	*/


	std::string FMTTask::getThreadId()
	{
	#if defined (_MSC_VER)
		DWORD threadWIN32 = GetThreadId(GetCurrentThread());
		return boost::lexical_cast<std::string>(threadWIN32);
	#else
		return boost::lexical_cast<std::string>(boost::this_thread::get_id());
	#endif
		
	}

}