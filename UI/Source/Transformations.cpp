#include "stdafx.h"
#include "FMTForm.h"
#include "FMTModelParser.h"
#include <msclr\marshal_cppstd.h>
#include "FMTFormCache.h"
#include "FMTModel.h"
#include "TransformationCore.h"

namespace Wrapper {

	bool FMTForm::aggregateAllActions(const int p_modelIndex, System::Collections::Generic::List<System::String^>^ p_aggregates, System::Collections::Generic::List<System::String^>^ p_order, System::String^ p_shedulePri, System::String^ p_outputDirPath, System::String^ p_scenario_name, System::String^ p_pri_name)
	{
		try
		{
			if (FMTFormCache::GetInstance()->empty())
			{
				throw std::out_of_range("Invalid model index");
			}

			std::vector<std::string> aggregates = {  };
			for (int i = 0; i < p_aggregates->Count; ++i)
			{
				aggregates.push_back(msclr::interop::marshal_as<std::string>(p_aggregates[i]));
			}
			std::vector<std::string> order = {  };
			for (int i = 0; i < p_order->Count; ++i)
			{
				order.push_back(msclr::interop::marshal_as<std::string>(p_order[i]));
			}
			const std::string PRIMARYLOCATION = msclr::interop::marshal_as<std::string>(p_shedulePri);
			const std::string SCENARIO_NAME = msclr::interop::marshal_as<std::string>(p_scenario_name);
			const std::string OUTPOURPATH = msclr::interop::marshal_as<std::string>(p_outputDirPath);
			const std::string PRINAME = msclr::interop::marshal_as<std::string>(p_pri_name);
			const std::vector<std::string> SCENARIOS(1, SCENARIO_NAME);
			const Models::FMTModel MODEL = { FMTFormCache::GetInstance()->getModel(p_modelIndex) };
			const Models::FMTModel aggregatedModel = FMTWrapperCore::Transformation::aggregateAllActions(MODEL, aggregates, order, PRIMARYLOCATION, SCENARIO_NAME);

			FMTFormCache::GetInstance()->push_back(aggregatedModel);
			return true;
		}
		catch (...)
		{
			raiseFromCatch("", "FMTModelaggregateAllActions", __LINE__, __FILE__);
			return false;
		}

	}
	bool FMTForm::splitActions(
		const int p_modelIndex, 
		System::String^ p_schedulePri, 
		System::Collections::Generic::List<System::String^>^ p_splitted, 
		System::Collections::Generic::List<System::String^>^ p_splitted_mask, 
		System::String^ p_outputDirPath, 
		System::String^ p_scenario_name, 
		System::String^ p_pri_name)
	{
		try
		{
			std::vector<std::string>splitted = {}; // Exemple : { "ACT","AEC"}
			std::vector<std::string>splitted_mask = {}; //Exemple : { "? ? ? ? ? ? ? ? ? ? ? ? ? UTA11 ? ? ? ? ? ", "? ? ? ? ? ? ? ? ? ? ? ? ? !UTA11 ? ? ? ? ? ?" };
			const std::string PRIMARYLOCATION = msclr::interop::marshal_as<std::string>(p_schedulePri);
			const std::string OUTPUT_DIRECTORY = msclr::interop::marshal_as<std::string>(p_outputDirPath);
			const std::string SCENARIO = msclr::interop::marshal_as<std::string>(p_scenario_name);
			const std::string PRINAME = msclr::interop::marshal_as<std::string>(p_pri_name);

			// on convertie en c++
			for (int i = 0; i < p_splitted->Count; ++i)
			{
				splitted.push_back(msclr::interop::marshal_as<std::string>(p_splitted[i]));
			}

			for (int i = 0; i < p_splitted_mask->Count; ++i)
			{
				splitted_mask.push_back(msclr::interop::marshal_as<std::string>(p_splitted_mask[i]));
			}


			if (FMTFormCache::GetInstance()->empty())
			{
				throw std::out_of_range("Invalid model index");
			}
			const Models::FMTModel MODEL = FMTFormCache::GetInstance()->getModel(p_modelIndex);

			Models::FMTModel SPLITTED_MODEL = FMTWrapperCore::Transformation::splitActions(
				MODEL, 
				PRIMARYLOCATION, 
				splitted, 
				splitted_mask, 
				SCENARIO);

			FMTFormCache::GetInstance()->push_back(SPLITTED_MODEL);
			return true;
		}
		catch (...)
		{
			raiseFromCatch("", "FMTModelaggregateAllActions", __LINE__, __FILE__);
			return false;
		}
	}

