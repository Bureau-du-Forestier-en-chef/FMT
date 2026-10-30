/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

// Traversal of the graph of a model by FMTModel::getOutput: the graph selects the vertices of the
// nodes of an output at a period, then sums the area of these vertices, or of their action edges.
// The model is the LP scenario of TWD_land, read and built once with its schedule over 10 periods,
// without a solver, as for Flow.Outputs; each call computes one output at one period. The first
// call of an output fills the node cache of the graph: the .FirstCall variant empties that cache
// before each call, so that each call is the first one.

#include "GraphBenchmarks.h"

#include "Benchmark.h"
#include "BenchmarkSuite.h"
#include "ModelSetup.h"
#include "QuietFmt.h"

#ifdef FMTWITHOSI
	#include "FMTLpModel.h"
	#include "FMTModel.h"
	#include "FMTModelParser.h"
	#include "FMTOutput.h"
	#include "FMTSchedule.h"
#endif

#include <filesystem>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace
{
	const std::string SCENARIO = "LP";
	constexpr int LENGTH = 10;

	// The LP scenario of TWD_land, read once and built with its schedule.
	class ScheduledModel
	{
	public:
		explicit ScheduledModel(std::string p_primaryFile);
		// Reads and builds the model. Only the first call does the work.
		void prepare();
		std::string getDataset() const;
	#ifdef FMTWITHOSI
		Models::FMTLpModel& getModel();
	#endif

	private:
		std::string m_primaryFile;
	#ifdef FMTWITHOSI
		std::unique_ptr<Models::FMTLpModel> m_model;
	#endif
	};

	ScheduledModel::ScheduledModel(std::string p_primaryFile) :
		m_primaryFile(std::move(p_primaryFile))
	{
	}

	void ScheduledModel::prepare()
	{
	#ifdef FMTWITHOSI
		if (m_model)
		{
			return;
		}
		Performance::quietFmt();
		Parser::FMTModelParser parser;
		const std::vector<Models::FMTModel> MODELS = parser.readproject(m_primaryFile, std::vector<std::string>(1, SCENARIO));
		const std::vector<Core::FMTSchedule> SCHEDULES = parser.readSchedules(m_primaryFile, MODELS).at(0);
		m_model = std::make_unique<Models::FMTLpModel>(MODELS.at(0), Performance::getSolver());
		Performance::buildWithSchedule(*m_model, SCHEDULES, LENGTH);
	#endif
	}

	std::string ScheduledModel::getDataset() const
	{
		return std::filesystem::path(m_primaryFile).stem().string() + "/" + SCENARIO;
	}

#ifdef FMTWITHOSI
	Models::FMTLpModel& ScheduledModel::getModel()
	{
		return *m_model;
	}
#endif

	// Computes the total of an output at a period. With p_firstCall, empties the node cache of the
	// graph before each call.
	class GetOutputBenchmark final : public Performance::Benchmark
	{
	public:
		GetOutputBenchmark(std::string p_name, std::shared_ptr<ScheduledModel> p_model, std::string p_output, int p_period,
			bool p_firstCall);
		std::string getName() const override;
		std::string getDataset() const override;
		std::string getUnavailableReason() const override;
		void prepare() override;
		double run() override;

	private:
		std::string m_name;
		std::shared_ptr<ScheduledModel> m_model;
		std::string m_outputName;
		int m_period;
		bool m_firstCall;
	#ifdef FMTWITHOSI
		Core::FMTOutput m_output;
	#endif
	};

	GetOutputBenchmark::GetOutputBenchmark(std::string p_name, std::shared_ptr<ScheduledModel> p_model, std::string p_output,
		int p_period, bool p_firstCall) :
		m_name(std::move(p_name)),
		m_model(std::move(p_model)),
		m_outputName(std::move(p_output)),
		m_period(p_period),
		m_firstCall(p_firstCall)
	{
	}

	std::string GetOutputBenchmark::getName() const
	{
		return m_name;
	}

	std::string GetOutputBenchmark::getDataset() const
	{
		return m_model->getDataset();
	}

	std::string GetOutputBenchmark::getUnavailableReason() const
	{
	#ifdef FMTWITHOSI
		return std::string();
	#else
		return "FMT is built without OSI";
	#endif
	}

	void GetOutputBenchmark::prepare()
	{
		m_model->prepare();
	#ifdef FMTWITHOSI
		for (const Core::FMTOutput& OUTPUT : m_model->getModel().getOutputs())
		{
			if (OUTPUT.getName() == m_outputName)
			{
				m_output = OUTPUT;
				return;
			}
		}
		throw std::invalid_argument(m_name + ": the model has no output " + m_outputName);
	#endif
	}

	double GetOutputBenchmark::run()
	{
	#ifdef FMTWITHOSI
		Models::FMTLpModel& model = m_model->getModel();
		if (m_firstCall)
		{
			model.clearGraphCache();
		}
		return model.getOutput(m_output, m_period, Core::FMToutputlevel::totalonly).at("Total");
	#else
		return 0.0;
	#endif
	}
}

namespace Performance
{
	void addGraphBenchmarks(BenchmarkSuite& p_suite, const std::string& p_primaryFile)
	{
		const std::shared_ptr<ScheduledModel> MODEL = std::make_shared<ScheduledModel>(p_primaryFile);
		// totalarea, ? ? ? _INVENT _AREA: the vertices of the last period, whose area is that of the
		// root, 1814.76 ha.
		p_suite.add(std::make_unique<GetOutputBenchmark>("Graph.GetOutput.Inventory", MODEL, "TOTALAREA", LENGTH, false));
		p_suite.add(std::make_unique<GetOutputBenchmark>("Graph.GetOutput.Inventory.FirstCall", MODEL, "TOTALAREA", LENGTH, true));
		// oSupRec, ? ? ? coupetotale _AREA: the COUPETOTALE edges of period 1, 201.64 + 50.107368 ha in
		// the schedule of LP.
		p_suite.add(std::make_unique<GetOutputBenchmark>("Graph.GetOutput.Action", MODEL, "OSUPREC", 1, false));
	}
}
