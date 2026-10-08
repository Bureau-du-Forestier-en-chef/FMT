/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PERFORMANCE_BENCHMARK_H_INCLUDED
#define PERFORMANCE_BENCHMARK_H_INCLUDED

#include "AllocationMonitor.h"
#include "RunSettings.h"

#include <chrono>
#include <cstddef>
#include <string>
#include <vector>

namespace Performance
{
	// One measured operation. The runner calls prepare once, then run many times: warm-up, timed
	// samples, then counted calls. Everything run needs is built by prepare, which is not measured.
	class Benchmark
	{
	public:
		virtual ~Benchmark() = default;
		// Name in the form <group>.<operation>[.<variant>], unique in its executable.
		virtual std::string getName() const = 0;
		// Data read by the benchmark, written with its results.
		virtual std::string getDataset() const = 0;
		// SHA-256 of the files of the dataset (see DatasetFingerprint), or an empty text. Known once
		// prepare has run.
		virtual std::string getDatasetFingerprint() const;
		// Why the benchmark cannot run in this build, such as a feature FMT is built without, or an
		// empty text. The runner then reports it as skipped, without calling prepare.
		virtual std::string getUnavailableReason() const;
		// How many times the runner calls run. The default suits an operation of microseconds or
		// less; a slower operation returns RunSettings::forSlowCalls, a model flow
		// RunSettings::forFlows.
		virtual RunSettings getSettings(BenchmarkMode p_mode) const;
		// Threads whose allocations are counted: the calling thread by default, every thread for an
		// operation that hands its work to other threads.
		virtual ThreadScope getThreadScope() const;
		// Number of threads the operation runs its work on, written with its results: 1 by default.
		virtual std::size_t getThreads() const;
		// Fingerprint of what the last call of run produced, written with its results, or an empty
		// text. The same operation run on another number of threads must give the same.
		virtual std::string getResultFingerprint() const;
		// Builds everything run needs. Not measured.
		virtual void prepare() = 0;
		// Runs the measured operation once and returns its result, which the runner compares with
		// the expected result of the benchmark.
		virtual double run() = 0;
		// Names of the phases of run, in their order, declared by definePhase: empty for an
		// operation timed as a whole.
		const std::vector<std::string>& getPhaseNames() const;
		// Time spent in each phase since clearPhaseDurations, in nanoseconds.
		const std::vector<double>& getPhaseDurations() const;
		// Sets the time of every phase to zero: the runner calls it before each timed sample.
		void clearPhaseDurations();

	protected:
		// Declares a phase of run and returns its index. Called before the first call of run, by
		// the constructor or by prepare, so that a phase costs no allocation in the measured calls.
		std::size_t definePhase(const std::string& p_name);
		// Marks the start of a call: run calls it before its first phase.
		void beginPhases();
		// Adds to phase p_phase the time since the previous mark, then marks now.
		void endPhase(std::size_t p_phase);

	private:
		std::vector<std::string> m_phaseNames;
		std::vector<double> m_phaseDurations;
		std::chrono::steady_clock::time_point m_phaseMark;
	};
}

#endif
