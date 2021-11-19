#include "FMTtask.hpp"
#include <functional>

namespace Parallel
{
	FMTtask::FMTtask(const Core::FMTobject& baseobject) :
		worker(std::bind(&FMTtask::join,this))
		{
		this->passinobject(baseobject);
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
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTtask::join", __LINE__, __FILE__);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTtask::join", __LINE__, __FILE__);
		}
	}
	void FMTtask::work()
	{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTtask::join", __LINE__, __FILE__);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTtask::join", __LINE__, __FILE__);
		}
	}
}