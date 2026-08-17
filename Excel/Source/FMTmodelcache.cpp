#include "stdafx.h"
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include "FMTLpModel.h"
#include "FMTOutput.h"
#include "FMTTheme.h"
#include "FMTMask.h"
#include "FMTSchedule.h"
#include "FMTExceptionHandler.h"
#include "FMTException.h"
#include "FMTExcelLogger.h"
#include "FMTModelCache.h"
#include "FMTException.h"
#include "FMTObject.h"
#include <boost/algorithm/string.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread.hpp>
#include "FMTForest.h"
#include "FMTAreaParser.h"
#include "FMTOperatingAreaScheme.h"
#include "FMTOperatingArea.h"
#include "FMTGraphStats.h"
#include "FMTExcelExceptionHandler.h"
#include <map>
#include "FMTSerie.h"
#include <thread>
#include <mutex>
#include <sstream> 

namespace Wrapper
{

	FMTModelCache::FMTModelCache() :
		Models::FMTLpModel(),
		m_cachingSwitch(false),
		m_mutex(new boost::recursive_mutex()),
		m_outputsMap(),
		m_themesMap(),
		m_maskCache(),
		m_outputCache(),
		m_mapLocation(),
		m_map(),
		m_generalCache(),
		m_serieCache(),
		m_globalMask(),
		m_maskCacheMutex(new boost::recursive_mutex()),
		m_outputCacheMutex(new boost::recursive_mutex()),
		m_generalCacheMutex(new boost::recursive_mutex()),
		m_serieCacheMutex(new boost::recursive_mutex()),
		m_operatingAreaCache(
			new std::vector<Heuristics::FMTOperatingArea>()),
		m_buildExceptions()
	{
	}

	FMTModelCache::~FMTModelCache() = default;

	FMTModelCache& FMTModelCache::operator=(
		const FMTModelCache& rhs)
	{
		if (this != &rhs)
		{
			m_cachingSwitch = rhs.m_cachingSwitch;

			boost::lock(
				*m_mutex,
				*rhs.m_mutex);

			boost::lock_guard<boost::recursive_mutex>
				self_lock(
					*m_mutex,
					boost::adopt_lock);

			boost::lock_guard<boost::recursive_mutex>
				other_lock(
					*rhs.m_mutex,
					boost::adopt_lock);

			Core::FMTObject::operator=(rhs);

			m_outputsMap = rhs.m_outputsMap;
			m_themesMap = rhs.m_themesMap;
			m_maskCache = rhs.m_maskCache;
			m_outputCache = rhs.m_outputCache;
			m_mapLocation = rhs.m_mapLocation;
			m_generalCache = rhs.m_generalCache;
			m_serieCache = rhs.m_serieCache;

			Models::FMTLpModel::operator=(rhs);

			m_globalMask = rhs.m_globalMask;

			m_operatingAreaCache =
				std::move(
					std::unique_ptr<
					std::vector<Heuristics::FMTOperatingArea>>(
						new std::vector<
						Heuristics::FMTOperatingArea>(
							*rhs.m_operatingAreaCache)));

			m_buildExceptions =
				rhs.m_buildExceptions;

			if (rhs.m_map)
			{
				m_map =
					std::move(
						std::unique_ptr<Spatial::FMTForest>(
							new Spatial::FMTForest(
								*rhs.m_map)));
			}
		}

		return *this;
	}

	FMTModelCache::FMTModelCache(
		const FMTModelCache& rhs) :
		Models::FMTLpModel(rhs),
		m_cachingSwitch(rhs.m_cachingSwitch),
		m_mutex(new boost::recursive_mutex()),
		m_outputsMap(rhs.m_outputsMap),
		m_themesMap(rhs.m_themesMap),
		m_maskCache(rhs.m_maskCache),
		m_outputCache(rhs.m_outputCache),
		m_mapLocation(rhs.m_mapLocation),
		m_map(),
		m_generalCache(rhs.m_generalCache),
		m_serieCache(rhs.m_serieCache),
		m_globalMask(rhs.m_globalMask),
		m_maskCacheMutex(new boost::recursive_mutex()),
		m_outputCacheMutex(new boost::recursive_mutex()),
		m_generalCacheMutex(new boost::recursive_mutex()),
		m_serieCacheMutex(new boost::recursive_mutex()),
		m_operatingAreaCache(),
		m_buildExceptions(rhs.m_buildExceptions)
	{
		boost::lock_guard<boost::recursive_mutex>
			guard1(*m_mutex);

		boost::lock_guard<boost::recursive_mutex>
			guard2(*rhs.m_mutex);

		if (rhs.m_map)
		{
			m_map =
				std::move(
					std::unique_ptr<Spatial::FMTForest>(
						new Spatial::FMTForest(
							*rhs.m_map)));
		}

		m_operatingAreaCache =
			std::move(
				std::unique_ptr<
				std::vector<Heuristics::FMTOperatingArea>>(
					new std::vector<
					Heuristics::FMTOperatingArea>(
						*rhs.m_operatingAreaCache)));
	}

