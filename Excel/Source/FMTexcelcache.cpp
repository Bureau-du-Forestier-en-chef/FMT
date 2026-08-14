#include "stdafx.h"
#include "FMTExcelCache.h"
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
#include "FMTObject.h"
#include <msclr\marshal_cppstd.h>
#include <cliext/list>
#include "FMTModelParser.h"
#include "FMTModelCache.h"
#include "FMTException.h"
#include "FMTObject.h"
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include "FMTExcelExceptionHandler.h"
#include "FMTExcelLogger.h"
#include "FMTSerie.h"


namespace Wrapper
{
	FMTExcelCache::FMTExcelCache() :
		m_parser(new Parser::FMTModelParser()),
		m_cacheLog(),
		m_cacheExceptionHandler(),
		m_models(
			new std::unordered_map<
			std::string,
			FMTModelCache>()),
		m_exceptionRaised(false)
	{
		std::unique_ptr<Logging::FMTLogger> logger(
			new Logging::FMTExcelLogger());

		std::unique_ptr<Exception::FMTExceptionHandler>
			handler(
				new Exception::FMTExcelExceptionHandler());

		m_parser->passInLogger(logger);
		m_parser->passInExceptionHandler(handler);

		m_cacheLog =
			m_parser->getLogger();

		m_cacheExceptionHandler =
			dynamic_cast<
			Exception::FMTExcelExceptionHandler*>(
				m_parser->getExceptionHandler());
	}

	bool FMTExcelCache::gotexception()
	{
		return m_exceptionRaised;
	}

	System::String^ FMTExcelCache::getlogoutput()
	{
		FMTModelCache emptycache;

		Logging::FMTExcelLogger* log =
			emptycache.getlogger();

		std::string out;

		if (log != nullptr)
		{
			out = log->getPrintOut();
			log->clearOut();
		}

		System::String^ str2 =
			gcnew System::String(out.c_str());

		return str2;
	}

	FMTExcelCache::~FMTExcelCache()
	{
		if (m_parser != nullptr)
		{
			delete m_parser;
		}

		/*
		if (m_cacheLog != nullptr)
		{
			delete m_cacheLog;
		}

		if (m_cacheExceptionHandler != nullptr)
		{
			delete m_cacheExceptionHandler;
		}
		*/

		if (m_models != nullptr)
		{
			delete m_models;
		}
	}

	void FMTExcelCache::_captureException(
		const std::string& method)
	{
		(void)method;

		m_exceptionRaised = true;
	}

	std::string FMTExcelCache::_formatForCache(
		System::String^ primarylocation,
		System::String^ scenario)
	{
		std::string cachename;

		try
		{
			msclr::interop::marshal_context context;

			const std::string pfile =
				context.marshal_as<std::string>(
					primarylocation);

			const std::string sfile =
				context.marshal_as<std::string>(
					scenario);

			const std::string justprimary =
				_getPrimaryName(pfile);

			cachename =
				justprimary + "~" + sfile;

			boost::to_upper(cachename);
		}
		catch (...)
		{
			_captureException(
				"FMTExcelCache::_formatForCache");
		}

		return cachename;
	}

	std::string FMTExcelCache::_getPrimaryName(
		const std::string& primarylocation)
	{
		std::string primaryname;

		try
		{
			std::string base_filename =
				primarylocation.substr(
					primarylocation.find_last_of("/\\") + 1);

			std::string::size_type const fit(
				base_filename.find_last_of('.'));

			primaryname =
				base_filename.substr(
					0,
					fit);
		}
		catch (...)
		{
			_captureException(
				"FMTExcelCache::_getPrimaryName");
		}

		return primaryname;
	}

