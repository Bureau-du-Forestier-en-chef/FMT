/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTtaskhandler.hpp"
#include "FMTtask.hpp"
#include <boost/thread.hpp>

namespace Parallel
{
	FMTtaskhandler::FMTtaskhandler(const std::unique_ptr<FMTtask>& maintask,
		unsigned int maxthread) :
		alltasks(maintask->split(std::min(boost::thread::hardware_concurrency(), maxthread)))
	{

	}

	const std::vector<std::unique_ptr<FMTtask>>& FMTtaskhandler::gettasks() const
		{
		return alltasks;
		}

	template<class ptrtype>
	const std::vector<const ptrtype*> FMTtaskhandler::gettasksfromdynamiccast() const
	{
		std::vector<const ptrtype*>castedptr;
		for (const std::unique_ptr<FMTtask>& task : alltasks)
			{
			castedptr.push_back(dynamic_cast<ptrtype>(task.get()));
			}
		return castedptr;
	}

	void FMTtaskhandler::conccurentrun()
		{
		try {
			for (std::unique_ptr<FMTtask>& task : alltasks)
				{
				task->run();
				}
			for (std::unique_ptr<FMTtask>& task : alltasks)
				{
				task->join();
				}
		}catch (...)
			{
				_exhandler->printexceptions("", "FMTtaskhandler::conccurentrun", __LINE__, __FILE__);
			}

		}

}