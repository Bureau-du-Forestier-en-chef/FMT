/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PERFORMANCE_BENCHMARKOPTIONS_H_INCLUDED
#define PERFORMANCE_BENCHMARKOPTIONS_H_INCLUDED

#include <cstdint>
#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace Performance
{
	// Short run that only checks results and allocation bounds, or full timing measurement.
	enum class BenchmarkMode
	{
		Smoke,
		Full
	};

	std::string toString(BenchmarkMode p_mode);

	// What a benchmark must produce, read from the expectations file.
	struct Expectation
	{
		// Result of every call of the measured operation.
		double result = 0.0;
		// Most allocations a typical call may make, compared with the median of the counted calls.
		// A bound of 0 is stricter: no call may allocate.
		std::optional<std::int64_t> maximumAllocations;
		// Most bytes a typical call may keep allocated when it returns, compared in the same way. A
		// bound of 0 is stricter: no call may keep any.
		std::optional<std::int64_t> maximumRetainedBytes;
		// Highest private memory the process may reach, in megabytes. Checked only when the
		// benchmark runs alone in its process, as under ctest.
		std::optional<double> maximumPeakMegabytes;
	};

	// A benchmark defined by its row of the expectations file rather than by the executable: the
	// row gives its arguments, such as the model of a private benchmark.
	struct DefinedBenchmark
	{
		std::string name;
		std::string arguments;
	};

	// Command line of a benchmark executable:
	//
	//   --benchmark <name>       runs one benchmark, named exactly
	//   --filter <pattern>       runs the benchmarks whose name matches, * matching any text
	//   --mode smoke|full        default: the FMT_BENCHMARK_MODE environment variable, else smoke
	//   --model <primary file>   project read by the benchmarks
	//   --expectations <csv>     expected results and bounds (performance.csv)
	//   --expected <value>       replaces the expected result of the benchmark named by --benchmark
	//   --max-allocations <n>    replaces its allocation bound
	//   --max-retained-bytes <n> replaces its bound on the memory a call keeps
	//   --max-peak-memory <MB>   replaces its bound on the peak private memory of the process
	//   --output <json file>     where the results are written
	//   --list                   prints the names of the selected benchmarks and runs none
	//
	// Without an option, every benchmark runs on TWD_land, with the expectations and the output
	// folder of the source tree seen from build/release/bin/Release.
	class BenchmarkOptions
	{
	public:
		// Reads the command line. p_executable names the rows of the expectations file that
		// belong to the executable.
		static BenchmarkOptions parse(int p_argc, const char* const p_argv[], const std::string& p_executable);
		bool isListing() const;
		// The --benchmark name or the --filter pattern.
		const std::string& getSelection() const;
		bool selects(const std::string& p_benchmark) const;
		BenchmarkMode getMode() const;
		const std::string& getModelFile() const;
		const std::filesystem::path& getOutputFile() const;
		// Folder of the results file, where a benchmark writes what it produces.
		std::filesystem::path getWorkFolder() const;
		// Returns the expectation of p_benchmark, or nothing when the expectations file has no row
		// for it.
		std::optional<Expectation> getExpectation(const std::string& p_benchmark) const;
		// The rows of the expectations file that give arguments, in their order.
		const std::vector<DefinedBenchmark>& getDefinedBenchmarks() const;

	private:
		// Selected benchmark names, * matching any text.
		std::string m_pattern = "*";
		// True when m_pattern is the exact name given by --benchmark.
		bool m_exactName = false;
		bool m_listing = false;
		BenchmarkMode m_mode = BenchmarkMode::Smoke;
		std::string m_modelFile;
		std::filesystem::path m_expectationsFile;
		std::filesystem::path m_outputFile;
		std::optional<double> m_expectedOverride;
		std::optional<std::int64_t> m_boundOverride;
		std::optional<std::int64_t> m_retainedBoundOverride;
		std::optional<double> m_peakBoundOverride;
		std::map<std::string, Expectation> m_expectations;
		std::vector<DefinedBenchmark> m_definedBenchmarks;

		BenchmarkOptions();
		void _set(const std::string& p_option, const std::string& p_value);
		void _readExpectations(const std::string& p_executable);
		void _applyOverrides();
	};
}

#endif