	std::string FMTExcelCache::_getMapPath(
		const std::string& primarylocation)
	{
		std::string mappath;

		try
		{
			const boost::filesystem::path primpath(
				primarylocation);

			boost::filesystem::path pridir =
				primpath.parent_path();

			const boost::filesystem::path mapdir =
				pridir /
				boost::filesystem::path("Carte");

			if (boost::filesystem::is_directory(mapdir))
			{
				for (auto& entry :
					boost::make_iterator_range(
						boost::filesystem::directory_iterator(
							mapdir),
						{}))
				{
					std::string extension =
						entry.path()
						.extension()
						.string();

					boost::to_upper(extension);

					if (extension == ".SHP")
					{
						mappath =
							std::string(
								entry.path().string());

						break;
					}
				}
			}
		}
		catch (...)
		{
			_captureException(
				"FMTExcelCache::_getMapPath");
		}

		return mappath;
	}

	System::Collections::Generic::List<System::String^>^
		FMTExcelCache::readnsolveTemplates(
			System::String^ primarylocation,
			System::String^ templatefolder,
			int length,
			bool solve)
	{
		System::Collections::Generic::List<System::String^>^
			scenarios =
			gcnew System::Collections::Generic::List<
			System::String^>();

		try
		{
			msclr::interop::marshal_context context;

			const std::string pfile =
				context.marshal_as<std::string>(
					primarylocation);

			const std::string sfile =
				context.marshal_as<std::string>(
					templatefolder);

			const std::vector<Models::FMTModel> allmodels =
				m_parser->readTemplates(
					pfile,
					sfile);

			const std::string mappath =
				_getMapPath(
					pfile);

			for (const Models::FMTModel& model :
				allmodels)
			{
				const std::string name =
					model.getName();

				if (name != "ROOT")
				{
					System::String^ modelname =
						gcnew System::String(
							name.c_str());

					const std::string naming =
						_formatForCache(
							primarylocation,
							modelname);

					(*m_models)[naming] =
						FMTModelCache(
							model,
							mappath);

					(*m_models)[naming]
						.setLength(length);

					if ((*m_models)[naming]
						.buildnsolve(solve))
					{
						scenarios->Add(
							modelname);
					}
					else
					{
						FMTModelCache emptycache;

						Logging::FMTExcelLogger* log =
							emptycache.getlogger();

						*log <<
							"Infeasable model: " +
							name +
							"\n";
					}
				}
			}
		}
		catch (...)
		{
			_captureException(
				"FMTExcelCache::readnsolveTemplates");
		}

		return scenarios;
	}

	bool FMTExcelCache::addAndBuild(
		System::String^ primarylocation,
		System::String^ scenario,
		int length)
	{
		try
		{
			const std::string naming =
				_formatForCache(
					primarylocation,
					scenario);

			if (m_models->find(naming) ==
				m_models->end())
			{
				msclr::interop::marshal_context context;

				const std::string pfile =
					context.marshal_as<std::string>(
						primarylocation);

				const std::string sfile =
					context.marshal_as<std::string>(
						scenario);

				std::vector<std::string> scenarios(
					1,
					sfile);

				const std::vector<Models::FMTModel>
					allmodels =
					m_parser->readproject(
						pfile,
						scenarios);

				const std::string mappath =
					_getMapPath(
						pfile);

				(*m_models)[naming] =
					FMTModelCache(
						allmodels.at(0),
						mappath);

				(*m_models)[naming]
					.setLength(length);

				if ((*m_models)[naming]
					.buildnsolve(false))
				{
					return true;
				}
			}
			else
			{
				FMTModelCache emptycache;

				Logging::FMTExcelLogger* log =
					emptycache.getlogger();

				if (log != nullptr)
				{
					log->clearOut();
				}
			}
		}
		catch (...)
		{
			_captureException(
				"FMTExcelCache::addAndBuild");
		}

		return false;
	}

