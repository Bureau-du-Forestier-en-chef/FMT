/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

// Model flows: at every call, a model is read, then built and solved, replayed, simulated or
// replanned, as an application does, or the outputs of a model are computed. Each flow times its
// phases, so that a change shows where it costs: in FMT (reading, building) or in the solver. The
// same classes measure the flows on TWD_land and the private ones, whose model and arguments come
// from their row of the expectations file.

#include "FlowBenchmarks.h"

#include "Benchmark.h"
#include "BenchmarkSuite.h"
#include "DatasetFingerprint.h"
#include "DefinedBenchmarks.h"
#include "QuietFmt.h"

#ifdef FMTWITHOSI
	#include "FMTLpModel.h"
	#include "FMTModel.h"
	#include "FMTModelParser.h"
	#include "FMTNssModel.h"
	#include "FMTOutput.h"
	#include "FMTReplanningTask.h"
	#include "FMTSchedule.h"
	#include "FMTTaskHandler.h"
#endif

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace
{
	// Seed of the non-spatial simulations, as in the replanning examples.
	constexpr unsigned int SEED = 0;
	// Tolerance of a replayed schedule, as in FMTsetsolution.
	constexpr double REPLAY_TOLERANCE = 0.01;
	// Minimal drift of a replanned output, as in the replanning examples.
	constexpr double MINIMAL_DRIFT = 0.5;

	std::vector<std::string> split(const std::string& p_text, char p_separator)
	{
		std::vector<std::string> fields;
		std::stringstream stream(p_text);
		std::string field;
		while (std::getline(stream, field, p_separator))
		{
			fields.push_back(field);
		}
		return fields;
	}

#ifdef FMTWITHOSI
	// Rows of a CSV file written by a replanning, its header left out.
	std::size_t countRows(const std::filesystem::path& p_file)
	{
		std::ifstream stream(p_file);
		if (!stream)
		{
			throw std::runtime_error("The replanning wrote no file " + p_file.string());
		}
		std::size_t rows = 0;
		std::string line;
		bool header = true;
		while (std::getline(stream, line))
		{
			if (header)
			{
				header = false;
			}
			else if (!line.empty())
			{
				++rows;
			}
		}
		return rows;
	}

	// MOSEK, the solver of production, when FMT is built with it; CLP otherwise.
	Models::FMTSolverInterface solver()
	{
	#ifdef FMTWITHMOSEK
		return Models::FMTSolverInterface::MOSEK;
	#else
		return Models::FMTSolverInterface::CLP;
	#endif
	}

	// Sum of the finite totals of the first p_maximumOutputs outputs of the model, all of them when it
	// is 0, over p_length periods. An output that gives no total is left out.
	double sumOfOutputs(const Models::FMTModel& p_model, int p_length, std::size_t p_maximumOutputs)
	{
		double sum = 0.0;
		std::size_t outputs = 0;
		for (const Core::FMTOutput& OUTPUT : p_model.getOutputs())
		{
			if (p_maximumOutputs > 0 && outputs == p_maximumOutputs)
			{
				break;
			}
			++outputs;
			for (int period = 1; period <= p_length; ++period)
			{
				const std::map<std::string, double> VALUES = p_model.getOutput(OUTPUT, period, Core::FMToutputlevel::totalonly);
				const auto TOTAL = VALUES.find("Total");
				if (TOTAL != VALUES.end() && std::isfinite(TOTAL->second))
				{
					sum += TOTAL->second;
				}
			}
		}
		return sum;
	}

	// Builds a model with the schedule of its scenario over p_length periods, without solving it, as
	// FMTsetsolution does.
	void buildWithSchedule(Models::FMTLpModel& p_model, const std::vector<Core::FMTSchedule>& p_schedules, int p_length)
	{
		p_model.setParameter(Models::FMTintmodelparameters::LENGTH, p_length);
		p_model.setParameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD, true);
		p_model.setParameter(Models::FMTboolmodelparameters::QUIET_LOGGING, true);
		p_model.FMTModel::setParameter(Models::FMTdblmodelparameters::TOLERANCE, REPLAY_TOLERANCE);
		p_model.doPlanning(false, p_schedules);
	}

	double outputValue(const Models::FMTModel& p_model, const std::string& p_output, int p_period)
	{
		for (const Core::FMTOutput& OUTPUT : p_model.getOutputs())
		{
			if (OUTPUT.getName() == p_output)
			{
				return p_model.getOutput(OUTPUT, p_period, Core::FMToutputlevel::totalonly).at("Total");
			}
		}
		throw std::invalid_argument("The model has no output " + p_output);
	}
