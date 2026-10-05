/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "BenchmarkSuite.h"

#include "BenchmarkRunner.h"
#include "ProcessorPolicy.h"

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>

namespace
{
	// A process can be paused for about 30 ms some 50 ms after it reads its first file, and run slower
	// until then: seen on Windows 11, even in a program that only reads one file. A full measurement
	// lets this pass before its first benchmark.
	constexpr std::chrono::milliseconds SETTLING_TIME{ 250 };

	std::string gigabytes(std::uint64_t p_bytes)
	{
		std::ostringstream text;
		text << std::fixed << std::setprecision(1) << static_cast<double>(p_bytes) / 1073741824.0 << " GB";
		return text.str();
	}

	// A duration in a unit that keeps it readable: nanoseconds below a millisecond, then
	// milliseconds, then seconds.
	std::string duration(double p_nanoseconds)
	{
		std::ostringstream text;
		if (p_nanoseconds < 1e6)
		{
			text << std::fixed << std::setprecision(1) << p_nanoseconds << " ns";
		}
		else if (p_nanoseconds < 1e9)
		{
			text << std::fixed << std::setprecision(3) << p_nanoseconds / 1e6 << " ms";
		}
		else
		{
			text << std::fixed << std::setprecision(3) << p_nanoseconds / 1e9 << " s";
		}
		return text.str();
	}

	std::string megabytes(std::uint64_t p_bytes)
	{
		std::ostringstream text;
		text << std::fixed << std::setprecision(1) << static_cast<double>(p_bytes) / 1e6 << " MB";
		return text.str();
	}

	void printEnvironment(const Performance::BenchmarkEnvironment& p_environment)
	{
		std::cout << "FMT " << p_environment.fmtVersion << ", commit " << p_environment.commit
			<< (p_environment.dirty ? " (modified)" : "") << ", " << p_environment.buildType << ", "
			<< p_environment.compiler << " " << p_environment.compilerVersion << ", " << p_environment.allocator
			<< std::endl;
		std::cout << p_environment.operatingSystem << ", " << p_environment.processor << ", "
			<< p_environment.logicalCores << " logical cores, " << gigabytes(p_environment.availableMemoryBytes)
			<< " available, mode " << Performance::toString(p_environment.mode) << std::endl;
		std::cout << "Measuring thread on " << p_environment.processors << std::endl;
		if (!p_environment.optimized)
		{
			std::cout << "WARNING: the benchmarks are not optimized, their times are not a reference" << std::endl;
		}
	}

	// One line per measure, then one line per check, prefixed like the lines of Testing::Checker.
	void printResult(const Performance::BenchmarkResult& p_result)
	{
		const Performance::TimingStatistics& TIMING = p_result.timing;
		std::cout << std::endl << p_result.benchmark << " (" << p_result.dataset << ")" << std::endl;
		std::cout << "  time    " << duration(TIMING.medianNs) << " per call, median of " << TIMING.samples
			<< " samples of " << TIMING.callsPerSample << " calls (min " << duration(TIMING.minimumNs)
			<< ", max " << duration(TIMING.maximumNs) << ")" << std::endl;
		for (const Performance::PhaseStatistics& PHASE : p_result.phases)
		{
			std::cout << "  phase   " << PHASE.name << ": " << duration(PHASE.medianNs) << " (min " << duration(PHASE.minimumNs)
				<< ", max " << duration(PHASE.maximumNs) << ")" << std::endl;
		}
		const Performance::AllocationStatistics& ALLOCATIONS = p_result.allocations;
		if (ALLOCATIONS.measured)
		{
			std::cout << "  heap    " << ALLOCATIONS.medianPerCall << " allocations per call (min "
				<< ALLOCATIONS.minimumPerCall << ", max " << ALLOCATIONS.maximumPerCall << " over " << ALLOCATIONS.calls
				<< " calls), " << ALLOCATIONS.medianBytesPerCall << " bytes per call, "
				<< ALLOCATIONS.medianRetainedBytesPerCall << " retained" << std::endl;
		}
		std::cout << "  memory  " << megabytes(p_result.processPeakPrivateBytes) << " peak private memory of the process" << std::endl;
		if (!p_result.datasetFingerprint.empty())
		{
			std::cout << "  data    SHA-256 " << p_result.datasetFingerprint << std::endl;
		}
		for (const Performance::BenchmarkCheck& CHECK : p_result.checks)
		{
			std::cout << (CHECK.passed ? "  ok      " : "  FAILED  ") << CHECK.description << std::endl;
		}
		if (p_result.isSkipped())
		{
			std::cout << "  SKIPPED " << p_result.skipReason << std::endl;
		}
	}