	bool FMTExcelCache::add(
		System::String^ primarylocation,
		System::String^ scenario)
	{
		try
		{
			const std::string naming =
				_formatForCache(
					primarylocation,
					scenario);

			if (m_models->find(naming) ==
				m_models->end())
			{
				msclr::interop::marshal_context context;

				const std::string pfile =
					context.marshal_as<std::string>(
						primarylocation);

				const std::string sfile =
					context.marshal_as<std::string>(
						scenario);

				std::vector<std::string> scenarios(
					1,
					sfile);

				const std::vector<Models::FMTModel>
					allmodels =
					m_parser->readproject(
						pfile,
						scenarios);

				const std::vector<
					std::vector<Core::FMTSchedule>>
					allschedule =
					m_parser->readSchedules(
						pfile,
						allmodels);

				const std::string mappath =
					_getMapPath(
						pfile);

				int period = 0;

				for (const Core::FMTSchedule& schedule :
					allschedule.at(0))
				{
					period =
						std::max(
							period,
							schedule.getPeriod());
				}

				(*m_models)[naming] =
					FMTModelCache(
						allmodels.at(0),
						mappath);

				(*m_models)[naming]
					.setLength(period);

				m_models->at(naming)
					.setSolution(
						allschedule.at(0));

				return true;
			}
			else
			{
				FMTModelCache emptycache;

				Logging::FMTExcelLogger* log =
					emptycache.getlogger();

				if (log != nullptr)
				{
					log->clearOut();
				}
			}
		}
		catch (...)
		{
			_captureException(
				"FMTExcelCache::add");
		}

		return false;
	}

	void FMTExcelCache::remove(
		System::String^ primarylocation,
		System::String^ scenario)
	{
		try
		{
			const std::string naming =
				_formatForCache(
					primarylocation,
					scenario);

			if (m_models->find(naming) !=
				m_models->end())
			{
				m_models->erase(
					naming);
			}
		}
		catch (...)
		{
			_captureException(
				"FMTExcelCache::remove");
		}
	}

	System::Collections::Generic::List<int>^
		FMTExcelCache::getperiods(
			System::String^ primaryname,
			System::String^ scenario)
	{
		System::Collections::Generic::List<int>^ list =
			gcnew System::Collections::Generic::List<int>();

		try
		{
			const std::string naming =
				_formatForCache(
					primaryname,
					scenario);

			std::unordered_map<
				std::string,
				FMTModelCache>::const_iterator mit =
				m_models->find(naming);

			if (mit != m_models->end())
			{
				for (int period = 1;
					period <= mit->second.getperiods();
					++period)
				{
					list->Add(period);
				}
			}
		}
		catch (...)
		{
			_captureException(
				"FMTExcelCache::getperiods");
		}

		return list;
	}

	bool FMTExcelCache::writejpeg(
		System::String^ jpeglocation,
		System::String^ primaryname,
		System::String^ scenario,
		int themeid,
		System::Collections::Generic::List<
		System::String^>^ attributes)
	{
		try
		{
			const std::string naming =
				_formatForCache(
					primaryname,
					scenario);

			std::unordered_map<
				std::string,
				FMTModelCache>::const_iterator mit =
				m_models->find(naming);

			if (mit != m_models->end())
			{
				std::vector<std::string> values;

				msclr::interop::marshal_context context;

				for each (System::String ^ attribute in attributes)
				{
					values.push_back(
						context.marshal_as<std::string>(
							attribute));
				}

				const std::string location =
					context.marshal_as<std::string>(
						jpeglocation);

				return mit->second.writejpeg(
					static_cast<size_t>(themeid),
					values,
					location);
			}
		}
		catch (...)
		{
			_captureException(
				"FMTExcelCache::writejpeg");
		}

		return false;
	}

	double FMTExcelCache::getvalue(
		System::String^ primaryname,
		System::String^ scenario,
		System::String^ outputname,
		System::String^ themeselection,
		int period)
	{
		double value = 0.0;

		try
		{
			const std::string naming =
				_formatForCache(
					primaryname,
					scenario);

			std::unordered_map<
				std::string,
				FMTModelCache>::const_iterator mit =
				m_models->find(naming);

			if (mit != m_models->end())
			{
				msclr::interop::marshal_context context;

				const std::string outname =
					context.marshal_as<std::string>(
						outputname);

				const std::string selection =
					context.marshal_as<std::string>(
						themeselection);

				value =
					mit->second.getValue(
						outname,
						selection,
						period);
			}
		}
		catch (...)
		{
			_captureException(
				"FMTExcelCache::getvalue");
		}

		return value;
	}

