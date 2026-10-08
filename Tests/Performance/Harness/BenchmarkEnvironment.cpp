/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "BenchmarkEnvironment.h"

// windows.h comes before the FMT headers, so that NOMINMAX applies whichever header includes it.
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

#include "FMTObject.h"
#include "FMTVersion.h"

// Written at every build by CommitInfo.cmake; absent when the harness is compiled outside CMake.
#if __has_include("BenchmarkCommit.h")
#include "BenchmarkCommit.h"
#endif

#if defined(_MSC_VER) && (defined(_M_X64) || defined(_M_IX86))
#include <intrin.h>
#elif (defined(__GNUC__) || defined(__clang__)) && (defined(__x86_64__) || defined(__i386__))
#include <cpuid.h>
#endif

#ifdef __linux__
#include <sys/utsname.h>
#endif

#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>
#include <thread>
#include <vector>

namespace
{
	std::vector<std::string> compiledFeatures()
	{
		const char* const CANDIDATES[] = { "OSI", "MOSEK", "GDAL", "ONNXRUNTIME", "PYTHON", "R" };
		std::vector<std::string> features;
		for (const char* const CANDIDATE : CANDIDATES)
		{
			if (Version::FMTVersion::hasFeature(CANDIDATE))
			{
				features.push_back(CANDIDATE);
			}
		}
		return features;
	}

	// FMTlib is linked to mimalloc, but mimalloc serves malloc only when its DLL is loaded and has
	// redirected the C runtime (dynamic override on Windows). The owner of a block allocated here
	// tells which allocator serves the process.
	std::string activeAllocator()
	{
#ifdef _WIN32
		const wchar_t* const NAMES[] = { L"mimalloc.dll", L"mimalloc-override.dll", L"mimalloc-debug.dll" };
		HMODULE mimalloc = nullptr;
		for (const wchar_t* const NAME : NAMES)
		{
			mimalloc = GetModuleHandleW(NAME);
			if (mimalloc != nullptr)
			{
				break;
			}
		}
		if (mimalloc == nullptr)
		{
			return "CRT heap";
		}
		using VersionFunction = int(*)();
		using OwnershipFunction = bool(*)(const void*);
		const VersionFunction GET_VERSION = reinterpret_cast<VersionFunction>(GetProcAddress(mimalloc, "mi_version"));
		const OwnershipFunction OWNS = reinterpret_cast<OwnershipFunction>(GetProcAddress(mimalloc, "mi_is_in_heap_region"));
		std::string name = "mimalloc";
		if (GET_VERSION != nullptr)
		{
			// mi_version returns 212 for version 2.1.2.
			const int NUMBER = GET_VERSION();
			name += " " + std::to_string(NUMBER / 100) + "." + std::to_string((NUMBER / 10) % 10) + "."
				+ std::to_string(NUMBER % 10);
		}
		bool redirected = false;
		if (OWNS != nullptr)
		{
			void* const BLOCK = std::malloc(16);
			redirected = BLOCK != nullptr && OWNS(BLOCK);
			std::free(BLOCK);
		}
		return redirected ? name : "CRT heap (" + name + " loaded, not redirected)";
#else
		return "unknown";
#endif
	}

	std::string sourceCommit()
	{
#ifdef FMT_BENCHMARK_COMMIT
		return FMT_BENCHMARK_COMMIT;
#else
		return "unknown";
#endif
	}

	bool isDirty()
	{
#if defined(FMT_BENCHMARK_DIRTY) && FMT_BENCHMARK_DIRTY
		return true;
#else
		return false;
#endif
	}

	std::string configurationName()
	{
#ifdef FMT_BENCHMARK_CONFIGURATION
		return FMT_BENCHMARK_CONFIGURATION;
#else
		return "unknown";
#endif
	}

	bool isOptimized()
	{
#ifdef NDEBUG
		return true;
#else
		return false;
#endif
	}

	void setCompiler(Performance::BenchmarkEnvironment& p_environment)
	{
#if defined(_MSC_VER)
		p_environment.compiler = "MSVC";
		p_environment.compilerVersion = std::to_string(_MSC_FULL_VER);
#elif defined(__clang__)
		p_environment.compiler = "Clang";
		p_environment.compilerVersion = __clang_version__;
#elif defined(__GNUC__)
		p_environment.compiler = "GCC";
		p_environment.compilerVersion = __VERSION__;
#else
		p_environment.compiler = "unknown";
#endif
	}

