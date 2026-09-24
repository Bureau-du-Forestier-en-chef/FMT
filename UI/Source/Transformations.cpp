#include "stdafx.h"
#include <msclr\marshal_cppstd.h>

#include "FMTForm.h"
#include "Controller.h"
#include "Conversions.h"

namespace Wrapper {

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
			FMTWrapper::Backend::Controller::aggregateAllActions(
				p_modelIndex,
				Conversions::toStdVector(p_aggregates),
				Conversions::toStdVector(p_order),
				Conversions::toStdString(p_shedulePri),
				Conversions::toStdString(p_scenario_name));

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
			FMTWrapper::Backend::Controller::splitActions(
				p_modelIndex,
				Conversions::toStdString(p_schedulePri),
				Conversions::toStdVector(p_splitted),
				Conversions::toStdVector(p_splitted_mask),
				Conversions::toStdString(p_scenario_name));

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
				FMTWrapper::Backend::Controller::getThemeAttributes(p_modelIndex, p_themeIndex))
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
			FMTWrapper::Backend::Controller::buildAction(
				p_modelIndex,
				Conversions::toStdString(p_actionName),
				Conversions::toStdString(p_targetYield),
				Conversions::toStdString(p_schedulePri),
				Conversions::toStdString(p_scenario_name));
		}
		catch (...)
		{
			_raiseFromCatch("", "FMTForm::buildAction", __LINE__, __FILE__);
			passed = false;
		}

		return passed;
	}
}