	double FMTExcelCache::getyield(
		System::String^ primaryname,
		System::String^ scenario,
		System::String^ yieldname,
		System::String^ themeselection,
		int period,
		int age)
	{
		double value = 0.0;

		try
		{
			const std::string naming =
				_formatForCache(
					primaryname,
					scenario);

			std::unordered_map<
				std::string,
				FMTModelCache>::const_iterator mit =
				m_models->find(naming);

			if (mit != m_models->end())
			{
				msclr::interop::marshal_context context;

				const std::string yieldnamec =
					context.marshal_as<std::string>(
						yieldname);

				const std::string selection =
					context.marshal_as<std::string>(
						themeselection);

				value =
					mit->second.getYield(
						yieldnamec,
						selection,
						age,
						period);
			}
		}
		catch (...)
		{
			_captureException(
				"FMTExcelCache::getyield");
		}

		return value;
	}

	System::Collections::Generic::List<System::String^>^
		FMTExcelCache::getattributes(
			System::String^ primaryname,
			System::String^ scenario,
			int themeid,
			System::String^ value,
			bool aggregates)
	{
		System::Collections::Generic::List<System::String^>^
			list =
			gcnew System::Collections::Generic::List<
			System::String^>();

		try
		{
			const std::string naming =
				_formatForCache(
					primaryname,
					scenario);

			std::unordered_map<
				std::string,
				FMTModelCache>::const_iterator mit =
				m_models->find(naming);

			if (mit != m_models->end())
			{
				msclr::interop::marshal_context context;

				const std::string valueof =
					context.marshal_as<std::string>(
						value);

				for (const std::string& item :
					mit->second.getAttributes(
						themeid,
						valueof,
						aggregates))
				{
					System::String^ sysvalue =
						gcnew System::String(
							item.c_str());

					list->Add(sysvalue);
				}
			}
		}
		catch (...)
		{
			_captureException(
				"FMTExcelCache::getattributes");
		}

		return list;
	}

	System::Collections::Generic::List<System::String^>^
		FMTExcelCache::getbuildexceptions(
			System::String^ primaryname,
			System::String^ scenario,
			int exception)
	{
		System::Collections::Generic::List<System::String^>^ list =
			gcnew System::Collections::Generic::List<System::String^>();

		try
		{
			std::unordered_map<
				std::string,
				FMTModelCache>::const_iterator mit =
				m_models->find(
					_formatForCache(
						primaryname,
						scenario));

			if (mit != m_models->end())
			{
				for (const std::string& value :
					mit->second.getBuildExceptions(
						exception))
				{
					System::String^ sysvalue =
						gcnew System::String(
							value.c_str());

					list->Add(sysvalue);
				}
			}
		}
		catch (...)
		{
			_captureException(
				"FMTExcelCache::getbuildexceptions");
		}

		return list;
	}

	System::Collections::Generic::List<System::String^>^
		FMTExcelCache::getnochoice(
			System::String^ primaryname,
			System::String^ scenario,
			System::String^ filter)
	{
		System::Collections::Generic::List<System::String^>^ list =
			gcnew System::Collections::Generic::List<System::String^>();

		try
		{
			const std::string naming =
				_formatForCache(
					primaryname,
					scenario);

			std::unordered_map<
				std::string,
				FMTModelCache>::const_iterator mit =
				m_models->find(naming);

			if (mit != m_models->end())
			{
				msclr::interop::marshal_context context;

				const std::string sfilter =
					context.marshal_as<std::string>(
						filter);

				for (const std::string& value :
					mit->second.getnoaction(
						sfilter))
				{
					System::String^ sysvalue =
						gcnew System::String(
							value.c_str());

					list->Add(sysvalue);
				}
			}
		}
		catch (...)
		{
			_captureException(
				"FMTExcelCache::getattributesdescription");
		}

		return list;
	}

