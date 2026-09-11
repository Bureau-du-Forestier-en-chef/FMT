#include "stdafx.h"
#include <msclr\marshal_cppstd.h>

#include "FMTForm.h"
#include "ModelQuery.h"
#include "TransformationCore.h"

namespace Wrapper {

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

	bool FMTForm::aggregateAllActions(
		const int p_modelIndex,
		System::Collections::Generic::List<System::String^>^ p_aggregates,
		System::Collections::Generic::List<System::String^>^ p_order,
		System::String^ p_shedulePri,
		System::String^ p_outputDirPath,
		System::String^ p_scenario_name,
		System::String^ p_pri_name)
	{
		try
		{
			FMTWrapperCore::Transformation::aggregateAllActionsFromCache(
				p_modelIndex,
				_toStdVector(p_aggregates),
				_toStdVector(p_order),
				msclr::interop::marshal_as<std::string>(p_shedulePri),
				msclr::interop::marshal_as<std::string>(p_scenario_name));

			return true;
		}
		catch (...)
		{
			_raiseFromCatch("", "FMTModelaggregateAllActions", __LINE__, __FILE__);
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
			FMTWrapperCore::Transformation::splitActionsFromCache(
				p_modelIndex,
				msclr::interop::marshal_as<std::string>(p_schedulePri),
				_toStdVector(p_splitted),
				_toStdVector(p_splitted_mask),
				msclr::interop::marshal_as<std::string>(p_scenario_name));

			return true;
		}
		catch (...)
		{
			_raiseFromCatch("", "FMTModelaggregateAllActions", __LINE__, __FILE__);
			return false;
		}
	}

	System::Collections::Generic::List<System::String^>^ FMTForm::getAttributes(
		const int p_modelIndex,
		const int p_themeIndex)
	{
		System::Collections::Generic::List<System::String^>^ results =
			gcnew System::Collections::Generic::List<System::String^>();

		try
		{
			for (const std::string& ATTRIBUTE :
				FMTWrapperCore::ModelQuery::getThemeAttributes(p_modelIndex, p_themeIndex))
			{
				results->Add(gcnew System::String(ATTRIBUTE.c_str()));
			}
		}
		catch (...)
		{
			_raiseFromCatch("", "FMTForm::getAttributes", __LINE__, __FILE__);
		}

		return results;
	}

	bool FMTForm::buildAction(
		const int p_modelIndex,
		System::String^ p_actionName,
		System::String^ p_targetYield,
		System::String^ p_schedulePri,
		System::String^ p_outputDirPath,
		System::String^ p_scenario_name,
		System::String^ p_pri_name)
	{
		bool passed = true;

		try
		{
			FMTWrapperCore::Transformation::buildActionFromCache(
				p_modelIndex,
				msclr::interop::marshal_as<std::string>(p_actionName),
				msclr::interop::marshal_as<std::string>(p_targetYield),
				msclr::interop::marshal_as<std::string>(p_schedulePri),
				msclr::interop::marshal_as<std::string>(p_scenario_name));
		}
		catch (...)
		{
			_raiseFromCatch("", "FMTForm::buildAction", __LINE__, __FILE__);
			passed = false;
		}

		return passed;
	}
}
