#include "stdafx.h"
#include <msclr\marshal_cppstd.h>

#include "FMTForm.h"
#include "Controller.h"
#include "Conversions.h"

namespace Wrapper
{
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
			FMTWrapper::Backend::PlanningParameters params;
			params.primaryFilePath = Conversions::toStdString(fichierPri);
			params.solver = solver;
			params.numberOfPeriods = period;
			params.numberOfThreads = nbreProcessus;
			params.outputNames = Conversions::toStdVector(outputs);
			params.outputLevel = outputLevel;
			params.outputMinPeriod = etanduSortiesMin;
			params.outputMaxPeriod = etanduSortiesMax;
			params.outputPath = Conversions::toStdString(cheminSorties);
			params.gdalProvider = Conversions::toStdString(providerGdal);

			std::vector<int> modelIndexes;

			for each (int scenario in scenarios)
			{
				modelIndexes.push_back(scenario);
			}

			// Without flags, the Core refuses the first scenario, as the former code
			// failed to read playback.
			std::vector<bool> playbackFlags;

			if (playback != nullptr)
			{
				for each (bool value in playback)
				{
					playbackFlags.push_back(value);
				}
			}

			FMTWrapper::Backend::Controller::plan(params, modelIndexes, playbackFlags);
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
		// p_writeSchedule has never been used: since the first version of the interface,
		// indProduireSolution drives the writing of the replicate schedules. It
		// stays in the public signature, which the .NET UI depends on.
		try
		{
			FMTWrapper::Backend::ReplanningParameters params;
			params.solver = solver;
			params.numberOfPeriods = period;
			params.replanningPeriods = periodReplannif;
			params.minimalDrift = variabilite;
			params.numberOfThreads = nbreProcessus;
			params.minimumReplicates = nombreReplicasMin;
			params.maximumReplicates = nombreReplicasMax;
			params.outputNames = Conversions::toStdVector(outputs);
			params.outputLevel = outputLevel;
			params.outputPath = Conversions::toStdString(cheminSorties);
			params.gdalProvider = Conversions::toStdString(providerGdal);
			params.taskLogLevel = taskLogLevel;
			params.writeSchedules = indProduireSolution;

			FMTWrapper::Backend::Controller::replan(
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