	System::Collections::Generic::List<System::String^>^
		FMTExcelCache::getattributesdescription(
			System::String^ primaryname,
			System::String^ scenario,
			int themeid,
			System::String^ value)
	{
		System::Collections::Generic::List<System::String^>^ list =
			gcnew System::Collections::Generic::List<System::String^>();

		try
		{
			const std::string naming =
				_formatForCache(
					primaryname,
					scenario);

			std::unordered_map<
				std::string,
				FMTModelCache>::const_iterator mit =
				m_models->find(naming);

			if (mit != m_models->end())
			{
				msclr::interop::marshal_context context;

				const std::string valueof =
					context.marshal_as<std::string>(
						value);

				for (const std::string& item :
					mit->second.getattributesdescription(
						themeid,
						valueof))
				{
					System::String^ sysvalue =
						gcnew System::String(
							item.c_str());

					list->Add(sysvalue);
				}
			}
		}
		catch (...)
		{
			_captureException(
				"FMTExcelCache::getattributesdescription");
		}

		return list;
	}

	System::Collections::Generic::List<System::String^>^
		FMTExcelCache::getAggregates(
			System::String^ primaryname,
			System::String^ scenario,
			int themeid)
	{
		System::Collections::Generic::List<System::String^>^ list =
			gcnew System::Collections::Generic::List<System::String^>();

		try
		{
			const std::string naming =
				_formatForCache(
					primaryname,
					scenario);

			std::unordered_map<
				std::string,
				FMTModelCache>::const_iterator mit =
				m_models->find(naming);

			if (mit != m_models->end())
			{
				for (const std::string& value :
					mit->second.getAggregates(
						themeid))
				{
					System::String^ sysvalue =
						gcnew System::String(
							value.c_str());

					list->Add(sysvalue);
				}
			}
		}
		catch (...)
		{
			_captureException(
				"FMTExcelCache::getAggregates");
		}

		return list;
	}

	System::Collections::Generic::List<System::String^>^
		FMTExcelCache::getprimaries()
	{
		System::Collections::Generic::List<System::String^>^ list =
			gcnew System::Collections::Generic::List<System::String^>();

		for (const auto& item : *m_models)
		{
			const std::string value =
				item.first.substr(
					0,
					item.first.find("~"));

			System::String^ str2 =
				gcnew System::String(
					value.c_str());

			if (!list->Contains(str2))
			{
				list->Add(str2);
			}
		}

		return list;
	}

	int FMTExcelCache::size()
	{
		return static_cast<int>(
			m_models->size());
	}

	System::Collections::Generic::List<System::String^>^
		FMTExcelCache::getscenarios(
			System::String^ fichierprimaire)
	{
		System::Collections::Generic::List<System::String^>^ list =
			gcnew System::Collections::Generic::List<System::String^>();

		msclr::interop::marshal_context context;

		const std::string pfile =
			context.marshal_as<std::string>(
				fichierprimaire);

		for (const auto& item : *m_models)
		{
			if (item.first.find(pfile) !=
				std::string::npos)
			{
				const std::string value =
					item.first.substr(
						item.first.find("~") + 1,
						item.first.size());

				System::String^ str2 =
					gcnew System::String(
						value.c_str());

				list->Add(str2);
			}
		}

		return list;
	}