	int exitCode(const std::vector<Performance::BenchmarkResult>& p_results)
	{
		bool everySkipped = true;
		for (const Performance::BenchmarkResult& RESULT : p_results)
		{
			if (!RESULT.isValid())
			{
				return 1;
			}
			everySkipped = everySkipped && RESULT.isSkipped();
		}
		return everySkipped ? Performance::SKIP_RETURN_CODE : 0;
	}
}

namespace Performance
{
	BenchmarkSuite::BenchmarkSuite(const BenchmarkOptions& p_options) :
		m_options(p_options)
	{
	}

	void BenchmarkSuite::add(std::unique_ptr<Benchmark> p_benchmark)
	{
		for (const std::unique_ptr<Benchmark>& BENCHMARK : m_benchmarks)
		{
			if (BENCHMARK->getName() == p_benchmark->getName())
			{
				throw std::invalid_argument("Two benchmarks are named " + p_benchmark->getName());
			}
		}
		m_benchmarks.push_back(std::move(p_benchmark));
	}

	int BenchmarkSuite::run()
	{
		const std::vector<Benchmark*> SELECTED = _select();
		if (m_options.isListing())
		{
			for (const Benchmark* const BENCHMARK : SELECTED)
			{
				std::cout << BENCHMARK->getName() << std::endl;
			}
			return 0;
		}
		if (SELECTED.empty())
		{
			std::cerr << "No benchmark matches \"" << m_options.getSelection() << "\"" << std::endl;
			return 1;
		}
		BenchmarkEnvironment environment = BenchmarkEnvironment::collect(m_options.getMode());
		environment.processors = ProcessorPolicy::applyToMeasuringThread();
		printEnvironment(environment);
		if (m_options.getMode() == BenchmarkMode::Full)
		{
			std::this_thread::sleep_for(SETTLING_TIME);
		}
		std::vector<BenchmarkResult> results;
		for (Benchmark* const BENCHMARK : SELECTED)
		{
			BenchmarkRunner runner(BENCHMARK->getSettings(m_options.getMode()));
			const bool FIRST_IN_PROCESS = results.empty();
			results.push_back(runner.run(*BENCHMARK, m_options.getExpectation(BENCHMARK->getName()), FIRST_IN_PROCESS));
			printResult(results.back());
		}
		_write(environment, results);
		return exitCode(results);
	}

	std::vector<Benchmark*> BenchmarkSuite::_select() const
	{
		std::vector<Benchmark*> selected;
		for (const std::unique_ptr<Benchmark>& BENCHMARK : m_benchmarks)
		{
			if (m_options.selects(BENCHMARK->getName()))
			{
				selected.push_back(BENCHMARK.get());
			}
		}
		return selected;
	}

	void BenchmarkSuite::_write(const BenchmarkEnvironment& p_environment, const std::vector<BenchmarkResult>& p_results) const
	{
		const std::filesystem::path& OUTPUT_FILE = m_options.getOutputFile();
		if (OUTPUT_FILE.has_parent_path())
		{
			std::filesystem::create_directories(OUTPUT_FILE.parent_path());
		}
		std::ofstream stream(OUTPUT_FILE);
		if (!stream)
		{
			throw std::runtime_error("Cannot write the results file " + OUTPUT_FILE.string());
		}
		writeResults(stream, p_environment, p_results);
		std::cout << std::endl << "Results written to " << OUTPUT_FILE.string() << std::endl;
	}
}