	void FMTModelCache::_allocateResources()
	{
		try
		{
			if (m_cachingSwitch &&
				this->getGraphSize() > 0 &&
				m_outputCache.empty() &&
				m_generalCache.empty() &&
				m_maskCache.empty())
			{
				const size_t nperiods =
					static_cast<size_t>(
						getperiods());

				size_t maxsizet = 0;

				for (const Core::FMTTheme& theme : themes)
				{
					maxsizet =
						std::max(
							maxsizet,
							theme.size());
				}

				maxsizet *= themes.size();

				m_outputCache.reserve(
					outputs.size() * maxsizet);

				m_generalCache.reserve(
					outputs.size() *
					nperiods *
					maxsizet);

				m_maskCache.reserve(
					maxsizet);
			}
		}
		catch (...)
		{
			_exhandler->printExceptions(
				"",
				"FMTModelCache::_allocateResources",
				__LINE__,
				__FILE__);
		}
	}

	void FMTModelCache::_loadMap() const
	{
		try
		{
			boost::lock_guard<boost::recursive_mutex>
				guard(*m_mutex);

			if (!m_map &&
				!m_mapLocation.empty())
			{
				Parser::FMTAreaParser areaparser;

				m_map =
					std::unique_ptr<Spatial::FMTForest>(
						new Spatial::FMTForest(
							areaparser.vectormaptoFMTforest(
								m_mapLocation,
								250,
								themes,
								"AGE",
								"SUPERFICIE",
								1.0,
								1.0,
								"STANLOCK")));
			}
		}
		catch (...)
		{
			_exhandler->printExceptions(
				"",
				"FMTModelCache::_loadMap",
				__LINE__,
				__FILE__);
		}
	}

	void FMTModelCache::_setBaseResources()
	{
		try
		{
			size_t outid = 0;

			for (const Core::FMTOutput& output : outputs)
			{
				m_outputsMap[
					output.getName()] = outid;

				++outid;
			}

			size_t theid = 0;

			for (const Core::FMTTheme& theme : themes)
			{
				m_themesMap[
					theme.getName()] = theid;

				++theid;
			}

			std::string mask;

			for (size_t thid = 0;
				thid < themes.size();
				++thid)
			{
				mask += "? ";
			}

			mask.pop_back();

			m_globalMask =
				Core::FMTMask(
					mask,
					themes);

			Models::FMTModel::setParameter(
				Models::FMTdblmodelparameters::TOLERANCE,
				0.001);

			m_cachingSwitch = true;
		}
		catch (...)
		{
			_exhandler->printExceptions(
				"",
				"FMTModelCache::_setBaseResources",
				__LINE__,
				__FILE__);
		}
	}

	FMTModelCache::FMTModelCache(
		const Models::FMTModel& lmodel,
		const std::string& lmaplocation) :
		Models::FMTLpModel(
			lmodel,
			Models::FMTSolverInterface::MOSEK),
		m_cachingSwitch(false),
		m_mutex(new boost::recursive_mutex()),
		m_outputsMap(),
		m_themesMap(),
		m_maskCache(),
		m_outputCache(),
		m_mapLocation(lmaplocation),
		m_map(),
		m_generalCache(),
		m_serieCache(),
		m_globalMask(),
		m_maskCacheMutex(new boost::recursive_mutex()),
		m_outputCacheMutex(new boost::recursive_mutex()),
		m_generalCacheMutex(new boost::recursive_mutex()),
		m_serieCacheMutex(new boost::recursive_mutex()),
		m_operatingAreaCache(
			new std::vector<Heuristics::FMTOperatingArea>()),
		m_buildExceptions()
	{
		try
		{
			boost::lock_guard<boost::recursive_mutex>
				guard(*m_mutex);

			_setBaseResources();
		}
		catch (...)
		{
			_exhandler->printExceptions(
				"",
				"FMTModelCache::FMTModelCache",
				__LINE__,
				__FILE__);
		}
	}

	void FMTModelCache::setLength(
		const int& period)
	{
		try
		{
			setParameter(
				Models::FMTintmodelparameters::LENGTH,
				period);
		}
		catch (...)
		{
			_exhandler->printExceptions(
				"",
				"FMTModelCache::setLength",
				__LINE__,
				__FILE__);
		}
	}

	void FMTModelCache::setSolution(
		const std::vector<Core::FMTSchedule>& schedules)
	{
		try
		{
			setParameter(
				Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD,
				true);

			Exception::FMTExcelExceptionHandler* excelhandler =
				dynamic_cast<Exception::FMTExcelExceptionHandler*>(
					_exhandler.get());

			excelhandler->resetBuildExceptions();

			doPlanning(
				false,
				schedules);

			m_buildExceptions =
				excelhandler->getBuildExceptions();
		}
		catch (...)
		{
			_exhandler->printExceptions(
				"",
				"FMTModelCache::setSolution",
				__LINE__,
				__FILE__);
		}
	}