	System::Collections::Generic::List<System::String^>^
		FMTExcelCache::getConstraints(
			System::String^ primaryname,
			System::String^ scenario,
			System::String^ output)
	{
		System::Collections::Generic::List<System::String^>^ list =
			gcnew System::Collections::Generic::List<System::String^>();

		try
		{
			const std::string naming =
				_formatForCache(
					primaryname,
					scenario);

			std::unordered_map<
				std::string,
				FMTModelCache>::const_iterator mit =
				m_models->find(naming);

			if (mit != m_models->end())
			{
				msclr::interop::marshal_context context;

				const std::string sfilter =
					context.marshal_as<std::string>(
						output);

				for (const std::string& value :
					mit->second.getConstraints(
						sfilter))
				{
					System::String^ sysvalue =
						gcnew System::String(
							value.c_str());

					list->Add(sysvalue);
				}
			}
		}
		catch (...)
		{
			_captureException(
				"FMTExcelCache::getConstraints");
		}

		return list;
	}

	System::Collections::Generic::List<int>^
		FMTExcelCache::getGraphStatsSubset(
			System::String^ p_PrimaryName,
			System::String^ p_Scenario,
			System::String^ p_ThemeSelection)
	{
		System::Collections::Generic::List<int>^ list =
			gcnew System::Collections::Generic::List<int>();

		try
		{
			const std::string naming =
				_formatForCache(
					p_PrimaryName,
					p_Scenario);

			std::unordered_map<
				std::string,
				FMTModelCache>::const_iterator mit =
				m_models->find(naming);

			if (mit != m_models->end())
			{
				msclr::interop::marshal_context context;

				const std::string selection =
					context.marshal_as<std::string>(
						p_ThemeSelection);

				for (const int& value :
					mit->second.getGraphStatsSubset(
						selection))
				{
					list->Add(value);
				}
			}
		}
		catch (...)
		{
			_captureException(
				"FMTExcelCache::getgraphstats");
		}

		return list;
	}

	System::Collections::Generic::List<int>^
		FMTExcelCache::getgraphstats(
			System::String^ primaryname,
			System::String^ scenario)
	{
		System::Collections::Generic::List<int>^ list =
			gcnew System::Collections::Generic::List<int>();

		try
		{
			const std::string naming =
				_formatForCache(
					primaryname,
					scenario);

			std::unordered_map<
				std::string,
				FMTModelCache>::const_iterator mit =
				m_models->find(naming);

			if (mit != m_models->end())
			{
				for (const int& value :
					mit->second.getGraphStats())
				{
					list->Add(value);
				}
			}
		}
		catch (...)
		{
			_captureException(
				"FMTExcelCache::getgraphstats");
		}

		return list;
	}

	System::Collections::Generic::List<System::String^>^
		FMTExcelCache::getActions(
			System::String^ primaryname,
			System::String^ scenario,
			System::String^ filter)
	{
		System::Collections::Generic::List<System::String^>^ list =
			gcnew System::Collections::Generic::List<System::String^>();

		try
		{
			const std::string naming =
				_formatForCache(
					primaryname,
					scenario);

			std::unordered_map<
				std::string,
				FMTModelCache>::const_iterator mit =
				m_models->find(naming);

			if (mit != m_models->end())
			{
				msclr::interop::marshal_context context;

				const std::string sfilter =
					context.marshal_as<std::string>(
						filter);

				for (const std::string& value :
					mit->second.getActions(
						sfilter))
				{
					System::String^ sysvalue =
						gcnew System::String(
							value.c_str());

					list->Add(sysvalue);
				}
			}
		}
		catch (...)
		{
			_captureException(
				"FMTExcelCache::getActions");
		}

		return list;
	}

	System::Collections::Generic::List<System::String^>^
		FMTExcelCache::getactionaggregates(
			System::String^ primaryname,
			System::String^ scenario,
			System::String^ filter)
	{
		System::Collections::Generic::List<System::String^>^ list =
			gcnew System::Collections::Generic::List<System::String^>();

		try
		{
			const std::string naming =
				_formatForCache(
					primaryname,
					scenario);

			std::unordered_map<
				std::string,
				FMTModelCache>::const_iterator mit =
				m_models->find(naming);

			if (mit != m_models->end())
			{
				msclr::interop::marshal_context context;

				const std::string sfilter =
					context.marshal_as<std::string>(
						filter);

				for (const std::string& value :
					mit->second.getactionaggregates(
						sfilter))
				{
					System::String^ sysvalue =
						gcnew System::String(
							value.c_str());

					list->Add(sysvalue);
				}
			}
		}
		catch (...)
		{
			_captureException(
				"FMTExcelCache::getactionaggregates");
		}

		return list;
	}

