/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

// Operations on Core::FMTMask that every step of a model repeats: testing whether a development
// belongs to a selection, which must not allocate, and building a mask from its text, as the
// parser does for every row of a model.

#include "MaskBenchmarks.h"

#include "FMTMask.h"
#include "FMTModel.h"
#include "FMTModelParser.h"
#include "FMTTheme.h"

#include "Benchmark.h"
#include "BenchmarkSuite.h"

#include <filesystem>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace
{
	// A development of TWD_land, and a selection that holds it: the aggregate UC (unite1 and
	// unite2), the aggregate PROD (peuplement1 to peuplement3) and every attribute of the third
	// theme. The mask of the selection sets 2 + 3 + 3 = 8 bits (TWD_land.lan).
	const std::string DEVELOPMENT = "UNITE1 PEUPLEMENT1 UTR1";
	const std::string SELECTION = "UC PROD ?";

	// The themes of the root of TWD_land, read once for the mask benchmarks.
	class LandscapeThemes
	{
	public:
		explicit LandscapeThemes(std::string p_primaryFile);
		// Reads the themes. Only the first call does the work.
		void prepare();
		const std::vector<Core::FMTTheme>& getThemes() const;
		std::string getDataset() const;

	private:
		std::string m_primaryFile;
		std::vector<Core::FMTTheme> m_themes;
		bool m_prepared = false;
	};

	LandscapeThemes::LandscapeThemes(std::string p_primaryFile) :
		m_primaryFile(std::move(p_primaryFile))
	{
	}

	void LandscapeThemes::prepare()
	{
		if (m_prepared)
		{
			return;
		}
		Parser::FMTModelParser parser;
		parser.setDefaultExceptionHandler();
		const std::vector<Models::FMTModel> MODELS = parser.readproject(m_primaryFile, std::vector<std::string>(1, "ROOT"));
		m_themes = MODELS.at(0).getThemes();
		m_prepared = true;
	}

	const std::vector<Core::FMTTheme>& LandscapeThemes::getThemes() const
	{
		return m_themes;
	}

	std::string LandscapeThemes::getDataset() const
	{
		return std::filesystem::path(m_primaryFile).stem().string() + "/ROOT";
	}

	// Tests whether the development belongs to the selection: 1 when it does.
	class MaskSubsetBenchmark final : public Performance::Benchmark
	{
	public:
		explicit MaskSubsetBenchmark(std::shared_ptr<LandscapeThemes> p_themes);
		std::string getName() const override;
		std::string getDataset() const override;
		void prepare() override;
		double run() override;

	private:
		std::shared_ptr<LandscapeThemes> m_themes;
		Core::FMTMask m_development;
		Core::FMTMask m_selection;
	};

	MaskSubsetBenchmark::MaskSubsetBenchmark(std::shared_ptr<LandscapeThemes> p_themes) :
		m_themes(std::move(p_themes))
	{
	}

	std::string MaskSubsetBenchmark::getName() const
	{
		return "Mask.IsSubsetOf";
	}

	std::string MaskSubsetBenchmark::getDataset() const
	{
		return m_themes->getDataset();
	}

	void MaskSubsetBenchmark::prepare()
	{
		m_themes->prepare();
		m_development = Core::FMTMask(DEVELOPMENT, m_themes->getThemes());
		m_selection = Core::FMTMask(SELECTION, m_themes->getThemes());
	}

	double MaskSubsetBenchmark::run()
	{
		return m_development.isSubsetOf(m_selection) ? 1.0 : 0.0;
	}

	// Builds the mask of the selection from its text, and returns the number of bits it sets.
	class MaskFromStringBenchmark final : public Performance::Benchmark
	{
	public:
		explicit MaskFromStringBenchmark(std::shared_ptr<LandscapeThemes> p_themes);
		std::string getName() const override;
		std::string getDataset() const override;
		void prepare() override;
		double run() override;

	private:
		std::shared_ptr<LandscapeThemes> m_themes;
	};

	MaskFromStringBenchmark::MaskFromStringBenchmark(std::shared_ptr<LandscapeThemes> p_themes) :
		m_themes(std::move(p_themes))
	{
	}

	std::string MaskFromStringBenchmark::getName() const
	{
		return "Mask.FromString";
	}

	std::string MaskFromStringBenchmark::getDataset() const
	{
		return m_themes->getDataset();
	}

	void MaskFromStringBenchmark::prepare()
	{
		m_themes->prepare();
	}

	double MaskFromStringBenchmark::run()
	{
		const Core::FMTMask MASK(SELECTION, m_themes->getThemes());
		return static_cast<double>(MASK.count());
	}
}

namespace Performance
{
	void addMaskBenchmarks(BenchmarkSuite& p_suite, const std::string& p_primaryFile)
	{
		const std::shared_ptr<LandscapeThemes> THEMES = std::make_shared<LandscapeThemes>(p_primaryFile);
		p_suite.add(std::make_unique<MaskSubsetBenchmark>(THEMES));
		p_suite.add(std::make_unique<MaskFromStringBenchmark>(THEMES));
	}
}
