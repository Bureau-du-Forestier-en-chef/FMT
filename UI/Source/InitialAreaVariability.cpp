#include "stdafx.h"
#include <msclr\marshal_cppstd.h>

#include "FMTForm.h"
#include "Controller.h"

namespace Wrapper
{
	namespace
	{
		// Les nullptr deviennent des valeurs vides : c'est le Core qui refuse alors
		// l'opération avec ses propres messages.
		std::string _toStdString(System::String^ p_value)
		{
			return p_value == nullptr
				? std::string()
				: msclr::interop::marshal_as<std::string>(p_value);
		}

		std::vector<std::string> _toStdVector(
			System::Collections::Generic::List<System::String^>^ p_values)
		{
			std::vector<std::string> converted;

			if (p_values != nullptr)
			{
				for each (System::String ^ value in p_values)
				{
					converted.push_back(_toStdString(value));
				}
			}

			return converted;
		}
	}

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
		// fichierPri n'a jamais été utilisé : il reste dans la signature publique,
		// dont dépend le UI .NET.
		try
		{
			FMTWrapperCore::AreaVariabilityParameters params;
			params.solver = solver;
			params.constraintNames = _toStdVector(contraintes);
			params.numberOfPeriods = period;
			params.outputNames = _toStdVector(outputs);
			params.outputLevel = outputLevel;
			params.outputMinPeriod = etanduSortiesMin;
			params.outputMaxPeriod = etanduSortiesMax;
			params.outputPath = _toStdString(cheminSorties);
			params.gdalProvider = _toStdString(providerGdal);

			// La table est convertie telle quelle ; le Core l'interprète (en-tête, masque,
			// proportion). Table absente et table vide y donnent deux messages distincts.
			params.proportionsTableProvided = ListeInformations != nullptr;

			if (params.proportionsTableProvided)
			{
				for each (System::Collections::Generic::List<System::String^>^ row in ListeInformations)
				{
					params.proportionsTable.push_back(_toStdVector(row));
				}
			}

			return FMTWrapperCore::Controller::runAreaVariability(params, scenario).success;
		}
		catch (...)
		{
			_raiseFromCatch("", "FMTForm::InitialAreaVariability", __LINE__, __FILE__);
			return false;
		}
	}
}