	System::Collections::Generic::List<System::String^>^
		FMTExcelCache::getOutputs(
			System::String^ primaryname,
			System::String^ scenario)
	{
		System::Collections::Generic::List<System::String^>^ list =
			gcnew System::Collections::Generic::List<System::String^>();

		try
		{
			const std::string naming =
				_formatForCache(primaryname, scenario);

			std::unordered_map<
				std::string,
				FMTModelCache>::const_iterator mit =
				m_models->find(naming);

			if (mit != m_models->end())
			{
				for (const std::string& value :
					mit->second.getOutputs())
				{
					list->Add(
						gcnew System::String(
							value.c_str()));
				}
			}
		}
		catch (...)
		{
			_captureException(
				"FMTExcelCache::getOutputs");
		}

		return list;
	}

	System::Collections::Generic::List<System::String^>^
		FMTExcelCache::getYields(
			System::String^ primaryname,
			System::String^ scenario)
	{
		System::Collections::Generic::List<System::String^>^ list =
			gcnew System::Collections::Generic::List<System::String^>();

		try
		{
			const std::string naming =
				_formatForCache(primaryname, scenario);

			std::unordered_map<
				std::string,
				FMTModelCache>::const_iterator mit =
				m_models->find(naming);

			if (mit != m_models->end())
			{
				for (const std::string& value :
					mit->second.getYields())
				{
					list->Add(
						gcnew System::String(
							value.c_str()));
				}
			}
		}
		catch (...)
		{
			_captureException(
				"FMTExcelCache::getYields");
		}

		return list;
	}

	System::Collections::Generic::List<System::String^>^
		FMTExcelCache::getThemes(
			System::String^ primaryname,
			System::String^ scenario)
	{
		System::Collections::Generic::List<System::String^>^ list =
			gcnew System::Collections::Generic::List<System::String^>();

		try
		{
			const std::string naming =
				_formatForCache(primaryname, scenario);

			std::unordered_map<
				std::string,
				FMTModelCache>::const_iterator mit =
				m_models->find(naming);

			if (mit != m_models->end())
			{
				for (const std::string& value :
					mit->second.getThemes())
				{
					list->Add(
						gcnew System::String(
							value.c_str()));
				}
			}
		}
		catch (...)
		{
			_captureException(
				"FMTExcelCache::getThemes");
		}

		return list;
	}

	System::Collections::Generic::List<System::String^>^
		FMTExcelCache::getRotationsKeys(
			System::String^ primaryname,
			System::String^ scenario,
			System::String^ themeselection,
			System::String^ aggregate)
	{
		System::Collections::Generic::List<System::String^>^ values =
			gcnew System::Collections::Generic::List<System::String^>();

		try
		{
			const std::string naming =
				_formatForCache(primaryname, scenario);

			std::unordered_map<
				std::string,
				FMTModelCache>::const_iterator mit =
				m_models->find(naming);

			if (mit != m_models->end())
			{
				std::unordered_set<std::string>
					uniqueRotations;

				msclr::interop::marshal_context context;

				const std::string sfilter =
					context.marshal_as<std::string>(
						themeselection);

				const std::string saggregate =
					context.marshal_as<std::string>(
						aggregate);

				for (const Core::FMTSerie& rotation :
					mit->second.getRotations(
						sfilter,
						saggregate))
				{
					System::String^ rotationName =
						gcnew System::String(
							rotation.getSerie().c_str());

					const std::string key =
						context.marshal_as<std::string>(
							rotationName);

					if (uniqueRotations.find(key) ==
						uniqueRotations.end())
					{
						values->Add(rotationName);
						uniqueRotations.insert(key);
					}
				}
			}
		}
		catch (...)
		{
			_captureException(
				"FMTExcelCache::getRotationsKeys");
		}

		return values;
	}

