#include "FMTtask.h"
#include <functional>
#include "FMTquietlogger.h"
#include "FMTexceptionhandler.h"
#include <boost/lexical_cast.hpp>

namespace Parallel
{

	boost::recursive_mutex FMTtask::taskmutex;

	boost::mutex FMTtask::m_checkpointMutex;

	size_t FMTtask::m_workingThreads = 0;

	size_t FMTtask::m_allThreads = 0;

	boost::condition_variable FMTtask::m_checkpoint;

	void FMTtask::setTotalThreads(const size_t& p_threads)
	{
		boost::mutex::scoped_lock guard(m_checkpointMutex);
		m_allThreads = p_threads;
		m_workingThreads = p_threads;
	}


	void FMTtask::decrementWorkingThread()
	{
		boost::mutex::scoped_lock guard(m_checkpointMutex);
		--m_workingThreads;
	}

	void FMTtask::checkpoint()
	{
		decrementWorkingThread();
		if (m_workingThreads == 0)
			{
			m_checkpoint.notify_all();
			boost::mutex::scoped_lock guard(m_checkpointMutex);
			m_workingThreads = m_allThreads;
		}else {
			boost::mutex::scoped_lock guard(m_checkpointMutex);
			while (m_workingThreads != m_allThreads) m_checkpoint.wait(guard);
			}
		//*_logger << "out " << getthreadid() << "\n";
	}


	FMTtask::FMTtask() :
		Core::FMTobject(),
		done(false),
		tasklogger(std::unique_ptr<Logging::FMTlogger>(new Logging::FMTquietlogger()))
	{
		
	}

	FMTtask::FMTtask(const FMTtask& rhs) :
		Core::FMTobject(rhs),
		done(false),
		tasklogger(std::unique_ptr<Logging::FMTlogger>(new Logging::FMTquietlogger()))
	{
		
	}

	FMTtask& FMTtask::operator = (const FMTtask& rhs)
	{
		if (this!=&rhs)
		{
			Core::FMTobject::operator=(rhs);
			done = rhs.done;
			tasklogger = std::unique_ptr<Logging::FMTlogger>(new Logging::FMTquietlogger());

		}
		return *this;
	}

	std::vector<std::unique_ptr<FMTtask>>FMTtask::split(const unsigned int& numberoftasks) const
	{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTtask::split", __LINE__, __FILE__);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTtask::split", __LINE__, __FILE__);
		}
		return std::vector<std::unique_ptr<FMTtask>>();
	}

	std::unique_ptr<FMTtask>FMTtask::spawn()
	{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTtask::spawn", __LINE__, __FILE__);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTtask::spawn", __LINE__, __FILE__);
		}
		return std::move(std::unique_ptr<FMTtask>());
	}


	std::unique_ptr<FMTtask>FMTtask::clone() const
	{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTtask::clone", __LINE__, __FILE__);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTtask::clone", __LINE__, __FILE__);
		}
		return std::move(std::unique_ptr<FMTtask>(new FMTtask()));
	}

	std::vector<size_t>FMTtask::splitwork(int numberoftasks, const int& totalworksize) const
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
					,"FMTtask::splitwork", __LINE__, __FILE__);
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
			_exhandler->raisefromcatch("", "FMTtask::splitwork", __LINE__, __FILE__);
			}
		return taskssize;
		}

	void FMTtask::work()
	{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTtask::work", __LINE__, __FILE__);
		}catch (...)
		{
			_exhandler->raisefromthreadcatch("", "FMTtask::work", __LINE__, __FILE__);
		}
	}

	void FMTtask::finalize()
	{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTtask::finalize", __LINE__, __FILE__);
		}
		catch (...)
		{
			_exhandler->raisefromthreadcatch("", "FMTtask::finalize", __LINE__, __FILE__);
		}
	}

	void FMTtask::setstatus(bool status)
	{
		checksignals();
		boost::lock_guard<boost::recursive_mutex> guard(taskmutex);
		done = status;
	}

	bool FMTtask::isdone() const
	{
		checksignals();
		boost::lock_guard<boost::recursive_mutex> guard(taskmutex);
		const bool isdone = (done );
		return done;
	}


	std::string FMTtask::getthreadid() const
	{
		checksignals();
		return boost::lexical_cast<std::string>(boost::this_thread::get_id());
	}

}