	bool FMTModelCache::buildnsolve(
		bool solve)
	{
		try
		{
			Exception::FMTExcelExceptionHandler* excelhandler =
				dynamic_cast<Exception::FMTExcelExceptionHandler*>(
					_exhandler.get());

			excelhandler->resetBuildExceptions();

			const bool optimal =
				doPlanning(solve);

			m_buildExceptions =
				excelhandler->getBuildExceptions();

			return (!solve || optimal);
		}
		catch (...)
		{
			_exhandler->printExceptions(
				"",
				"FMTModelCache::buildnsolve",
				__LINE__,
				__FILE__);
		}

		return false;
	}

	int FMTModelCache::getperiods() const
	{
		return getParameter(
			Models::FMTintmodelparameters::LENGTH);
	}

	bool FMTModelCache::getfrommaskcache(
		const std::string& cachekey,
		Core::FMTMask& mask) const
	{
		try
		{
			if (m_cachingSwitch)
			{
				boost::lock_guard<boost::recursive_mutex>
					guard(*m_maskCacheMutex);

				std::unordered_map<
					std::string,
					Core::FMTMask>::iterator mskit =
					m_maskCache.find(cachekey);

				if (mskit != m_maskCache.end())
				{
					mask = mskit->second;
					return true;
				}
			}
		}
		catch (...)
		{
			_exhandler->printExceptions(
				"",
				"FMTModelCache::getfrommaskcache",
				__LINE__,
				__FILE__);
		}

		mask = m_globalMask;
		return false;
	}

	void FMTModelCache::writetomaskcache(
		const std::string& cachekey,
		const Core::FMTMask& mask) const
	{
		try
		{
			if (m_cachingSwitch)
			{
				boost::lock_guard<boost::recursive_mutex>
					guard(*m_maskCacheMutex);

				m_maskCache[cachekey] = mask;
			}
		}
		catch (...)
		{
			_exhandler->printExceptions(
				"",
				"FMTModelCache::writetomaskcache",
				__LINE__,
				__FILE__);
		}
	}

	Core::FMTMask FMTModelCache::_themeSelectionToMask(
		const std::string& p_themeSelection) const
	{
		try
		{
			Core::FMTMask subset;

			if (getfrommaskcache(
				p_themeSelection,
				subset))
			{
				return subset;
			}

			if (p_themeSelection.find('=') !=
				std::string::npos)
			{
				std::vector<std::string> results;

				boost::split(
					results,
					p_themeSelection,
					boost::is_any_of(";"));

				std::string emptyMask;
				std::vector<std::string> composition;

				for (const Core::FMTTheme& theme : themes)
				{
					emptyMask += "? ";
					composition.push_back("?");
				}

				emptyMask.pop_back();

				subset =
					Core::FMTMask(
						emptyMask,
						themes);
				for (std::string& value : results)
				{
					boost::trim(value);
					size_t stfind = value.find('=');
					if (stfind != std::string::npos)//need to subsettheme!
					{
						const std::string themename = value.substr(0, stfind);
						Core::FMTTheme const* localtheme = nullptr;
						if (themename.find("THEME") != std::string::npos)
						{
							const size_t themeid = static_cast<size_t>(std::stoi(themename.substr(themename.find("THEME") + 5)) - 1);
							if (!(themeid < themes.size()))
							{
								return Core::FMTMask();
							}
							localtheme = &themes.at(themeid);
						}
						else {
							if (m_themesMap.find(themename) == m_themesMap.end())
							{
								return Core::FMTMask();
							}
							localtheme = &themes.at(m_themesMap.at(themename));
						}
						std::string data = value.substr(stfind + 1, value.size());
						boost::trim(data);
						std::vector<std::string>attributes;
						if (!data.empty() && data.at(0) == '{' && data.back() == '}')
						{
							data.pop_back();
							data.erase(data.begin());
							boost::split(attributes, data, boost::is_any_of(","));
						}
						else {
							attributes.push_back(data);
						}

						composition[std::distance(&*themes.begin(), localtheme)] = data;

						for (std::string& attribute : attributes)
						{
							boost::trim(attribute);
							Core::FMTMask localMask(subset);

							if (localtheme)
							{
								if (localtheme->isAttribute(attribute) ||
									localtheme->isAggregate(attribute) ||
									attribute == "?")
								{
									localMask.set(*localtheme, attribute);
								}
								//attributes names is a vector of string that the string can be empty ... so we cannot search in it
								else if (!attribute.empty()) {
									const std::vector<std::string>& attributenames = localtheme->getAttributeNames();
									std::vector<std::string>::const_iterator ait = std::find(attributenames.begin(), attributenames.end(), attribute);
									if (ait == attributenames.end())//raise
									{
										return Core::FMTMask();
									}
									else {
										const std::string attname = localtheme->getBaseAttributes().at(std::distance(attributenames.begin(), ait));
										localMask.set(*localtheme, attname);

									}
								}
								else {
									return Core::FMTMask();
								}
							}
							if (attributes.size() == 1 ||
								*attributes.begin() == attribute)
							{
								subset = localMask;
							}
							else {
								subset = subset.getUnion(localMask);
								std::string final_value;
								for (const std::string& VAL : composition)
								{
									final_value += VAL + " ";
								}
								final_value.pop_back();
								subset = Core::FMTMask(final_value, subset.getBitsetReference());
							}

						}
					}
				}

				writetomaskcache(p_themeSelection, subset);
				return subset;
			}
		}
		catch (...)
		{
			_exhandler->printExceptions(
				"",
				"FMTModelCache::_themeSelectionToMask",
				__LINE__,
				__FILE__);
		}

		return Core::FMTMask();
	}