	bool FMTExcelCache::containsRotations(
		System::String^ primaryname,
		System::String^ scenario,
		System::String^ serie,
		System::String^ themeselection,
		System::String^ aggregate)
	{
		bool answer = false;

		try
		{
			const std::string naming =
				_formatForCache(primaryname, scenario);

			std::unordered_map<
				std::string,
				FMTModelCache>::const_iterator mit =
				m_models->find(naming);

			if (mit != m_models->end())
			{
				msclr::interop::marshal_context context;

				const std::string sfilter =
					context.marshal_as<std::string>(
						themeselection);

				const std::string saggregate =
					context.marshal_as<std::string>(
						aggregate);

				const std::string theSerie =
					context.marshal_as<std::string>(
						serie);

				answer =
					mit->second.haveSerie(
						theSerie,
						sfilter,
						saggregate);
			}
		}
		catch (...)
		{
			_captureException(
				"FMTExcelCache::containsRotations");
		}

		return answer;
	}

	System::Collections::Generic::List<
		System::Collections::Generic::KeyValuePair<
		System::String^,
		int>>^ FMTExcelCache::getRotations(
			System::String^ primaryname,
			System::String^ scenario,
			System::String^ themeselection,
			System::String^ aggregate)
	{
		auto values =
			gcnew System::Collections::Generic::List<
			System::Collections::Generic::KeyValuePair<
			System::String^,
			int>>();

		try
		{
			const std::string naming =
				_formatForCache(primaryname, scenario);

			std::unordered_map<
				std::string,
				FMTModelCache>::const_iterator mit =
				m_models->find(naming);

			if (mit != m_models->end())
			{
				msclr::interop::marshal_context context;

				const std::string sfilter =
					context.marshal_as<std::string>(
						themeselection);

				const std::string saggregate =
					context.marshal_as<std::string>(
						aggregate);

				for (const Core::FMTSerie& rotation :
					mit->second.getRotations(
						sfilter,
						saggregate))
				{
					System::String^ rotationName =
						gcnew System::String(
							rotation.getSerie().c_str());

					values->Add(
						System::Collections::Generic::
						KeyValuePair<
						System::String^,
						int>(
							rotationName,
							rotation.getLength()));
				}
			}
		}
		catch (...)
		{
			_captureException(
				"FMTExcelCache::getRotations");
		}

		return values;
	}

	System::Collections::Generic::List<double>^
		FMTExcelCache::Juxtaposition(
			System::String^ primaryname,
			System::String^ scenario,
			System::Collections::Generic::List<
			System::String^>^ themeselection,
			System::String^ yieldname,
			System::String^ output,
			double ratio,
			double perimeters)
	{
		System::Collections::Generic::List<double>^ list =
			gcnew System::Collections::Generic::List<double>();

		try
		{
			const std::string naming =
				_formatForCache(primaryname, scenario);

			std::unordered_map<
				std::string,
				FMTModelCache>::const_iterator mit =
				m_models->find(naming);

			if (mit != m_models->end())
			{
				std::vector<std::string> themeselections;

				msclr::interop::marshal_context context;

				for each (System::String ^ ths in themeselection)
				{
					themeselections.push_back(
						context.marshal_as<std::string>(
							ths));
				}

				const std::string stroutput =
					context.marshal_as<std::string>(
						output);

				const std::string yieldof =
					context.marshal_as<std::string>(
						yieldname);

				for (const double& value :
					mit->second.Juxtaposition(
						themeselections,
						yieldof,
						stroutput,
						ratio,
						perimeters))
				{
					list->Add(value);
				}
			}
		}
		catch (...)
		{
			_captureException(
				"FMTExcelCache::Juxtaposition");
		}

		return list;
	}

	void FMTExcelCache::unraiseexception()
	{
		m_exceptionRaised = false;
	}

}