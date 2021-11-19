/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#ifndef FMTTASK_H_INCLUDED
#define FMTTASK_H_INCLUDED
#include <memory>
#include "FMTobject.hpp"
#include <boost/thread/thread.hpp>

/// Namespace for parallel tasking may include multithreading / multiprocessing
namespace Parallel
{
	class FMTtask : public Core::FMTobject
	{
		boost::thread worker;
	public:
		FMTtask() = default;
		FMTtask(const FMTtask& rhs) = default;
		FMTtask& operator = (const FMTtask& rhs) = default;
		FMTtask(const Core::FMTobject& baseobject);
		std::unique_ptr<FMTtask>clone() const;
		virtual void join();
		virtual void work();

	};

}



#endif
