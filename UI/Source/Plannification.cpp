#include "stdafx.h"
#include <msclr\marshal_cppstd.h>

#include "FMTForm.h"
#include "Controller.h"

namespace Wrapper
{
	namespace
	{
		std::vector<std::string> _toStdVector(
			System::Collections::Generic::List<System::String^>^ p_values)
		{
			std::vector<std::string> converted;

			if (p_values == nullptr)
			{
				return converted;
			}

			for each (System::String ^ value in p_values)
			{
				converted.push_back(msclr::interop::marshal_as<std::string>(value));
			}

			return converted;
		}
	}

	bool FMTForm::Plannification(
		System::String^ fichierPri,
		System::Collections::Generic::List<int>^ scenarios,
		int solver,
		int period,
		int nbreProcessus,
		System::Collections::Generic::List<System::String^>^ outputs,
		int outputLevel,
		int etanduSortiesMin,
		int etanduSortiesMax,
		System::String^ cheminSorties,
		System::String^ providerGdal,
		System::Collections::Generic::List<bool>^ playback)
	{
		try
		{
			FMTWrapperCore::PlanningParameters params;
			params.primaryFilePath = msclr::interop::marshal_as<std::string>(fichierPri);
			params.solver = solver;
			params.numberOfPeriods = period;
			params.numberOfThreads = nbreProcessus;
			params.outputNames = _toStdVector(outputs);
			params.outputLevel = outputLevel;
			params.outputMinPeriod = etanduSortiesMin;
			params.outputMaxPeriod = etanduSortiesMax;
			params.outputPath = msclr::interop::marshal_as<std::string>(cheminSorties);
			params.gdalProvider = msclr::interop::marshal_as<std::string>(providerGdal);

			std::vector<int> modelIndexes;

			for each (int scenario in scenarios)
			{
				modelIndexes.push_back(scenario);
			}

			// Sans drapeaux, le Core refuse le premier scénario, comme l'ancien code
			// échouait à lire playback.
			std::vector<bool> playbackFlags;

			if (playback != nullptr)
			{
				for each (bool value in playback)
				{
					playbackFlags.push_back(value);
				}
			}

			FMTWrapperCore::Controller::plan(params, modelIndexes, playbackFlags);
		}
		catch (...)
		{
			_raiseFromCatch("", "FMTForm::Plannification", __LINE__, __FILE__);
			return false;
		}

		return true;
	}

	bool FMTForm::Replanification(
		int indexScenStrategique,
		int indexScenStochastique,
		int indexScenTactique,
		int solver,
		int period,
		int periodReplannif,
		double variabilite,
		int nbreProcessus,
		int nombreReplicasMin,
		int nombreReplicasMax,
		System::Collections::Generic::List<System::String^>^ outputs,
		int outputLevel,
		System::String^ cheminSorties,
		System::String^ providerGdal,
		int taskLogLevel,
		bool indProduireSolution,
		bool p_writeSchedule)
	{
		// p_writeSchedule n'a jamais été utilisé : depuis la première version de l'interface,
		// c'est indProduireSolution qui commande l'écriture des cédules des réplicats. Il
		// reste dans la signature publique, dont dépend le UI .NET.
		try
		{
			FMTWrapperCore::ReplanningParameters params;
			params.solver = solver;
			params.numberOfPeriods = period;
			params.replanningPeriods = periodReplannif;
			params.minimalDrift = variabilite;
			params.numberOfThreads = nbreProcessus;
			params.minimumReplicates = nombreReplicasMin;
			params.maximumReplicates = nombreReplicasMax;
			params.outputNames = _toStdVector(outputs);
			params.outputLevel = outputLevel;
			params.outputPath = msclr::interop::marshal_as<std::string>(cheminSorties);
			params.gdalProvider = msclr::interop::marshal_as<std::string>(providerGdal);
			params.taskLogLevel = taskLogLevel;
			params.writeSchedules = indProduireSolution;

			FMTWrapperCore::Controller::replan(
				params,
				indexScenStrategique,
				indexScenStochastique,
				indexScenTactique);
		}
		catch (...)
		{
			_raiseFromCatch("", "FMTForm::Replanification", __LINE__, __FILE__);
			return false;
		}

		return true;
	}
}
