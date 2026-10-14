/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "ProcessorPolicy.h"

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

#include <string>
#include <vector>

namespace
{
#ifdef _WIN32
	// Opts the process out of power throttling (EcoQoS), which Windows applies on its own to a
	// process whose window is in the background.
	bool disablePowerThrottling()
	{
		PROCESS_POWER_THROTTLING_STATE state{};
		state.Version = PROCESS_POWER_THROTTLING_CURRENT_VERSION;
		state.ControlMask = PROCESS_POWER_THROTTLING_EXECUTION_SPEED;
		state.StateMask = 0;
		return SetProcessInformation(GetCurrentProcess(), ProcessPowerThrottling, &state, sizeof(state)) != 0;
	}

	// The CPU set functions of Windows 10, looked up in kernel32.dll: MinGW declares them only from
	// mingw-w64 12, and its import library may lack them.
	using CpuSetInformationFunction = BOOL(WINAPI*)(PSYSTEM_CPU_SET_INFORMATION, ULONG, PULONG, HANDLE, ULONG);
	using CpuSetSelectionFunction = BOOL(WINAPI*)(HANDLE, const ULONG*, ULONG);

	template <typename Function>
	Function kernelFunction(const char* p_name)
	{
		const HMODULE KERNEL = GetModuleHandleW(L"kernel32.dll");
		return KERNEL == nullptr ? nullptr : reinterpret_cast<Function>(GetProcAddress(KERNEL, p_name));
	}

	// The CPU sets of the logical processors, and the identifiers of those of the most performant
	// efficiency class (a higher class runs faster).
	struct CpuSets
	{
		std::size_t logicalProcessors = 0;
		std::vector<ULONG> fastest;
	};

	CpuSets readCpuSets()
	{
		CpuSets cpuSets;
		const CpuSetInformationFunction GET_CPU_SET_INFORMATION =
			kernelFunction<CpuSetInformationFunction>("GetSystemCpuSetInformation");
		if (GET_CPU_SET_INFORMATION == nullptr)
		{
			return cpuSets;
		}
		ULONG length = 0;
		GET_CPU_SET_INFORMATION(nullptr, 0, &length, GetCurrentProcess(), 0);
		if (length == 0)
		{
			return cpuSets;
		}
		std::vector<unsigned char> buffer(length);
		if (GET_CPU_SET_INFORMATION(reinterpret_cast<PSYSTEM_CPU_SET_INFORMATION>(buffer.data()), length, &length,
			GetCurrentProcess(), 0) == 0)
		{
			return cpuSets;
		}
		std::vector<SYSTEM_CPU_SET_INFORMATION> entries;
		for (ULONG offset = 0; offset < length;)
		{
			const SYSTEM_CPU_SET_INFORMATION* const ENTRY = reinterpret_cast<const SYSTEM_CPU_SET_INFORMATION*>(buffer.data() + offset);
			if (ENTRY->Size == 0)
			{
				break;
			}
			if (ENTRY->Type == CpuSetInformation)
			{
				entries.push_back(*ENTRY);
			}
			offset += ENTRY->Size;
		}
		BYTE highestClass = 0;
		for (const SYSTEM_CPU_SET_INFORMATION& ENTRY : entries)
		{
			highestClass = ENTRY.CpuSet.EfficiencyClass > highestClass ? ENTRY.CpuSet.EfficiencyClass : highestClass;
		}
		for (const SYSTEM_CPU_SET_INFORMATION& ENTRY : entries)
		{
			if (ENTRY.CpuSet.EfficiencyClass == highestClass)
			{
				cpuSets.fastest.push_back(ENTRY.CpuSet.Id);
			}
		}
		cpuSets.logicalProcessors = entries.size();
		return cpuSets;
	}
#endif
}

namespace Performance
{
	std::string ProcessorPolicy::apply()
	{
#ifdef _WIN32
		const bool UNTHROTTLED = disablePowerThrottling();
		const CpuSets CPU_SETS = readCpuSets();
		std::string description;
		// The default CPU sets of the process hold for the threads it starts without sets of their
		// own, such as the workers of a replanning.
		const ULONG FASTEST = static_cast<ULONG>(CPU_SETS.fastest.size());
		const CpuSetSelectionFunction SELECT_FOR_THREAD = kernelFunction<CpuSetSelectionFunction>("SetThreadSelectedCpuSets");
		const CpuSetSelectionFunction SELECT_FOR_PROCESS = kernelFunction<CpuSetSelectionFunction>("SetProcessDefaultCpuSets");
		if (!CPU_SETS.fastest.empty() && CPU_SETS.fastest.size() < CPU_SETS.logicalProcessors
			&& SELECT_FOR_THREAD != nullptr && SELECT_FOR_PROCESS != nullptr
			&& SELECT_FOR_THREAD(GetCurrentThread(), CPU_SETS.fastest.data(), FASTEST) != 0
			&& SELECT_FOR_PROCESS(GetCurrentProcess(), CPU_SETS.fastest.data(), FASTEST) != 0)
		{
			description = "fastest cores for every thread, " + std::to_string(CPU_SETS.fastest.size()) + " of "
				+ std::to_string(CPU_SETS.logicalProcessors) + " logical processors";
		}
		else
		{
			description = "all " + std::to_string(CPU_SETS.logicalProcessors) + " logical processors";
		}
		return description + (UNTHROTTLED ? ", not throttled" : ", throttling not disabled");
#else
		return "not controlled";
#endif
	}
}