	Core::FMTOutput FMTModelCache::getOutput(
		const std::string& outputname,
		const Core::FMTMask& subset) const
	{
		try
		{
			std::unordered_map<std::string, size_t>::const_iterator
				outit = m_outputsMap.find(outputname);

			if (!outputs.empty() &&
				outit != m_outputsMap.end())
			{
				// Complete model output
				if (subset.size() == subset.count() ||
					subset.empty())
				{
					return outputs.at(outit->second);
				}
				else
				{
					Core::FMTOutput output;

					const std::string cachekey =
						outputname + "_" +
						std::string(subset);

					if (!getfromoutputcache(
						cachekey,
						output))
					{
						output =
							outputs.at(outit->second)
							.intersectWithMask(
								subset,
								themes);

						writetooutputcache(
							cachekey,
							output);
					}

					return output;
				}
			}
		}
		catch (...)
		{
			_exhandler->printExceptions(
				"",
				"FMTModelCache::getOutput",
				__LINE__,
				__FILE__);
		}

		return Core::FMTOutput();
	}

	double FMTModelCache::getvaluefrommodel(
		const Core::FMTOutput& output,
		const int& period) const
	{
		double value = 0.0;

		try
		{
			boost::lock_guard<boost::recursive_mutex>
				guard(*m_mutex);

			value =
				Models::FMTLpModel::getOutput(
					output,
					period,
					Core::FMToutputlevel::totalonly)
				.at("Total");
		}
		catch (...)
		{
			_exhandler->printExceptions(
				"",
				"FMTModelCache::getvaluefrommodel",
				__LINE__,
				__FILE__);
		}

		return value;
	}

	bool FMTModelCache::getfromoutputcache(
		const std::string& cachekey,
		Core::FMTOutput& output) const
	{
		bool incache = false;

		try
		{
			if (m_cachingSwitch)
			{
				boost::lock_guard<boost::recursive_mutex>
					guard(*m_outputCacheMutex);

				std::unordered_map<
					std::string,
					Core::FMTOutput>::iterator outit =
					m_outputCache.find(cachekey);

				if (outit != m_outputCache.end())
				{
					incache = true;
					output = outit->second;
				}
			}
		}
		catch (...)
		{
			_exhandler->printExceptions(
				"",
				"FMTModelCache::getfromoutputcache",
				__LINE__,
				__FILE__);
		}

		return incache;
	}

	void FMTModelCache::writetooutputcache(
		const std::string& cachekey,
		const Core::FMTOutput& output) const
	{
		try
		{
			if (m_cachingSwitch)
			{
				boost::lock_guard<boost::recursive_mutex>
					guard(*m_outputCacheMutex);

				m_outputCache[cachekey] = output;
			}
		}
		catch (...)
		{
			_exhandler->printExceptions(
				"",
				"FMTModelCache::writetooutputcache",
				__LINE__,
				__FILE__);
		}
	}

	std::vector<std::string>
		FMTModelCache::getnoaction(
			const std::string& filter) const
	{
		std::vector<std::string> masks;

		try
		{
			const Core::FMTMask mask_filter =
				_themeSelectionToMask(filter);

			if (!mask_filter.empty())
			{
				for (const Core::FMTDevelopment* dev :
					this->getNoChoice(mask_filter))
				{
					masks.push_back(
						std::string(*dev));
				}
			}
		}
		catch (...)
		{
			_exhandler->printExceptions(
				"",
				"FMTModelCache::getnoaction",
				__LINE__,
				__FILE__);
		}

		return masks;
	}