#endif

	// What every flow shares: its model, the fingerprint of its files and its settings.
	class FlowBenchmark : public Performance::Benchmark
	{
	public:
		FlowBenchmark(std::string p_name, std::string p_primaryFile, std::vector<std::string> p_scenarios);
		std::string getName() const override;
		std::string getDataset() const override;
		std::string getDatasetFingerprint() const override;
		std::string getUnavailableReason() const override;
		Performance::RunSettings getSettings(Performance::BenchmarkMode p_mode) const override;
		void prepare() override;

	protected:
		const std::string& getPrimaryFile() const;
		const std::vector<std::string>& getScenarios() const;

	private:
		std::string m_name;
		std::string m_primaryFile;
		std::vector<std::string> m_scenarios;
		std::string m_fingerprint;
	};

	FlowBenchmark::FlowBenchmark(std::string p_name, std::string p_primaryFile, std::vector<std::string> p_scenarios) :
		m_name(std::move(p_name)),
		m_primaryFile(std::move(p_primaryFile)),
		m_scenarios(std::move(p_scenarios))
	{
	}

	std::string FlowBenchmark::getName() const
	{
		return m_name;
	}

	std::string FlowBenchmark::getDataset() const
	{
		std::string scenarios;
		for (const std::string& SCENARIO : m_scenarios)
		{
			scenarios += (scenarios.empty() ? "" : "+") + SCENARIO;
		}
		return std::filesystem::path(m_primaryFile).stem().string() + "/" + scenarios;
	}

	std::string FlowBenchmark::getDatasetFingerprint() const
	{
		return m_fingerprint;
	}

	std::string FlowBenchmark::getUnavailableReason() const
	{
	#ifdef FMTWITHOSI
		return std::string();
	#else
		return "FMT is built without OSI";
	#endif
	}

	// A flow lasts from milliseconds on TWD_land to minutes on a production model.
	Performance::RunSettings FlowBenchmark::getSettings(Performance::BenchmarkMode p_mode) const
	{
		return Performance::RunSettings::forFlows(p_mode);
	}

	void FlowBenchmark::prepare()
	{
		Performance::quietFmt();
		m_fingerprint = Performance::DatasetFingerprint::compute(m_primaryFile, m_scenarios);
	}

	const std::string& FlowBenchmark::getPrimaryFile() const
	{
		return m_primaryFile;
	}

	const std::vector<std::string>& FlowBenchmark::getScenarios() const
	{
		return m_scenarios;
	}

	// Reads a scenario, builds its graph and matrix over p_length periods and solves it. Returns the
	// objective, the same with MOSEK and CLP.
	class OptimizeFlow final : public FlowBenchmark
	{
	public:
		OptimizeFlow(std::string p_name, std::string p_primaryFile, std::string p_scenario, int p_length);
		double run() override;

	private:
		int m_length;
		std::size_t m_read;
		std::size_t m_build;
		std::size_t m_solve;
	};

	OptimizeFlow::OptimizeFlow(std::string p_name, std::string p_primaryFile, std::string p_scenario, int p_length) :
		FlowBenchmark(std::move(p_name), std::move(p_primaryFile), std::vector<std::string>(1, p_scenario)),
		m_length(p_length),
		m_read(definePhase("read")),
		m_build(definePhase("build")),
		m_solve(definePhase("solve"))
	{
	}

	double OptimizeFlow::run()
	{
	#ifdef FMTWITHOSI
		beginPhases();
		Parser::FMTModelParser parser;
		const std::vector<Models::FMTModel> MODELS = parser.readproject(getPrimaryFile(), getScenarios());
		endPhase(m_read);
		Models::FMTLpModel model(MODELS.at(0), solver());
		model.setParameter(Models::FMTintmodelparameters::LENGTH, m_length);
		model.setParameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS, 1);
		model.setParameter(Models::FMTboolmodelparameters::QUIET_LOGGING, true);
		model.build();
		endPhase(m_build);
		if (!model.solve())
		{
			throw std::runtime_error(getName() + ": the model has no optimal solution");
		}
		endPhase(m_solve);
		return model.getObjValue();
	#else
		return 0.0;
	#endif
	}

	// Reads a scenario and its schedule, then builds the model with the schedule over p_length
	// periods without solving it, as FMTsetsolution does. Returns the value of p_output at p_period:
	// no solver takes part.
	class ReplayFlow final : public FlowBenchmark
	{
	public:
		ReplayFlow(std::string p_name, std::string p_primaryFile, std::string p_scenario, int p_length, std::string p_output,
			int p_period);
		double run() override;

	private:
		int m_length;
		std::string m_output;
		int m_period;
		std::size_t m_read;
		std::size_t m_build;
	};

	ReplayFlow::ReplayFlow(std::string p_name, std::string p_primaryFile, std::string p_scenario, int p_length,
		std::string p_output, int p_period) :
		FlowBenchmark(std::move(p_name), std::move(p_primaryFile), std::vector<std::string>(1, p_scenario)),
		m_length(p_length),
		m_output(std::move(p_output)),
		m_period(p_period),
		m_read(definePhase("read")),
		m_build(definePhase("build"))
	{
	}

	double ReplayFlow::run()
	{
	#ifdef FMTWITHOSI
		beginPhases();
		Parser::FMTModelParser parser;
		const std::vector<Models::FMTModel> MODELS = parser.readproject(getPrimaryFile(), getScenarios());
		const std::vector<Core::FMTSchedule> SCHEDULES = parser.readSchedules(getPrimaryFile(), MODELS).at(0);
		endPhase(m_read);
		Models::FMTLpModel model(MODELS.at(0), solver());
		buildWithSchedule(model, SCHEDULES, m_length);
		endPhase(m_build);
		return outputValue(model, m_output, m_period);
	#else
		return 0.0;
	#endif
	}

	// Computes the outputs of a model built with the schedule of its scenario, as a report does. The
	// model is read and built once, by prepare, without a solver; each call computes the first
	// p_outputs outputs of the model, all of them when p_outputs is 0, over p_length periods, and
	// returns the sum of their finite totals.
	class OutputsFlow final : public FlowBenchmark
	{
	public:
		OutputsFlow(std::string p_name, std::string p_primaryFile, std::string p_scenario, int p_length, std::size_t p_outputs);
		void prepare() override;
		double run() override;

	private:
		int m_length;
		std::size_t m_outputs;
	#ifdef FMTWITHOSI
		std::unique_ptr<Models::FMTLpModel> m_model;
	#endif
	};

	OutputsFlow::OutputsFlow(std::string p_name, std::string p_primaryFile, std::string p_scenario, int p_length,
		std::size_t p_outputs) :
		FlowBenchmark(std::move(p_name), std::move(p_primaryFile), std::vector<std::string>(1, p_scenario)),
		m_length(p_length),
		m_outputs(p_outputs)
	{
	}

	void OutputsFlow::prepare()
	{
		FlowBenchmark::prepare();
	#ifdef FMTWITHOSI
		Parser::FMTModelParser parser;
		const std::vector<Models::FMTModel> MODELS = parser.readproject(getPrimaryFile(), getScenarios());
		const std::vector<Core::FMTSchedule> SCHEDULES = parser.readSchedules(getPrimaryFile(), MODELS).at(0);
		m_model = std::make_unique<Models::FMTLpModel>(MODELS.at(0), solver());
		buildWithSchedule(*m_model, SCHEDULES, m_length);
	#endif
	}

	double OutputsFlow::run()
	{
	#ifdef FMTWITHOSI
		return sumOfOutputs(*m_model, m_length, m_outputs);
	#else
		return 0.0;
	#endif
	}

	// Reads a scenario and simulates p_length periods with the non-spatial simulator, without a
	// solver, its developments moved to the period before p_period, as FMTNsstest does. Returns the
	// value of p_output at p_period.
	class SimulateFlow final : public FlowBenchmark
	{
	public:
		SimulateFlow(std::string p_name, std::string p_primaryFile, std::string p_scenario, int p_length, std::string p_output,
			int p_period);
		double run() override;

	private:
		int m_length;
		std::string m_output;
		int m_period;
		std::size_t m_read;
		std::size_t m_simulate;
	};

	SimulateFlow::SimulateFlow(std::string p_name, std::string p_primaryFile, std::string p_scenario, int p_length,
		std::string p_output, int p_period) :
		FlowBenchmark(std::move(p_name), std::move(p_primaryFile), std::vector<std::string>(1, p_scenario)),
		m_length(p_length),
		m_output(std::move(p_output)),
		m_period(p_period),
		m_read(definePhase("read")),
		m_simulate(definePhase("simulate"))
	{
	}

	double SimulateFlow::run()
	{
	#ifdef FMTWITHOSI
		beginPhases();
		Parser::FMTModelParser parser;
		const std::vector<Models::FMTModel> MODELS = parser.readproject(getPrimaryFile(), getScenarios());
		endPhase(m_read);
		Models::FMTNssModel model(MODELS.at(0), SEED);
		model.setParameter(Models::FMTintmodelparameters::UPDATE, 1);
		std::vector<Core::FMTActualDevelopment> developments;
		for (Core::FMTActualDevelopment development : model.getArea())
		{
			development.setPeriod(m_period - 1);
			developments.push_back(development);
		}
		model.setArea(developments);
		model.setParameter(Models::FMTintmodelparameters::LENGTH, m_length);
		model.setParameter(Models::FMTboolmodelparameters::QUIET_LOGGING, true);
		model.doPlanning(true);
		endPhase(m_simulate);
		return outputValue(model, m_output, m_period);
	#else
		return 0.0;
	#endif
	}

	// Reads the global, stochastic and local scenarios of a replanning, then replans p_replicates
	// replicates of p_periods periods on one thread, as replanner does, writing the selected
	// outputs to CSV files. Returns the number of rows written for the local model, replicates x
	// periods x outputs: the replanned values depend on which optimal solution the solver returns.
	class ReplanningFlow final : public FlowBenchmark
	{
	public:
		ReplanningFlow(std::string p_name, std::string p_primaryFile, std::vector<std::string> p_scenarios, int p_globalLength,
			int p_periods, int p_replicates, std::vector<std::string> p_outputs, std::filesystem::path p_outputFolder);
		Performance::ThreadScope getThreadScope() const override;
		void prepare() override;
		double run() override;

	private:
		int m_globalLength;
		int m_periods;
		int m_replicates;
		std::vector<std::string> m_outputs;
		std::filesystem::path m_outputFolder;
		std::size_t m_read;
		std::size_t m_setup;
		std::size_t m_replanning;
		std::size_t m_result;
	};

	ReplanningFlow::ReplanningFlow(std::string p_name, std::string p_primaryFile, std::vector<std::string> p_scenarios,
		int p_globalLength, int p_periods, int p_replicates, std::vector<std::string> p_outputs,
		std::filesystem::path p_outputFolder) :
		FlowBenchmark(std::move(p_name), std::move(p_primaryFile), std::move(p_scenarios)),
		m_globalLength(p_globalLength),
		m_periods(p_periods),
		m_replicates(p_replicates),
		m_outputs(std::move(p_outputs)),
		m_outputFolder(std::move(p_outputFolder)),
		m_read(definePhase("read")),
		m_setup(definePhase("setup")),
		m_replanning(definePhase("replanning")),
		m_result(definePhase("result"))
	{
	}

	// The task writes into a folder of the results folder, which may not exist yet: it creates the
	// folder of its outputs, not the folders above.
	void ReplanningFlow::prepare()
	{
		FlowBenchmark::prepare();
		std::filesystem::create_directories(m_outputFolder.parent_path());
	}

	// The replicates run in a thread of the task handler.
	Performance::ThreadScope ReplanningFlow::getThreadScope() const
	{
		return Performance::ThreadScope::All;
	}

	double ReplanningFlow::run()
	{
	#ifdef FMTWITHOSI
		beginPhases();
		Parser::FMTModelParser parser;
		const std::vector<Models::FMTModel> MODELS = parser.readproject(getPrimaryFile(), getScenarios());
		endPhase(m_read);
		std::filesystem::remove_all(m_outputFolder);
		Models::FMTLpModel global(MODELS.at(0), solver());
		global.setParameter(Models::FMTintmodelparameters::LENGTH, m_globalLength);
		global.setParameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS, 1);
		global.setParameter(Models::FMTboolmodelparameters::PRESOLVE_CAN_REMOVE_STATIC_THEMES, true);
		global.setParameter(Models::FMTboolmodelparameters::QUIET_LOGGING, true);
		Models::FMTNssModel stochastic(MODELS.at(1), SEED);
		stochastic.setParameter(Models::FMTintmodelparameters::LENGTH, 1);
		stochastic.setParameter(Models::FMTboolmodelparameters::QUIET_LOGGING, true);
		Models::FMTLpModel local(MODELS.at(2), solver());
		local.setParameter(Models::FMTintmodelparameters::LENGTH, 1);
		local.setParameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS, 1);
		local.setParameter(Models::FMTboolmodelparameters::QUIET_LOGGING, true);
		std::vector<Core::FMTOutput> outputs;
		for (const Core::FMTOutput& OUTPUT : global.getOutputs())
		{
			if (std::find(m_outputs.begin(), m_outputs.end(), OUTPUT.getName()) != m_outputs.end())
			{
				outputs.push_back(OUTPUT);
			}
		}
		if (outputs.size() != m_outputs.size())
		{
			throw std::invalid_argument(getName() + ": the global model lacks one of the selected outputs");
		}
		const std::vector<std::string> LAYER_OPTIONS(1, "SEPARATOR=SEMICOLON");
		std::unique_ptr<Parallel::FMTTask> task(new Parallel::FMTReplanningTask(global, stochastic, local, outputs,
			m_outputFolder.string(), "CSV", LAYER_OPTIONS, m_replicates, m_periods, MINIMAL_DRIFT,
			Core::FMToutputlevel::totalonly, false));
		// The handler logs through the quiet logger of the process, set by prepare. Replacing it here would
		// leave the solvers of the models above with the message handler of the destroyed logger.
		Parallel::FMTTaskHandler handler(task, 1);
		endPhase(m_setup);
		handler.conccurentRun();
		endPhase(m_replanning);
		const double ROWS = static_cast<double>(countRows(m_outputFolder / (getScenarios().at(2) + ".csv")));
		endPhase(m_result);
		return ROWS;
	#else
		return 0.0;
	#endif
	}
}