	std::string operatingSystemName()
	{
#ifdef _WIN32
		// GetVersionEx reports the version the executable is manifested for, not the real one.
		using GetVersionFunction = LONG(WINAPI*)(RTL_OSVERSIONINFOW*);
		const HMODULE NTDLL = GetModuleHandleW(L"ntdll.dll");
		const GetVersionFunction GET_VERSION = NTDLL == nullptr ? nullptr
			: reinterpret_cast<GetVersionFunction>(GetProcAddress(NTDLL, "RtlGetVersion"));
		RTL_OSVERSIONINFOW version{};
		version.dwOSVersionInfoSize = sizeof(version);
		if (GET_VERSION != nullptr && GET_VERSION(&version) == 0)
		{
			return "Windows " + std::to_string(version.dwMajorVersion) + "." + std::to_string(version.dwMinorVersion)
				+ "." + std::to_string(version.dwBuildNumber);
		}
		return "Windows";
#elif defined(__linux__)
		utsname system{};
		if (uname(&system) == 0)
		{
			return std::string(system.sysname) + " " + system.release;
		}
		return "Linux";
#else
		return "unknown";
#endif
	}

	// Name of the processor as it reports it, read with the cpuid instruction on x86 processors.
	std::string processorName()
	{
		char brand[49] = {};
#if defined(_MSC_VER) && (defined(_M_X64) || defined(_M_IX86))
		int registers[4] = {};
		__cpuid(registers, static_cast<int>(0x80000000u));
		if (static_cast<unsigned int>(registers[0]) >= 0x80000004u)
		{
			for (unsigned int leaf = 0; leaf < 3; ++leaf)
			{
				__cpuid(registers, static_cast<int>(0x80000002u + leaf));
				std::memcpy(brand + leaf * 16, registers, 16);
			}
		}
#elif (defined(__GNUC__) || defined(__clang__)) && (defined(__x86_64__) || defined(__i386__))
		unsigned int registers[4] = {};
		if (__get_cpuid(0x80000000u, &registers[0], &registers[1], &registers[2], &registers[3]) != 0
			&& registers[0] >= 0x80000004u)
		{
			for (unsigned int leaf = 0; leaf < 3; ++leaf)
			{
				__get_cpuid(0x80000002u + leaf, &registers[0], &registers[1], &registers[2], &registers[3]);
				std::memcpy(brand + leaf * 16, registers, 16);
			}
		}
#endif
		std::string name(brand);
		const std::size_t FIRST = name.find_first_not_of(' ');
		if (FIRST == std::string::npos)
		{
			return "unknown";
		}
		return name.substr(FIRST, name.find_last_not_of(' ') - FIRST + 1);
	}

	std::string utcTimestamp()
	{
		const std::time_t NOW = std::time(nullptr);
		std::tm utc{};
#ifdef _WIN32
		gmtime_s(&utc, &NOW);
#else
		gmtime_r(&NOW, &utc);
#endif
		char text[32] = {};
		std::strftime(text, sizeof(text), "%Y-%m-%dT%H:%M:%SZ", &utc);
		return text;
	}
}

namespace Performance
{
	BenchmarkEnvironment BenchmarkEnvironment::collect(BenchmarkMode p_mode)
	{
		BenchmarkEnvironment environment;
		environment.fmtVersion = Version::FMTVersion::getVersion();
		environment.fmtBuildDate = Version::FMTVersion::getBuildDate();
		environment.features = compiledFeatures();
		environment.allocator = activeAllocator();
		environment.commit = sourceCommit();
		environment.dirty = isDirty();
		environment.buildType = configurationName();
		environment.optimized = isOptimized();
		setCompiler(environment);
		environment.operatingSystem = operatingSystemName();
		environment.processor = processorName();
		environment.logicalCores = std::thread::hardware_concurrency();
		environment.availableMemoryBytes = Core::FMTObject::getAvailableMemory();
		environment.timestamp = utcTimestamp();
		environment.mode = p_mode;
		return environment;
	}
}
