#include "FMTtask.hpp"
#include <functional>
#include "FMTquietlogger.hpp"

namespace Parallel
{

	boost::recursive_mutex FMTtask::taskmutex;

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

	std::string FMTtask::getthreadid() const
	{
		return boost::lexical_cast<std::string>(boost::this_thread::get_id());
	}

}