/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

// Operations on Core::FMTMask that every step of a model repeats: testing whether a development
// belongs to a selection, comparing and hashing masks, which must not allocate, building a mask
// from its text, as the parser does for every row of a model, and finding the sections of a list
// whose mask holds a development, as FMT does for the actions, the transitions and the yields of
// every development.

#include "MaskBenchmarks.h"

#include "FMTMask.h"
#include "FMTModel.h"
#include "FMTModelParser.h"
#include "FMTTheme.h"
#include "FMTYieldHandler.h"
#include "FMTYields.h"

#include "Benchmark.h"
#include "BenchmarkSuite.h"

#include <cstddef>
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

	// The themes and the yields of the root of TWD_land, read once for the mask benchmarks.
	class LandscapeRoot
	{
	public:
		explicit LandscapeRoot(std::string p_primaryFile);
		// Reads the root. Only the first call does the work.
		void prepare();
		const std::vector<Core::FMTTheme>& getThemes() const;
		const Core::FMTYields& getYields() const;
		std::string getDataset() const;

	private:
		std::string m_primaryFile;
		std::vector<Core::FMTTheme> m_themes;
		Core::FMTYields m_yields;
		bool m_prepared = false;
	};

	LandscapeRoot::LandscapeRoot(std::string p_primaryFile) :
		m_primaryFile(std::move(p_primaryFile))
	{
	}

	void LandscapeRoot::prepare()
	{
		if (m_prepared)
		{
			return;
		}
		Parser::FMTModelParser parser;
		parser.setDefaultExceptionHandler();
		const std::vector<Models::FMTModel> MODELS = parser.readproject(m_primaryFile, std::vector<std::string>(1, "ROOT"));
		m_themes = MODELS.at(0).getThemes();
		m_yields = MODELS.at(0).getYields();
		m_prepared = true;
	}

	const std::vector<Core::FMTTheme>& LandscapeRoot::getThemes() const
	{
		return m_themes;
	}

	const Core::FMTYields& LandscapeRoot::getYields() const
	{
		return m_yields;
	}

	std::string LandscapeRoot::getDataset() const
	{
		return std::filesystem::path(m_primaryFile).stem().string() + "/ROOT";
	}

	// Tests whether the development belongs to the selection: 1 when it does.
	class MaskSubsetBenchmark final : public Performance::Benchmark
	{
	public:
		explicit MaskSubsetBenchmark(std::shared_ptr<LandscapeRoot> p_root);
		std::string getName() const override;
		std::string getDataset() const override;
		void prepare() override;
		double run() override;

	private:
		std::shared_ptr<LandscapeRoot> m_root;
		Core::FMTMask m_development;
		Core::FMTMask m_selection;
	};

	MaskSubsetBenchmark::MaskSubsetBenchmark(std::shared_ptr<LandscapeRoot> p_root) :
		m_root(std::move(p_root))
	{
	}

	std::string MaskSubsetBenchmark::getName() const
	{
		return "Mask.IsSubsetOf";
	}

	std::string MaskSubsetBenchmark::getDataset() const
	{
		return m_root->getDataset();
	}

	void MaskSubsetBenchmark::prepare()
	{
		m_root->prepare();
		m_development = Core::FMTMask(DEVELOPMENT, m_root->getThemes());
		m_selection = Core::FMTMask(SELECTION, m_root->getThemes());
	}

	double MaskSubsetBenchmark::run()
	{
		return m_development.isSubsetOf(m_selection) ? 1.0 : 0.0;
	}

	// The comparisons of two masks that the graph and the caches of FMT make.
	enum class MaskComparison
	{
		// operator== of the development and of the same development built apart: 1.
		Equals,
		// operator< of the development and of the selection, both ways: exactly one of two different
		// masks is less than the other, 1.
		Less,
		// FMTMask::hash, the hash of the keys of the caches of FMTList, of the development and of the
		// same development built apart: equal, 1.
		Hash,
		// isSubsetOf on the blocks of the selection that are not full, as the graph selects the
		// vertices of a static node: the development belongs to the selection, 1.
		IsSubsetOfBlocks
	};

	class MaskComparisonBenchmark final : public Performance::Benchmark
	{
	public:
		MaskComparisonBenchmark(std::shared_ptr<LandscapeRoot> p_root, MaskComparison p_comparison);
		std::string getName() const override;
		std::string getDataset() const override;
		void prepare() override;
		double run() override;

	private:
		std::shared_ptr<LandscapeRoot> m_root;
		MaskComparison m_comparison;
		Core::FMTMask m_development;
		Core::FMTMask m_sameDevelopment;
		Core::FMTMask m_selection;
		std::vector<std::size_t> m_selectionBlocks;
	};

	MaskComparisonBenchmark::MaskComparisonBenchmark(std::shared_ptr<LandscapeRoot> p_root, MaskComparison p_comparison) :
		m_root(std::move(p_root)),
		m_comparison(p_comparison)
	{
	}

	std::string MaskComparisonBenchmark::getName() const
	{
		switch (m_comparison)
		{
		case MaskComparison::Equals:
			return "Mask.Equals";
		case MaskComparison::Less:
			return "Mask.Less";
		case MaskComparison::Hash:
			return "Mask.Hash";
		default:
			return "Mask.IsSubsetOfBlocks";
		}
	}

	std::string MaskComparisonBenchmark::getDataset() const
	{
		return m_root->getDataset();
	}

	void MaskComparisonBenchmark::prepare()
	{
		m_root->prepare();
		m_development = Core::FMTMask(DEVELOPMENT, m_root->getThemes());
		m_sameDevelopment = Core::FMTMask(DEVELOPMENT, m_root->getThemes());
		m_selection = Core::FMTMask(SELECTION, m_root->getThemes());
		m_selectionBlocks = m_selection.getNonFullBlocks();
	}

	double MaskComparisonBenchmark::run()
	{
		switch (m_comparison)
		{
		case MaskComparison::Equals:
			return m_development == m_sameDevelopment ? 1.0 : 0.0;
		case MaskComparison::Less:
			return static_cast<double>(m_development < m_selection) + static_cast<double>(m_selection < m_development);
		case MaskComparison::Hash:
			return m_development.hash() == m_sameDevelopment.hash() ? 1.0 : 0.0;
		default:
			return m_development.isSubsetOf(m_selection, m_selectionBlocks) ? 1.0 : 0.0;
		}
	}

	// Builds the mask of the selection from its text, and returns the number of bits it sets.
	class MaskFromStringBenchmark final : public Performance::Benchmark
	{
	public:
		explicit MaskFromStringBenchmark(std::shared_ptr<LandscapeRoot> p_root);
		std::string getName() const override;
		std::string getDataset() const override;
		void prepare() override;
		double run() override;

	private:
		std::shared_ptr<LandscapeRoot> m_root;
	};

	MaskFromStringBenchmark::MaskFromStringBenchmark(std::shared_ptr<LandscapeRoot> p_root) :
		m_root(std::move(p_root))
	{
	}

	std::string MaskFromStringBenchmark::getName() const
	{
		return "Mask.FromString";
	}

	std::string MaskFromStringBenchmark::getDataset() const
	{
		return m_root->getDataset();
	}

	void MaskFromStringBenchmark::prepare()
	{
		m_root->prepare();
	}

	double MaskFromStringBenchmark::run()
	{
		const Core::FMTMask MASK(SELECTION, m_root->getThemes());
		return static_cast<double>(MASK.count());
	}

	// Finds the yield sections of the root whose mask holds the development, by FMTList::findSets,
	// and returns their number: ? PEUPLEMENT1 ?, the complex section ? ? ? and the last section ? ? ?
	// of TWD_land.yld, and the section ? ? ? of the _TSLA yield that FMTYields::generateDefaultYields
	// adds to every model read, 4. FMTList keeps the sections found for a mask in a cache, filled by
	// the first call for that mask. With p_firstCall, the benchmark empties that cache before each
	// call, so that each call is the first one.
	class MaskFindSetsBenchmark final : public Performance::Benchmark
	{
	public:
		MaskFindSetsBenchmark(std::shared_ptr<LandscapeRoot> p_root, bool p_firstCall);
		std::string getName() const override;
		std::string getDataset() const override;
		void prepare() override;
		double run() override;

	private:
		std::shared_ptr<LandscapeRoot> m_root;
		bool m_firstCall;
		Core::FMTYields m_yields;
		Core::FMTMask m_development;
	};

	MaskFindSetsBenchmark::MaskFindSetsBenchmark(std::shared_ptr<LandscapeRoot> p_root, bool p_firstCall) :
		m_root(std::move(p_root)),
		m_firstCall(p_firstCall)
	{
	}

	std::string MaskFindSetsBenchmark::getName() const
	{
		return m_firstCall ? "Mask.FindSets.FirstCall" : "Mask.FindSets";
	}

	std::string MaskFindSetsBenchmark::getDataset() const
	{
		return m_root->getDataset();
	}

	void MaskFindSetsBenchmark::prepare()
	{
		m_root->prepare();
		m_yields = m_root->getYields();
		m_development = Core::FMTMask(DEVELOPMENT, m_root->getThemes());
	}

	double MaskFindSetsBenchmark::run()
	{
		if (m_firstCall)
		{
			// Only the cache of the list: FMTYields::clearCache also empties the caches of its handlers.
			m_yields.Core::FMTList<std::unique_ptr<Core::FMTYieldHandler>>::clearCache();
		}
		return static_cast<double>(m_yields.findSets(m_development).size());
	}
}

namespace Performance
{
	void addMaskBenchmarks(BenchmarkSuite& p_suite, const std::string& p_primaryFile)
	{
		const std::shared_ptr<LandscapeRoot> ROOT = std::make_shared<LandscapeRoot>(p_primaryFile);
		p_suite.add(std::make_unique<MaskSubsetBenchmark>(ROOT));
		p_suite.add(std::make_unique<MaskFromStringBenchmark>(ROOT));
		for (const MaskComparison COMPARISON : { MaskComparison::Equals, MaskComparison::Less, MaskComparison::Hash,
			MaskComparison::IsSubsetOfBlocks })
		{
			p_suite.add(std::make_unique<MaskComparisonBenchmark>(ROOT, COMPARISON));
		}
		p_suite.add(std::make_unique<MaskFindSetsBenchmark>(ROOT, false));
		p_suite.add(std::make_unique<MaskFindSetsBenchmark>(ROOT, true));
	}
}