	std::vector<double> FMTModelCache::Juxtaposition(
		const std::vector<std::string>& themeselection,
		const std::string& yieldname,
		const std::string& output,
		const double& ratio,
		const double& perimeters) const
	{
		std::vector<double> allratios;

		try
		{
			if (!m_mapLocation.empty())
			{
				std::vector<Heuristics::FMTOperatingAreaScheme>
					allscheme;

				std::vector<size_t> allmasks;

				boost::unordered_map<
					Core::FMTMask,
					size_t> masklocation;

				size_t idofit = 0;

				std::vector<std::string> selected;

				for (const std::string& thselection :
					themeselection)
				{
					const Core::FMTMask subset =
						_themeSelectionToMask(
							thselection);

					if (!subset.empty())
					{
						std::vector<
							Heuristics::FMTOperatingArea>
							::const_iterator itof =
							std::find_if(
								m_operatingAreaCache->begin(),
								m_operatingAreaCache->end(),
								Heuristics::
								FMTOperatingAreaComparator(
									subset));

						if (itof ==
							m_operatingAreaCache->end())
						{
							allscheme.push_back(
								Heuristics::
								FMTOperatingAreaScheme(
									Heuristics::
									FMTOperatingArea(
										subset,
										perimeters),
									2,
									6,
									6,
									1,
									1,
									1));
						}
						else
						{
							const size_t location =
								std::distance(
									m_operatingAreaCache->cbegin(),
									itof);

							allmasks.push_back(
								location);

							masklocation[subset] =
								location;
						}

						selected.push_back(
							thselection);

						++idofit;
					}
				}

				if (!allscheme.empty())
				{
					Parser::FMTAreaParser areaparser;

					const std::vector<
						Heuristics::
						FMTOperatingAreaScheme> Ioop =
						areaparser.getSchemeNeighbors(
							allscheme,
							themes,
							m_mapLocation,
							"AGE",
							"SUPERFICIE",
							1.0,
							1.0,
							"STANLOCK");

					for (const auto& scheme : Ioop)
					{
						allmasks.push_back(
							m_operatingAreaCache->size());

						masklocation[
							scheme.getMask()] =
							m_operatingAreaCache->size();

							m_operatingAreaCache->push_back(
								scheme);
					}
				}

				const double totalcos =
					static_cast<double>(
						allmasks.size());

				const bool got_output =
					(m_outputsMap.find(output) !=
						m_outputsMap.end());

				for (int period = 1;
					period <= getperiods();
					++period)
				{
					size_t oaid = 0;
					double notrespected = 0.0;

					for (const std::string& thselection :
						selected)
					{
						if (getYield(
							yieldname,
							thselection,
							0,
							period) > 0)
						{
							for (const Core::FMTMask& neighbor :
								m_operatingAreaCache
								->at(allmasks.at(oaid))
								.getNeighbors())
							{
								if (masklocation.find(neighbor) !=
									masklocation.end())
								{
									const std::string nselection =
										selected.at(
											masklocation.at(
												neighbor));

									if (!got_output ||
										((getYield(
											yieldname,
											nselection,
											0,
											period) > 0) &&
											(getValue(
												output,
												nselection,
												period) >= ratio)))
									{
										++notrespected;
										break;
									}
								}
							}
						}

						++oaid;
					}

					allratios.push_back(
						((totalcos - notrespected) /
							totalcos) * 100);
				}
			}
		}
		catch (...)
		{
			_exhandler->printExceptions(
				"",
				"FMTModelCache::Juxtaposition",
				__LINE__,
				__FILE__);
		}

		return allratios;
	}

	double FMTModelCache::getValue(
		const std::string& outputname,
		const std::string& themeselection,
		const int& period) const
	{
		double value = 0.0;

		try
		{
			const std::string cachekey =
				getcachekey(
					"OUTPUT",
					outputname,
					themeselection,
					0,
					period);

			const bool incache =
				fillfromcache(
					value,
					cachekey);

			if (!incache)
			{
				const Core::FMTMask subset =
					_themeSelectionToMask(
						themeselection);

				const Core::FMTOutput theoutput =
					getOutput(
						outputname,
						subset);

				if (!outputs.empty() &&
					m_outputsMap.find(outputname) !=
					m_outputsMap.end() &&
					!theoutput.empty())
				{
					value =
						getvaluefrommodel(
							theoutput,
							period);
				}

				settocache(
					cachekey,
					value);
			}
		}
		catch (...)
		{
			_exhandler->printExceptions(
				"",
				"FMTModelCache::getValue",
				__LINE__,
				__FILE__);
		}

		return value;
	}

	double FMTModelCache::getyieldfrommodel(
		const Core::FMTYieldRequest& request,
		const std::string& yieldname) const
	{
		double value = 0.0;

		try
		{
			boost::lock_guard<boost::recursive_mutex>
				guard(*m_mutex);

			value =
				yields.get(
					request,
					yieldname);
		}
		catch (...)
		{
			_exhandler->printExceptions(
				"",
				"FMTModelCache::getyieldfrommodel",
				__LINE__,
				__FILE__);
		}

		return value;
	}

	double FMTModelCache::getYield(
		const std::string& yieldname,
		const std::string& themeselection,
		const int& age,
		const int& period) const
	{
		double value = 0.0;

		try
		{
			const std::string cachekey =
				getcachekey(
					"YIELD",
					yieldname,
					themeselection,
					age,
					period);

			const bool incache =
				fillfromcache(
					value,
					cachekey);

			if (!incache)
			{
				const Core::FMTMask subset =
					_themeSelectionToMask(
						themeselection);

				if (!yields.empty() &&
					yields.isYld(yieldname) &&
					!(subset.empty() &&
						!themeselection.empty()))
				{
					const Core::FMTDevelopment adev(
						subset,
						age,
						0,
						period);

					const Graph::FMTGraphVertexToYield
						graph_info =
						getGraphVertexToYield();

					const Core::FMTYieldRequest
						yieldrequest =
						adev.getYieldRequest(
							&graph_info);

					value =
						getyieldfrommodel(
							yieldrequest,
							yieldname);
				}

				settocache(
					cachekey,
					value);
			}
		}
		catch (...)
		{
			_exhandler->printExceptions(
				"",
				"FMTModelCache::getYield",
				__LINE__,
				__FILE__);
		}

		return value;
	}