namespace Performance
{
	void addFlowBenchmarks(BenchmarkSuite& p_suite, const std::string& p_primaryFile, const std::filesystem::path& p_workFolder)
	{
		// NOT_MASK, whose objective doplanning and presolvetest check over 5 periods.
		p_suite.add(std::make_unique<OptimizeFlow>("Flow.Optimize", p_primaryFile, "NOT_MASK", 5));
		// The same scenario over 20 periods: a graph twelve times larger.
		p_suite.add(std::make_unique<OptimizeFlow>("Flow.Optimize.Long", p_primaryFile, "NOT_MASK", 20));
		// The schedule of the LP scenario, replayed over its 10 periods.
		p_suite.add(std::make_unique<ReplayFlow>("Flow.Replay", p_primaryFile, "LP", 10, "OVOLREC", 2));
		// Every output of the root of TWD_land over the same 10 periods.
		p_suite.add(std::make_unique<OutputsFlow>("Flow.Outputs", p_primaryFile, "LP", 10, 0));
		// The DECISION scenario, simulated as FMTNsstest checks it.
		p_suite.add(std::make_unique<SimulateFlow>("Flow.Simulate", p_primaryFile, "DECISION", 1, "UNIT_REC", 5));
		// The scenarios of replanningtest, with 2 replicates of 5 periods.
		p_suite.add(std::make_unique<ReplanningFlow>("Flow.Replanning", p_primaryFile,
			std::vector<std::string>{ "Globalreplanning", "Globalfire", "Localreplanning" }, 10, 5, 2,
			std::vector<std::string>{ "OVOLREC", "BURNEDAREA" }, p_workFolder / "Flow.Replanning"));
	}