	System::Collections::Generic::List<System::String^>^ FMTForm::getAttributes(const int p_modelIndex, const int p_themeIndex) {
		// On d�fini notre variable qui va recevoir les r�sultats
		// bonne pratique de d�j� l'initialis� � nullptr au cas
		System::Collections::Generic::List<System::String^>^ results = gcnew System::Collections::Generic::List<System::String^>();
		try
		{
			if (FMTFormCache::GetInstance()->empty())
			{
				throw std::out_of_range("Invalid model index");
			}

			// On va chercher le mod�le avec tous les th�mes
			const Models::FMTModel& MODEL = FMTFormCache::GetInstance()->getModel(p_modelIndex);
			// On va chercher tous les th�mes dans le mod�le et le theme qui nous int�resse
			const Core::FMTTheme THEME = MODEL.getThemes().at(p_themeIndex);
			// On sort tous les attributes du th�me
			const std::vector<std::string>& ATTRIBUTES = THEME.getBaseAttributes();

			// On convertie pour le C#
			for (int i = 0; i < ATTRIBUTES.size(); ++i)
			{
				System::String^ convertedString = gcnew System::String(ATTRIBUTES[i].c_str());
				results->Add(convertedString);
			}
		}
		catch (...) {
			raiseFromCatch("", "FMTForm::getAttributes", __LINE__, __FILE__);
		}

		return results;
	}

	bool FMTForm::buildAction(const int p_modelIndex, System::String^ p_actionName, System::String^ p_targetYield, System::String^ p_schedulePri, System::String^ p_outputDirPath, System::String^ p_scenario_name, System::String^ p_pri_name)
	{
		bool passed = true;
		try
		{
			if (FMTFormCache::GetInstance()->empty())
			{
				passed = false;
				throw std::out_of_range("Invalid model index");
			}
			Parser::FMTModelParser ModelParser = FMTFormCache::GetInstance()->GetConfiguredParser();
			const std::string ACTION_NAME = msclr::interop::marshal_as<std::string>(p_actionName);
			const std::string TARGET_YIELD = msclr::interop::marshal_as<std::string>(p_targetYield);
			const std::string SCHEDULE_PRIMARYm_location = msclr::interop::marshal_as<std::string>(p_schedulePri);
			const std::string OUTPUT_DIRECTORY = msclr::interop::marshal_as<std::string>(p_outputDirPath);
			const std::string SCENARIO_NAME = msclr::interop::marshal_as<std::string>(p_scenario_name);
			const std::string PRI_NAME = msclr::interop::marshal_as<std::string>(p_pri_name);
			const std::vector<Models::FMTModel> MODELS = { FMTFormCache::GetInstance()->getModel(p_modelIndex) };

			const Models::FMTModel BUILDED_MODEL = FMTWrapperCore::Transformation::buildAction(MODELS.at(0), ACTION_NAME, TARGET_YIELD, SCHEDULE_PRIMARYm_location, SCENARIO_NAME);

			FMTFormCache::GetInstance()->push_back(BUILDED_MODEL);
		}
		catch (...) {
			raiseFromCatch("", "FMTForm::buildAction", __LINE__, __FILE__);
			passed = false;
		}
		return passed;
	}
}