	std::vector<std::string> FMTModelCache::getAttributes(
		const int& themeid,
		const std::string& value,
		const bool& aggregates) const
	{
		std::vector<std::string> attributes;

		try
		{
			if (!themes.empty() &&
				(static_cast<size_t>(themeid) <
					themes.size()))
			{
				if (themes.at(themeid).isAttribute(value) ||
					themes.at(themeid).isAggregate(value))
				{
					attributes =
						themes.at(themeid)
						.getAttributes(
							value,
							aggregates);
				}
				else
				{
					attributes =
						themes.at(themeid)
						.getAttributes(
							"?",
							aggregates);
				}
			}
		}
		catch (...)
		{
			_exhandler->printExceptions(
				"",
				"FMTModelCache::getAttributes",
				__LINE__,
				__FILE__);
		}

		return attributes;
	}

	std::vector<std::string>
		FMTModelCache::getattributesdescription(
			const int& themeid,
			const std::string& value) const
	{
		std::vector<std::string> attributes;

		try
		{
			if (!themes.empty() &&
				(static_cast<size_t>(themeid) <
					themes.size()))
			{
				std::vector<std::string> basenames =
					themes.at(themeid)
					.getAttributeNames();

				std::vector<std::string> references;

				if (themes.at(themeid).isAttribute(value))
				{
					references =
						themes.at(themeid)
						.getAttributes(value);
				}
				else
				{
					references =
						themes.at(themeid)
						.getAttributes("?");
				}

				for (const std::string& refvalue :
					references)
				{
					size_t ref_id =
						std::distance(
							references.begin(),
							std::find(
								references.begin(),
								references.end(),
								refvalue));

					attributes.push_back(
						basenames.at(ref_id));
				}
			}
		}
		catch (...)
		{
			_exhandler->printExceptions(
				"",
				"FMTModelCache::getattributesdescription",
				__LINE__,
				__FILE__);
		}

		return attributes;
	}

	std::vector<std::string>
		FMTModelCache::getAggregates(
			const int& themeid) const
	{
		std::vector<std::string> aggregates;

		try
		{
			if (!themes.empty() &&
				(static_cast<size_t>(themeid) <
					themes.size()))
			{
				aggregates =
					themes.at(themeid)
					.getAggregates();
			}
		}
		catch (...)
		{
			_exhandler->printExceptions(
				"",
				"FMTModelCache::getAggregates",
				__LINE__,
				__FILE__);
		}

		return aggregates;
	}

	std::string FMTModelCache::getcachekey(
		const std::string& type,
		const std::string& outputname,
		const std::string& themeselection,
		const int& age,
		const int& period) const
	{
		std::string key;

		try
		{
			key =
				type + "_" +
				outputname + "_" +
				themeselection + "_" +
				std::to_string(age) + "_" +
				std::to_string(period);
		}
		catch (...)
		{
			_exhandler->printExceptions(
				"",
				"FMTModelCache::getcachekey",
				__LINE__,
				__FILE__);
		}

		return key;
	}

	bool FMTModelCache::fillfromcache(
		double& value,
		const std::string& cachekey) const
	{
		bool gotincash = false;

		try
		{
			boost::lock_guard<boost::recursive_mutex>
				guard(*m_generalCacheMutex);

			std::unordered_map<
				std::string,
				double>::const_iterator cacheit =
				m_generalCache.find(cachekey);

			if (cacheit !=
				m_generalCache.end())
			{
				value = cacheit->second;
				gotincash = true;
			}
		}
		catch (...)
		{
			_exhandler->printExceptions(
				"",
				"FMTModelCache::fillfromcache",
				__LINE__,
				__FILE__);
		}

		return gotincash;
	}

	bool FMTModelCache::getSeriesFromCache(
		std::set<Core::FMTSerie>& value,
		const std::string& cachekey) const
	{
		bool gotincash = false;

		try
		{
			boost::lock_guard<boost::recursive_mutex>
				guard(*m_serieCacheMutex);

			std::unordered_map<
				std::string,
				std::set<Core::FMTSerie>>
				::const_iterator cacheit =
				m_serieCache.find(cachekey);

			if (cacheit !=
				m_serieCache.end())
			{
				value = cacheit->second;
				gotincash = true;
			}
		}
		catch (...)
		{
			_exhandler->printExceptions(
				"",
				"FMTModelCache::getSeriesFromCache",
				__LINE__,
				__FILE__);
		}

		return gotincash;
	}

	void FMTModelCache::settocache(
		const std::string& cachekey,
		const double& value) const
	{
		try
		{
			if (m_cachingSwitch)
			{
				boost::lock_guard<boost::recursive_mutex>
					guard(*m_generalCacheMutex);

				m_generalCache[cachekey] =
					value;
			}
		}
		catch (...)
		{
			_exhandler->printExceptions(
				"",
				"FMTModelCache::settocache",
				__LINE__,
				__FILE__);
		}
	}