	std::unique_ptr<Benchmark> makeFlowBenchmark(const std::string& p_name, const std::string& p_arguments,
		const std::filesystem::path& p_workFolder)
	{
		if (Performance::isOfKind(p_name, "Flow.Optimize"))
		{
			const std::vector<std::string> ARGUMENTS = splitArguments(p_name, p_arguments, "<primary file>|<scenario>|<length>");
			return std::make_unique<OptimizeFlow>(p_name, ARGUMENTS.at(0), ARGUMENTS.at(1), positiveArgument(p_name, ARGUMENTS.at(2)));
		}
		if (Performance::isOfKind(p_name, "Flow.Replay"))
		{
			const std::vector<std::string> ARGUMENTS = splitArguments(p_name, p_arguments,
				"<primary file>|<scenario>|<length>|<output>|<period>");
			return std::make_unique<ReplayFlow>(p_name, ARGUMENTS.at(0), ARGUMENTS.at(1), positiveArgument(p_name, ARGUMENTS.at(2)),
				ARGUMENTS.at(3), positiveArgument(p_name, ARGUMENTS.at(4)));
		}
		if (Performance::isOfKind(p_name, "Flow.Outputs"))
		{
			const std::vector<std::string> ARGUMENTS = splitArguments(p_name, p_arguments,
				"<primary file>|<scenario>|<length>|<outputs, a count or all>");
			const std::size_t OUTPUTS = ARGUMENTS.at(3) == "all" ? 0 : static_cast<std::size_t>(positiveArgument(p_name, ARGUMENTS.at(3)));
			return std::make_unique<OutputsFlow>(p_name, ARGUMENTS.at(0), ARGUMENTS.at(1), positiveArgument(p_name, ARGUMENTS.at(2)),
				OUTPUTS);
		}
		if (Performance::isOfKind(p_name, "Flow.Simulate"))
		{
			const std::vector<std::string> ARGUMENTS = splitArguments(p_name, p_arguments,
				"<primary file>|<scenario>|<length>|<output>|<period>");
			return std::make_unique<SimulateFlow>(p_name, ARGUMENTS.at(0), ARGUMENTS.at(1), positiveArgument(p_name, ARGUMENTS.at(2)),
				ARGUMENTS.at(3), positiveArgument(p_name, ARGUMENTS.at(4)));
		}
		if (Performance::isOfKind(p_name, "Flow.Replanning"))
		{
			const std::vector<std::string> ARGUMENTS = splitArguments(p_name, p_arguments,
				"<primary file>|<global scenario>|<stochastic scenario>|<local scenario>|<global length>|<replanned periods>"
				"|<replicates>|<outputs joined by +>");
			return std::make_unique<ReplanningFlow>(p_name, ARGUMENTS.at(0),
				std::vector<std::string>{ ARGUMENTS.at(1), ARGUMENTS.at(2), ARGUMENTS.at(3) }, positiveArgument(p_name, ARGUMENTS.at(4)),
				positiveArgument(p_name, ARGUMENTS.at(5)), positiveArgument(p_name, ARGUMENTS.at(6)), split(ARGUMENTS.at(7), '+'),
				p_workFolder / p_name);
		}
		return nullptr;
	}
}
