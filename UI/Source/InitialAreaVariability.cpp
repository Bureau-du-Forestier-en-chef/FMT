#include "stdafx.h"
#include <msclr\marshal_cppstd.h>

#include "FMTForm.h"
#include "Controller.h"
#include "Conversions.h"

namespace Wrapper
{
	bool FMTForm::InitialAreaVariability(
		System::String^ fichierPri,
		int scenario,
		int solver,
		System::Collections::Generic::List<System::String^>^ contraintes,
		int period,
		System::Collections::Generic::List<System::String^>^ outputs,
		int outputLevel,
		int etanduSortiesMin,
		int etanduSortiesMax,
		System::String^ cheminSorties,
		System::String^ providerGdal,
		System::Collections::Generic::List<System::Collections::Generic::List<System::String^>^>^ ListeInformations)
	{
		// fichierPri has never been used: it stays in the public signature,
		// which the .NET UI depends on.
		try
		{
			FMTWrapper::Backend::AreaVariabilityParameters params;
			params.solver = solver;
			params.constraintNames = Conversions::toStdVector(contraintes);
			params.numberOfPeriods = period;
			params.outputNames = Conversions::toStdVector(outputs);
			params.outputLevel = outputLevel;
			params.outputMinPeriod = etanduSortiesMin;
			params.outputMaxPeriod = etanduSortiesMax;
			params.outputPath = Conversions::toStdString(cheminSorties);
			params.gdalProvider = Conversions::toStdString(providerGdal);

			// The table is converted as is; the Core interprets it (header, mask,
			// proportion). A missing table and an empty table give two distinct messages.
			params.proportionsTableProvided = ListeInformations != nullptr;

			if (params.proportionsTableProvided)
			{
				for each (System::Collections::Generic::List<System::String^>^ row in ListeInformations)
				{
					params.proportionsTable.push_back(Conversions::toStdVector(row));
				}
			}

			return FMTWrapper::Backend::Controller::runAreaVariability(params, scenario).success;
		}
		catch (...)
		{
			_raiseFromCatch("", "FMTForm::InitialAreaVariability", __LINE__, __FILE__);
			return false;
		}
	}
}