	void FMTModelCache::setSeriesToCache(
		const std::string& cachekey,
		const std::set<Core::FMTSerie>& value) const
	{
		try
		{
			if (m_cachingSwitch)
			{
				boost::lock_guard<boost::recursive_mutex>
					guard(*m_serieCacheMutex);

				m_serieCache[cachekey] =
					value;
			}
		}
		catch (...)
		{
			_exhandler->printExceptions(
				"",
				"FMTModelCache::setSeriesToCache",
				__LINE__,
				__FILE__);
		}
	}

	std::vector<std::string>
		FMTModelCache::getactionaggregates(
			const std::string& filter) const
	{
		try
		{
			for (const Core::FMTAction& action : actions)
			{
				if (action.getName() == filter)
				{
					return action.getAggregates();
				}
			}
		}
		catch (...)
		{
			_exhandler->printExceptions(
				"",
				"FMTModelCache::getactionaggregates",
				__LINE__,
				__FILE__);
		}

		return std::vector<std::string>();
	}

	std::vector<std::string>
		FMTModelCache::getActions(
			const std::string& filter) const
	{
		std::vector<std::string> actionsname;

		actionsname.reserve(
			actions.size());

		try
		{
			for (const Core::FMTAction& action : actions)
			{
				const std::vector<std::string> aggregates =
					action.getAggregates();

				if (filter == "?" ||
					std::find(
						aggregates.begin(),
						aggregates.end(),
						filter) != aggregates.end())
				{
					actionsname.push_back(
						action.getName());
				}
			}
		}
		catch (...)
		{
			_exhandler->printExceptions(
				"",
				"FMTModelCache::getActions",
				__LINE__,
				__FILE__);
		}

		return actionsname;
	}

	std::vector<std::string>
		FMTModelCache::getConstraints(
			const std::string& output) const
	{
		std::vector<std::string> constraintsname;

		constraintsname.reserve(
			constraints.size());

		try
		{
			for (const Core::FMTConstraint& constraint :
				constraints)
			{
				const std::string value =
					std::string(constraint);

				if (value.find(output) !=
					std::string::npos)
				{
					constraintsname.push_back(
						value);
				}
			}
		}
		catch (...)
		{
			_exhandler->printExceptions(
				"",
				"FMTModelCache::getConstraints",
				__LINE__,
				__FILE__);
		}

		return constraintsname;
	}

	std::vector<std::string>
		FMTModelCache::getBuildExceptions(
			const int& exceptionid) const
	{
		std::vector<std::string> values;

		try
		{
			if (m_buildExceptions.find(exceptionid) !=
				m_buildExceptions.end())
			{
				values =
					m_buildExceptions.at(
						exceptionid);
			}
		}
		catch (...)
		{
			_exhandler->printExceptions(
				"",
				"FMTModelCache::getBuildExceptions",
				__LINE__,
				__FILE__);
		}

		return values;
	}

	std::set<Core::FMTSerie>
		FMTModelCache::getRotations(
			const std::string& themeselection,
			const std::string& aggregate) const
	{
		std::set<Core::FMTSerie> series;

		try
		{
			const std::string cacheKey =
				getcachekey(
					"SERIE",
					aggregate,
					themeselection,
					0,
					0);

			const bool gotSeries =
				getSeriesFromCache(
					series,
					cacheKey);

			if (!gotSeries)
			{
				const Core::FMTMask subset =
					_themeSelectionToMask(
						themeselection);

				if (!aggregate.empty() &&
					!subset.empty())
				{
					series =
						FMTSrModel::getRotations(
							subset,
							aggregate);
				}

				setSeriesToCache(
					cacheKey,
					series);
			}
		}
		catch (...)
		{
			_exhandler->printExceptions(
				"",
				"FMTModelCache::getRotations",
				__LINE__,
				__FILE__);
		}

		return series;
	}

	bool FMTModelCache::haveSerie(
		const std::string& p_serie,
		const std::string& themeselection,
		const std::string& aggregate) const
	{
		bool gotIt = false;

		try
		{
			const Core::FMTMask subset =
				_themeSelectionToMask(
					themeselection);

			if (!aggregate.empty() &&
				!subset.empty())
			{
				const std::string cacheKey =
					getcachekey(
						"SERIE",
						aggregate,
						themeselection,
						0,
						0);

				std::set<Core::FMTSerie> allSeries;

				const bool gotSeries =
					getSeriesFromCache(
						allSeries,
						cacheKey);

				if (!gotSeries)
				{
					allSeries =
						FMTSrModel::getRotations(
							subset,
							aggregate);

					setSeriesToCache(
						cacheKey,
						allSeries);
				}

				const int length =
					getParameter(
						Models::FMTintmodelparameters::LENGTH);

				Core::FMTSerie lowerBound(
					p_serie,
					0);

				Core::FMTSerie upperBound(
					p_serie,
					length + 1);

				std::set<Core::FMTSerie>::const_iterator it =
					allSeries.lower_bound(
						lowerBound);

				std::set<Core::FMTSerie>::const_iterator itUpper =
					allSeries.upper_bound(
						upperBound);

				for (; it != itUpper; ++it)
				{
					if (it->getSerie() == p_serie)
					{
						gotIt = true;
						break;
					}
				}
			}
		}
		catch (...)
		{
			_exhandler->printExceptions(
				"",
				"FMTModelCache::haveSerie",
				__LINE__,
				__FILE__);
		}

		return gotIt;
	}

