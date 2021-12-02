#include "FMTtask.hpp"
#include <functional>

namespace Parallel
{

	FMTtask::FMTtask(const FMTtask& rhs) :
		Core::FMTobject(rhs),
		done(false),
		taskmutex()
	{

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
		return std::unique_ptr<FMTtask>();
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
		return std::unique_ptr<FMTtask>(new FMTtask());
	}

	void FMTtask::work()
	{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTtask::work", __LINE__, __FILE__);
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTtask::work", __LINE__, __FILE__);
		}
	}

	void FMTtask::setstatus(bool status)
	{
		boost::lock_guard<boost::recursive_mutex> guard(taskmutex);
		done = status;
	}

	bool FMTtask::isdone() const
	{
		boost::lock_guard<boost::recursive_mutex> guard(taskmutex);
		const bool isdone = done;
		return done;
	}

}