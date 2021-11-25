#include "FMTtask.hpp"
#include <functional>

namespace Parallel
{

	FMTtask::FMTtask(const FMTtask& rhs) :
		Core::FMTobject(rhs),
		worker()
	{

	}

	std::vector<std::unique_ptr<FMTtask>>FMTtask::split(const size_t& numberoftasks) const
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
	void FMTtask::join()
	{
		try {
			worker.join();
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTtask::join", __LINE__, __FILE__);
		}
	}
	void FMTtask::run()
	{
		try {
			worker = boost::thread(std::bind(&FMTtask::work,*this));
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTtask::run", __LINE__, __FILE__);
		}
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

}