	std::vector<int> FMTModelCache::getGraphStats() const
	{
		std::vector<int> stats;

		try
		{
			Graph::FMTGraphStats graphstats =
				FMTSrModel::getStats();

			stats.push_back(graphstats.cols);
			stats.push_back(graphstats.rows);
			stats.push_back(graphstats.vertices);
			stats.push_back(graphstats.edges);
			stats.push_back(graphstats.transfer_rows);
			stats.push_back(graphstats.output_rows);
			stats.push_back(graphstats.output_cols);
		}
		catch (...)
		{
			_exhandler->printExceptions(
				"",
				"FMTModelCache::getGraphStats",
				__LINE__,
				__FILE__);
		}

		return stats;
	}

	std::vector<int> FMTModelCache::getGraphStatsSubset(
		const std::string& p_ThemeSelection) const
	{
		std::vector<int> stats;

		try
		{
			const Core::FMTMask subset =
				_themeSelectionToMask(
					p_ThemeSelection);

			Graph::FMTGraphStats graphstats =
				FMTSrModel::getGraphStats(
					subset);

			stats.push_back(graphstats.cols);
			stats.push_back(graphstats.rows);
			stats.push_back(graphstats.vertices);
			stats.push_back(graphstats.edges);
			stats.push_back(graphstats.transfer_rows);
		}
		catch (...)
		{
			_exhandler->printExceptions(
				"",
				"FMTModelCache::getGraphStatsSubset",
				__LINE__,
				__FILE__);
		}

		return stats;
	}

	std::vector<std::string> FMTModelCache::getOutputs() const
	{
		std::vector<std::string> outputsname;

		outputsname.reserve(
			outputs.size());

		try
		{
			for (const Core::FMTOutput& output :
				outputs)
			{
				outputsname.push_back(
					output.getName());
			}
		}
		catch (...)
		{
			_exhandler->printExceptions(
				"",
				"FMTModelCache::getOutputs",
				__LINE__,
				__FILE__);
		}

		return outputsname;
	}

	std::vector<std::string> FMTModelCache::getYields() const
	{
		try
		{
			return yields.getAllYieldNames();
		}
		catch (...)
		{
			_exhandler->printExceptions(
				"",
				"FMTModelCache::getYields",
				__LINE__,
				__FILE__);
		}

		return std::vector<std::string>();
	}

	std::vector<std::string> FMTModelCache::getThemes() const
	{
		std::vector<std::string> themenames;

		themenames.reserve(
			themes.size());

		try
		{
			for (const Core::FMTTheme& theme :
				themes)
			{
				themenames.push_back(
					theme.getName());
			}
		}
		catch (...)
		{
			_exhandler->printExceptions(
				"",
				"FMTModelCache::getThemes",
				__LINE__,
				__FILE__);
		}

		return themenames;
	}

	Logging::FMTExcelLogger* FMTModelCache::getlogger()
	{
		boost::lock_guard<boost::recursive_mutex>
			guard(*m_mutex);

		Logging::FMTExcelLogger* log =
			dynamic_cast<Logging::FMTExcelLogger*>(
				this->_logger.get());

		return log;
	}

	void FMTModelCache::putlogger(
		const std::unique_ptr<Logging::FMTLogger>& log)
	{
		boost::lock_guard<boost::recursive_mutex>
			guard(*m_mutex);

		Core::FMTObject::passInLogger(
			log);

		//this->passInLogger(log);
	}

	bool FMTModelCache::writejpeg(
		const size_t& themeid,
		const std::vector<std::string> attributevalues,
		const std::string& jpeglocation) const
	{
		try
		{
			_loadMap();

			if (m_map)
			{
				Parser::FMTAreaParser areaparser;

				const Core::FMTTheme& theme =
					themes.at(themeid);

				std::map<std::string, std::string> layer_map;

				const std::vector<std::string>& allattributes =
					theme.getBaseAttributes();

				size_t attributeid = 0;

				for (const std::string& attribute :
					allattributes)
				{
					if (attributeid < attributevalues.size())
					{
						layer_map[attribute] =
							attributevalues.at(attributeid);
					}
					else
					{
						layer_map[attribute] =
							attributevalues.back();
					}

					++attributeid;
				}

				return areaparser.writeForestTheme(
					*m_map,
					theme,
					jpeglocation,
					layer_map,
					"BMP");
			}
		}
		catch (...)
		{
			_exhandler->printExceptions(
				"",
				"FMTModelCache::writejpeg",
				__LINE__,
				__FILE__);
		}

		return false;
	}

}