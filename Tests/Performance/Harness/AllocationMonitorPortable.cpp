/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

// AllocationMonitor where the import tables cannot be redirected: nothing is counted, and the
// benchmarks that need an allocation count report it as skipped.

#include "AllocationMonitor.h"

namespace Performance
{
	bool AllocationMonitor::isSupported()
	{
		return false;
	}

	void AllocationMonitor::install()
	{
	}

	void AllocationMonitor::uninstall()
	{
	}

	void AllocationMonitor::start(ThreadScope)
	{
	}

	AllocationCounts AllocationMonitor::getCounts()
	{
		return AllocationCounts();
	}

	AllocationCounts AllocationMonitor::stop()
	{
		return AllocationCounts();
	}
}
