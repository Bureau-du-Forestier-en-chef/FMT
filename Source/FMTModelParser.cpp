/*
Copyright (c) 2019 Gouvernement du Qu?bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTModelParser.h"
#include "FMTParser.h"
#include "FMTConstantParser.h"
#include "FMTLandscapeParser.h"
#include "FMTAreaParser.h"
#include "FMTYieldParser.h"
#include "FMTActionParser.h"
#include "FMTTransitionParser.h"
#include "FMTLifespanParser.h"
#include "FMTOutputParser.h"
#include "FMTOptimizationParser.h"
#include "FMTScheduleParser.h"
#include "FMTModel.h"
#include "FMTSchedule.h"
#include "FMTConstraint.h"
#include "FMTOutput.h"
#include <map>
#include "FMTExceptionHandler.h"
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <array>
#include <streambuf>


#ifdef FMTWITHGDAL
#include "gdal.h"
#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#endif 

namespace Parser {


	FMTModelParser::FMTModelParser() :FMTParser()
	{

	}

	FMTModelParser::FMTModelParser(const FMTModelParser& rhs) : FMTParser(rhs)
	{

	}
	FMTModelParser& FMTModelParser::operator = (const FMTModelParser& rhs)
	{
		if (this != &rhs)
		{
			FMTParser::operator=(rhs);
		}
		return *this;
	}

#ifdef FMTWITHGDAL

	OGRLayer* FMTModelParser::createDriftLayer(GDALDataset* dataset, std::vector<std::string> creationoptions) const
	{
		OGRLayer* newlayer = nullptr;
		try {
			newlayer = createLayer(dataset, "outputsdrift", creationoptions);
			OGRFieldDefn PeriodField("Period", OFTInteger);
			PeriodField.SetWidth(32);
			OGRFieldDefn OutputField("Output", OFTString);
			OutputField.SetWidth(254);
			OGRFieldDefn driftfield("Drift", OFTReal);
			driftfield.SetPrecision(5);
			driftfield.SetWidth(32);
			OGRFieldDefn LowerProbabilityField("LowerProbability", OFTReal);
			LowerProbabilityField.SetPrecision(5);
			LowerProbabilityField.SetWidth(32);
			OGRFieldDefn UpperProbabilityField("UpperProbability", OFTReal);
			UpperProbabilityField.SetPrecision(5);
			UpperProbabilityField.SetWidth(32);
			if (newlayer->CreateField(&PeriodField) != OGRERR_NONE ||
				newlayer->CreateField(&OutputField) != OGRERR_NONE ||
				newlayer->CreateField(&LowerProbabilityField) != OGRERR_NONE ||
				newlayer->CreateField(&UpperProbabilityField) != OGRERR_NONE ||
				newlayer->CreateField(&driftfield) != OGRERR_NONE)
			{
				_exhandler->raise(Exception::FMTexc::FMTgdal_constructor_error,
					"Cannote create new fields outputsdrift", "FMTModelParser::createDriftLayer", __LINE__, __FILE__, m_section);
				//Cannot create field
			}

		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTModelParser::createDriftLayer", __LINE__, __FILE__);
		}
		return newlayer;
	}

	void FMTModelParser::writeDrift(OGRLayer* layer, const std::map<std::string, std::map<double, std::vector<double>>>& lowervalues,
		const std::map<std::string, std::map<double, std::vector<double>>>& uppervalues) const
	{
		try {
			for (const auto& toutputvalues : lowervalues)
			{
				size_t outputid = 0;
				for (const auto& driftvalues : toutputvalues.second)
				{
					int period = 1;
					for (const double& probability : driftvalues.second)
					{
						OGRFeature* newfeature = OGRFeature::CreateFeature(layer->GetLayerDefn());
						if (newfeature == NULL)
						{
							_exhandler->raise(Exception::FMTexc::FMTgdal_constructor_error,
								"Cannote generate new feature ", "FMTModelParser::writeFeatures", __LINE__, __FILE__, m_section);
							//Failed to generate feature
						}
						newfeature->SetField("Period", period);
						newfeature->SetField("Output", toutputvalues.first.c_str());
						newfeature->SetField("Drift", driftvalues.first);
						newfeature->SetField("LowerProbability", probability);
						newfeature->SetField("UpperProbability", uppervalues.at(toutputvalues.first).at(driftvalues.first).at(period - 1));
						if (layer->CreateFeature(newfeature) != OGRERR_NONE)
						{
							_exhandler->raise(Exception::FMTexc::FMTgdal_constructor_error,
								"Cannote create new feature id " + std::to_string(layer->GetFeatureCount()), "FMTModelParser::writeFeatures", __LINE__, __FILE__, m_section);
							//Failed to generate feature
						}
						OGRFeature::DestroyFeature(newfeature);
						++period;
					}
				}
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTModelParser::writeDrift", __LINE__, __FILE__);
		}
	}

	std::map<std::string, std::vector<std::vector<double>>>FMTModelParser::getIterationsValues(OGRLayer* layer) const
	{
		std::map<std::string, std::vector<std::vector<double>>>results;
		try {
			OGRFeature* feature;
			std::map<std::string, std::map<int, std::map<int, double>>> sortedresults;
			while ((feature = layer->GetNextFeature()) != NULL)
			{
				const int iteration = feature->GetFieldAsInteger("Iteration");
				const int period = feature->GetFieldAsInteger("Period");
				std::string output = feature->GetFieldAsString("Output");
				const std::string TYPE = feature->GetFieldAsString("Type");
				if (!TYPE.empty() &&
					TYPE!="Total")
					{
					output += "(" + TYPE + ")";
					}
				const double value = feature->GetFieldAsDouble("Value");
				if (sortedresults.find(output) == sortedresults.end())
				{
					sortedresults[output] = std::map<int, std::map<int, double>>();
				}
				if (sortedresults.at(output).find(period) == sortedresults.at(output).end())
				{
					sortedresults[output][period] = std::map<int, double>();
				}
				if (sortedresults.at(output).at(period).find(iteration) == sortedresults.at(output).at(period).end())
				{
					sortedresults[output][period][iteration] = 0;
				}
				sortedresults[output][period][iteration] += value;
				OGRFeature::DestroyFeature(feature);
			}
			for (const auto& outresult : sortedresults)
			{
				results[outresult.first] = std::vector<std::vector<double>>();
				for (const auto& outperiod : outresult.second)
				{
					std::vector<double>values;
					for (const auto& outvalue : outperiod.second)
					{
						values.push_back(outvalue.second);
					}
					results[outresult.first].push_back(values);
				}
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTModelParser::getIterationsValues", __LINE__, __FILE__);
		}
		return results;
	}


	OGRLayer* FMTModelParser::createResultsLayer(const std::string& p_modelName,
		GDALDataset* p_dataset,
		std::vector<std::string> p_creationOptions)const
	{
		OGRLayer* newlayer = nullptr;
		try {
			newlayer = createLayer(p_dataset, p_modelName, p_creationOptions);
			OGRFieldDefn IterationField("Iteration", OFTInteger);
			IterationField.SetWidth(32);
			OGRFieldDefn PeriodField("Period", OFTInteger);
			PeriodField.SetWidth(32);
			OGRFieldDefn OutputField("Output", OFTString);
			OutputField.SetWidth(254);
			OGRFieldDefn TypeField("Type", OFTString);
			TypeField.SetWidth(254);
			OGRFieldDefn ValueField("Value", OFTReal);
			ValueField.SetPrecision(5);
			ValueField.SetWidth(32);
			std::array<OGRFieldDefn*, 5>FieldDefinitions = { &IterationField ,&PeriodField ,&OutputField ,&TypeField, &ValueField };
			for (OGRFieldDefn* ValidDefinition : FieldDefinitions)
				{
				if (newlayer->CreateField(ValidDefinition) != OGRERR_NONE) 
					{
					_exhandler->raise(Exception::FMTexc::FMTgdal_constructor_error, 
						"Cannote create new field " + std::string(ValidDefinition->GetNameRef()), "FMTModelParser::writeResults", __LINE__, __FILE__, m_section);
					}

				}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTModelParser::createResultsLayer", __LINE__, __FILE__, m_section);
		}
		return newlayer;
	}

	void FMTModelParser::fillUpInfeasibles(OGRLayer* layer,
		const std::vector<Core::FMTOutput>& theoutputs,
		const int& iteration, const int& firstPeriod, const int& lastPeriod) const
	{
		try {
			boost::lock_guard<boost::recursive_mutex> guard(m_MTX);
			std::map<std::string, std::vector<std::vector<double>>>allvalues;
			//Cannot fill up infeasible for non-total ... i guess
			allvalues["Total"] = std::vector<std::vector<double>>(theoutputs.size(), std::vector<double>((lastPeriod - firstPeriod) + 1, std::numeric_limits<double>::quiet_NaN()));
			writeFeatures(layer, firstPeriod, iteration, theoutputs, allvalues, true);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTModelParser::fillUpInfeasibles", __LINE__, __FILE__, m_section);
		}

	}

	void FMTModelParser::writeFeatures(OGRLayer* layer, const int& firstPeriod, const int& iteration,
		const std::vector<Core::FMTOutput>& theoutputs,
		const std::map<std::string, std::vector<std::vector<double>>>& values, bool writeNaN)const
	{
		try {
			boost::lock_guard<boost::recursive_mutex> guard(m_MTX);
			for (const auto& toutputvalues : values)
			{
				size_t outputid = 0;
				for (const std::vector<double>& outputvalues : toutputvalues.second)
				{
					int period = firstPeriod;
					for (const double& value : outputvalues)
					{
						if (!std::isnan(value) || writeNaN)
						{
							std::string outputtype = "Total";
							if (!std::isnan(value))
							{
								outputtype = toutputvalues.first.c_str();
							}
							OGRFeature* newfeature = OGRFeature::CreateFeature(layer->GetLayerDefn());
							if (newfeature == NULL)
							{
								_exhandler->raise(Exception::FMTexc::FMTgdal_constructor_error,
									"Cannote generate new feature ", "FMTModelParser::writeFeatures", __LINE__, __FILE__, m_section);
								//Failed to generate feature
							}
							newfeature->SetField("Iteration", iteration);
							newfeature->SetField("Period", period);
							newfeature->SetField("Output", theoutputs.at(outputid).getName().c_str());
							newfeature->SetField("Type", outputtype.c_str());
							newfeature->SetField("Value", value);
							if (layer->CreateFeature(newfeature) != OGRERR_NONE)
							{
								_exhandler->raise(Exception::FMTexc::FMTgdal_constructor_error,
									"Cannote create new feature id " + std::to_string(layer->GetFeatureCount()), "FMTModelParser::writeFeatures", __LINE__, __FILE__, m_section);
								//Failed to generate feature
							}
							OGRFeature::DestroyFeature(newfeature);
						}
						++period;
					}
					++outputid;
				}

			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTModelParser::writeFeatures", __LINE__, __FILE__, m_section);
		}

	}

	void FMTModelParser::writeResults(const Models::FMTModel& model,
		const std::vector<Core::FMTOutput>& theoutputs,
		const int& firstPeriod, const int& lastPeriod,
		const std::string& location,
		Core::FMToutputlevel level,
		std::string gdaldrivername) const
	{
		try {
			GDALDataset* newdataset = createOGRDataset(location, gdaldrivername);
			OGRLayer* newlayer = createResultsLayer(model.getName(), newdataset);
			writeFeatures(newlayer, firstPeriod, 0, theoutputs, model.getOutputsFromPeriods(theoutputs, firstPeriod, lastPeriod, level));
			GDALClose(newdataset);
		}
		catch (...)
		{
			_exhandler->printExceptions(" at " + location, "FMTModelParser::writeResults", __LINE__, __FILE__, m_section);
		}
	}
#endif 


	void FMTModelParser::writePrimary(
		const std::string& location,
		const std::string& lanfile,
		const std::string& arefile,
		const std::string& yldfile,
		const std::string& actfile,
		const std::string& trnfile,
		const std::string& outfile,
		const std::string& optfile,
		const std::string& liffile,
		std::string seqfile) const
	{
		try {
			std::ofstream pristream;
			pristream.open(location);
			if (tryOpening(pristream, location))
			{
				pristream << "LANDSCAPE\t\t[" + lanfile + "]\n";
				pristream << "AREAS\t[" + arefile + "]\n";
				pristream << "YIELDS\t[" + yldfile + "]\n";
				pristream << "ACTIONS\t[" + actfile + "]\n";
				pristream << "TRANSITIONS\t\t[" + trnfile + "]\n";
				pristream << "LIFESPAN\t\t[" + liffile + "]\n";
				pristream << "OUTPUTS\t[" + outfile + "]\n";
				pristream << "OPTIMIZE\t\t[" + optfile + "]\n";
				if (!seqfile.empty())
				{
					pristream << "SCHEDULE\t[" + seqfile + "]\n";
				}
			}
			pristream.close();
		}
		catch (...)
		{
			_exhandler->printExceptions(" at " + location, "FMTModelParser::writePrimary", __LINE__, __FILE__, m_section);
		}
	}

	void  FMTModelParser::writeModel(const Models::FMTModel& model,
		const std::string& lanfile,
		const std::string& arefile,
		const std::string& yldfile,
		const std::string& actfile,
		const std::string& trnfile,
		const std::string& outfile,
		const std::string& optfile,
		const std::string& liffile,
		const std::string& seqfile) const
	{
	try {
		if (!lanfile.empty())
		{
			FMTLandscapeParser landparser;
			landparser.write(model.getThemes(), lanfile);
		}
		if (!arefile.empty()){
			const std::vector<Core::FMTActualDevelopment>devs = model.getArea();
			if (!devs.empty())
			{
				FMTAreaParser areaparser;
				double sumarea = 0;
				for (const Core::FMTActualDevelopment& dev : devs)
				{
					sumarea += dev.getArea();
				}
				const std::string header = "Total area: " + std::to_string(sumarea) + "\n";
				areaparser.setHeader(header);
				areaparser.write(devs, arefile);
			}
		}
		if (!yldfile.empty())
		{
			FMTYieldParser yldparser;
			yldparser.write(model.getYields(), yldfile);
		}

		if (!actfile.empty())
		{
			FMTActionParser actparser;
			actparser.write(model.getactions(), actfile);
		}
		
		if (!trnfile.empty())
		{
			FMTTransitionParser trnparser;
			trnparser.write(model.getTransitions(), trnfile);
		}
		if (!liffile.empty())
		{
			FMTLifespanParser lifparser;
			lifparser.write(model.getLifespan(), liffile);
		}
		if (!outfile.empty())
		{
			const std::vector<Core::FMTOutput>outputs = model.getOutputs();
			if (!outputs.empty())
			{
				FMTOutputParser outparser;
				outparser.write(outputs, outfile);
			}
		}
		if (!optfile.empty())
		{
			const std::vector<Core::FMTConstraint>constraints = model.getconstraints();
			if (!constraints.empty())
			{
				FMTOptimizationParser optparser;
				optparser.write(constraints, optfile);
			}
		}
		std::vector<Core::FMTSchedule>schedules;
		for (int period = 1; period <= model.getParameter(Models::FMTintmodelparameters::LENGTH); ++period)
		{
			const Core::FMTSchedule periodschedule = model.getSolution(period, true);
			if (!periodschedule.empty())
			{
				schedules.push_back(periodschedule);
			}
		}
		if (!seqfile.empty())
		{
			FMTScheduleParser scheduleparser;
			std::string status = "Non Optimal";
			std::string ObjectiveValue = "NA";
			if (model.isOptimal())
				{
				status = "Optimal";
				ObjectiveValue =  std::to_string(model.getObjectiveValue());
				}
			const std::string modelobjective = "Objective value: " + ObjectiveValue + "\n";
			const std::string modelstatus = "Status: " + status + "\n";
			scheduleparser.setHeader(modelstatus+ modelobjective);
			scheduleparser.write(schedules, seqfile, false);
		}
	}catch (...)
	{
		_exhandler->printExceptions("", "FMTModelParser::writeModel", __LINE__, __FILE__, m_section);
	}
}



void FMTModelParser::write(const Models::FMTModel& model,const std::string& folder) const
    {
	try {
		//Ajout de la section pri
		//retirer les aggr�gats de BFECgcbm et �crire les contraintes sans les 
		const std::string modelname = model.getName();
		const std::string lanname = modelname + ".lan";
		const std::string arename = modelname + ".are";
		const std::string yldname = modelname + ".yld";
		const std::string actname = modelname + ".act";
		const std::string trnname = modelname + ".trn";
		const std::string outname = modelname + ".out";
		const std::string optname = modelname + ".opt";
		const std::string lifname = modelname + ".lif";
		const std::string seqname = modelname + ".seq";
		const std::string lanfile = folder + lanname;
		const std::string arefile = folder + arename;
		const std::string yldfile = folder + yldname;
		const std::string actfile = folder + actname;
		const std::string trnfile = folder + trnname;
		const std::string outfile = folder + outname;
		const std::string optfile = folder + optname;
		const std::string liffile = folder + lifname;
		const std::string seqfile = folder + seqname;
			writePrimary(folder + modelname + ".pri",
				lanname, arename, yldname, actname, trnname, outname, optname, lifname, seqname);
			writeModel(model, lanfile, arefile, yldfile, actfile, trnfile, outfile, optfile, liffile, seqfile);
		
	}catch (...)
		{
		_exhandler->printExceptions(" at " + folder, "FMTModelParser::write", __LINE__, __FILE__, m_section);
		}
	}

	Models::FMTModel FMTModelParser::read(const std::string& con, const std::string& lan,
		const std::string& lif, const std::string& are, const std::string& yld,
		const std::string& act, const std::string& trn, const std::string& out, std::string opt)
	{
		try {
			std::map<std::string, std::vector<int>> commons;
			std::vector<Models::FMTModel>models;
			return referenceRead(commons, models,
				con, lan, lif, are, yld, act, trn, out, opt);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTModelParser::read", __LINE__, __FILE__, m_section);
		}
		return Models::FMTModel();
	}

	std::vector<Core::FMTConstraint>FMTModelParser::getConstraintsFromString(std::string constraintstr, const Models::FMTModel& model, Core::FMTConstants constants)
	{
		std::vector<Core::FMTConstraint>constraints;
		try {
			FMTOptimizationParser optparser;
			boost::to_upper(constraintstr);
			for (Core::FMTConstraint constraint : optparser.getConstraints(constraintstr, constants,model.yields,
																			model.outputs, model.themes, model.actions))
			{
				//constraint.passinobject(model);
				constraints.push_back(constraint);
			}
		}
		catch (...)
		{
			_exhandler->printExceptions("While getting constraint from " + constraintstr, "FMTModelParser::getConstraintsFromString", __LINE__, __FILE__, m_section);
		}
		return constraints;
	}


	Models::FMTModel FMTModelParser::referenceRead(std::map<std::string, std::vector<int>>& commonm_sections,
		std::vector<Models::FMTModel>& models,
		const std::string& con, const std::string& lan,
		const std::string& lif, const std::string& are, const std::string& yld,
		const std::string& act, const std::string& tr, const std::string& out,
		std::string opt, bool allow_mapping)
	{
		Models::FMTModel returnedmodel;
		try {
			Core::FMTConstants constants;
			std::vector<Core::FMTTheme>themes;
			std::vector<Core::FMTActualDevelopment>areas;
			Core::FMTYields yields;
			Core::FMTLifespans lifespan;
			std::vector<Core::FMTAction>actions;
			std::vector<Core::FMTTransition>transitions;
			std::vector<Core::FMTOutput>outputs;
			std::vector<Core::FMTConstraint>constraints;
			if (allow_mapping)
			{
				std::map<std::string, std::vector<int>>::const_iterator constants_it = commonm_sections.find(con);
				std::map<std::string, std::vector<int>>::const_iterator themes_it = commonm_sections.find(lan);
				std::map<std::string, std::vector<int>>::const_iterator lifespan_it = commonm_sections.find(lif);
				std::map<std::string, std::vector<int>>::const_iterator area_it = commonm_sections.find(are);
				std::map<std::string, std::vector<int>>::const_iterator yield_it = commonm_sections.find(yld);
				std::map<std::string, std::vector<int>>::const_iterator actions_it = commonm_sections.find(act);
				std::map<std::string, std::vector<int>>::const_iterator transitions_it = commonm_sections.find(tr);
				std::map<std::string, std::vector<int>>::const_iterator outputs_it = commonm_sections.find(out);
				std::map<std::string, std::vector<int>>::const_iterator optimize_it = commonm_sections.find(opt);
				std::vector<int>::iterator common_it;
				if (constants_it != commonm_sections.end() && themes_it != commonm_sections.end())
				{
					std::vector<int>common_sets(themes_it->second.size() + constants_it->second.size());
					common_it = std::set_intersection(themes_it->second.begin(), themes_it->second.end(),
						constants_it->second.begin(), constants_it->second.end(), common_sets.begin());
					common_sets.resize(common_it - common_sets.begin());
					if (!common_sets.empty())
					{
						std::sort(common_sets.begin(), common_sets.end());
						themes = models.at(*common_sets.begin()).getThemes();
						if (area_it != commonm_sections.end())
						{
							std::vector<int>common_area(common_sets.size() + area_it->second.size());
							common_it = std::set_intersection(common_sets.begin(), common_sets.end(),
								area_it->second.begin(), area_it->second.end(), common_area.begin());
							common_area.resize(common_it - common_area.begin());
							if (!common_area.empty())
							{
								std::sort(common_area.begin(), common_area.end());
								areas = models.at(*common_area.begin()).getArea();
							}
						}
						if (yield_it != commonm_sections.end())
						{
							std::vector<int>common_yield(common_sets.size() + yield_it->second.size());
							common_it = std::set_intersection(common_sets.begin(), common_sets.end(),
								yield_it->second.begin(), yield_it->second.end(), common_yield.begin());
							common_yield.resize(common_it - common_yield.begin());
							if (!common_yield.empty())
							{
								std::sort(common_yield.begin(), common_yield.end());
								yields = models.at(*common_yield.begin()).getYields();
							}
						}
						if (lifespan_it != commonm_sections.end())
						{
							std::vector<int>common_lif(common_sets.size() + lifespan_it->second.size());
							common_it = std::set_intersection(common_sets.begin(), common_sets.end(),
								lifespan_it->second.begin(), lifespan_it->second.end(), common_lif.begin());
							common_lif.resize(common_it - common_lif.begin());
							if (!common_lif.empty())
							{
								std::sort(common_lif.begin(), common_lif.end());
								lifespan = models.at(*common_lif.begin()).getLifespan();
								if (actions_it != commonm_sections.end())
								{
									std::vector<int>common_actions(common_lif.size() + actions_it->second.size());
									common_it = std::set_intersection(common_lif.begin(), common_lif.end(),
										actions_it->second.begin(), actions_it->second.end(), common_actions.begin());
									common_actions.resize(common_it - common_actions.begin());
									if (!common_actions.empty())
									{
										std::sort(common_actions.begin(), common_actions.end());
										actions = models.at(*common_actions.begin()).getactions();
										if (transitions_it != commonm_sections.end())
										{
											std::vector<int>common_transitions(common_actions.size() + transitions_it->second.size());
											common_it = std::set_intersection(common_actions.begin(), common_actions.end(),
												transitions_it->second.begin(), transitions_it->second.end(), common_transitions.begin());
											common_transitions.resize(common_it - common_transitions.begin());
											if (!common_transitions.empty())
											{
												std::sort(common_transitions.begin(), common_transitions.end());
												transitions = models.at(*common_transitions.begin()).getTransitions();
											}
										}
									}
								}
							}
						}

						if (outputs_it != commonm_sections.end())//should be with action??
						{
							std::vector<int>common_output(common_sets.size() + outputs_it->second.size());
							common_it = std::set_intersection(common_sets.begin(), common_sets.end(),
								outputs_it->second.begin(), outputs_it->second.end(), common_output.begin());
							common_output.resize(common_it - common_output.begin());
							if (!common_output.empty())
							{
								std::sort(common_output.begin(), common_output.end());
								outputs = models.at(*common_output.begin()).getOutputs();
								if (optimize_it != commonm_sections.end())
								{
									std::vector<int>common_optimize(common_output.size() + optimize_it->second.size());
									common_it = std::set_intersection(common_output.begin(), common_output.end(),
										optimize_it->second.begin(), optimize_it->second.end(), common_optimize.begin());
									common_optimize.resize(common_it - common_optimize.begin());
									if (!common_optimize.empty())
									{
										std::sort(common_optimize.begin(), common_optimize.end());
										constraints = models.at(*common_optimize.begin()).getconstraints();
									}
								}
							}
						}
					}
				}

				const int modelm_location = static_cast<int>(models.size());
				if (constants_it == commonm_sections.end())
				{
					commonm_sections[con] = std::vector<int>();
				}
				commonm_sections[con].push_back(modelm_location);
				if (themes_it == commonm_sections.end())
				{
					commonm_sections[lan] = std::vector<int>();
				}
				commonm_sections[lan].push_back(modelm_location);
				if (area_it == commonm_sections.end())
				{
					commonm_sections[are] = std::vector<int>();
				}
				commonm_sections[are].push_back(modelm_location);
				if (lifespan_it == commonm_sections.end())
				{
					commonm_sections[lif] = std::vector<int>();
				}
				commonm_sections[lif].push_back(modelm_location);
				if (yield_it == commonm_sections.end())
				{
					commonm_sections[yld] = std::vector<int>();
				}
				commonm_sections[yld].push_back(modelm_location);
				if (actions_it == commonm_sections.end())
				{
					commonm_sections[act] = std::vector<int>();
				}
				commonm_sections[act].push_back(modelm_location);
				if (transitions_it == commonm_sections.end())
				{
					commonm_sections[tr] = std::vector<int>();
				}
				commonm_sections[tr].push_back(modelm_location);
				if (outputs_it == commonm_sections.end())
				{
					commonm_sections[out] = std::vector<int>();
				}
				commonm_sections[out].push_back(modelm_location);
				if (optimize_it == commonm_sections.end())
				{
					commonm_sections[opt] = std::vector<int>();
				}
				commonm_sections[opt].push_back(modelm_location);
			}
			const boost::filesystem::path landfile(lan);
			const std::string modelname = landfile.stem().string();

			FMTConstantParser cparser;
			//cparser.passinobject(*this);
			constants = cparser.read(con);
			m_MostRecentFile = std::max(cparser.getMostRecentFiletime(), m_MostRecentFile);
			if (themes.empty())
			{
				FMTLandscapeParser landparser;
				//landparser.passinobject(*this);
				themes = landparser.read(constants, lan);
				m_MostRecentFile = std::max(landparser.getMostRecentFiletime(), m_MostRecentFile);
			}
			if (areas.empty())
			{
				FMTAreaParser areaparser;
				//areaparser.passinobject(*this);
				areas = areaparser.read(themes, constants, are);
				m_MostRecentFile = std::max(areaparser.getMostRecentFiletime(), m_MostRecentFile);
			}
			if (lifespan.empty())
			{
				FMTLifespanParser lifespanparser;
				//lifespanparser.passinobject(*this);
				lifespan = lifespanparser.read(themes, constants, lif);
				m_MostRecentFile = std::max(lifespanparser.getMostRecentFiletime(), m_MostRecentFile);
			}
			if (yields.empty())
			{
				FMTYieldParser yldparser;
				//yldparser.passinobject(*this);
				yields = yldparser.read(themes, constants, yld);
				m_MostRecentFile = std::max(yldparser.getMostRecentFiletime(), m_MostRecentFile);
			}
			if (actions.empty())
			{
				FMTActionParser actparser;
				//actparser.passinobject(*this);
				actions = actparser.read(themes, yields, constants, act);
				if (find_if(actions.begin(), actions.end(), Core::FMTActionComparator("_DEATH")) == actions.end())
				{
					_exhandler->raise(Exception::FMTexc::FMTundefineddeathaction,
						"_DEATH", "FMTModelParser::referenceRead", __LINE__, __FILE__, Core::FMTsection::Action);
					actions.push_back(Models::FMTModel::defaultDeathAction(lifespan, themes));
				}
				m_MostRecentFile = std::max(actparser.getMostRecentFiletime(), m_MostRecentFile);

			}
			if (transitions.empty())
			{
				FMTTransitionParser trnparser;
				//trnparser.passinobject(*this);
				transitions = trnparser.read(themes, actions, yields, constants, tr);
				if (find_if(transitions.begin(), transitions.end(), Core::FMTTransitionComparator("_DEATH")) == transitions.end())
				{
					_exhandler->raise(Exception::FMTexc::FMTundefineddeathtransition,
						"_DEATH", "FMTModelParser::referenceRead", __LINE__, __FILE__, Core::FMTsection::Transition);
					transitions.push_back(Models::FMTModel::defaultDeathTransition(lifespan, themes));
				}
				m_MostRecentFile = std::max(trnparser.getMostRecentFiletime(), m_MostRecentFile);
			}
			if (outputs.empty())
			{
				FMTOutputParser outparser;
				//outparser.passinobject(*this);
				outputs = outparser.read(themes, actions, yields, constants, out);
				m_MostRecentFile = std::max(outparser.getMostRecentFiletime(), m_MostRecentFile);
			}
			if (!opt.empty() && constraints.empty())
			{

				FMTOptimizationParser optzparser;
				std::vector<Core::FMTAction>excluded(actions); //should we realy use? excluded is actualy the same actions but with more period specification...
				//optzparser.passinobject(*this);
				constraints = optzparser.read(themes, actions, constants, outputs,yields, excluded, opt);
				m_MostRecentFile = std::max(optzparser.getMostRecentFiletime(), m_MostRecentFile);
				bool shouldcrapreference = (actions.size() != excluded.size());
				if (!shouldcrapreference)
				{
					size_t location = 0;
					for (const Core::FMTAction& action : actions)
					{
						if (action != excluded.at(location))
						{
							shouldcrapreference = true;
							break;
						}
						++location;
					}
				}
				if (shouldcrapreference)//send a signal to make sure no other model reference to this one!
				{//This model is not suppose to be considered common!
					commonm_sections[act].pop_back();
					if (commonm_sections.at(act).empty())
					{
						commonm_sections.erase(act);
					}
					commonm_sections[tr].pop_back();
					if (commonm_sections.at(tr).empty())
					{
						commonm_sections.erase(tr);
					}
				}
				actions = excluded; //here we go
			}
			returnedmodel = Models::FMTModel(areas, themes, actions,
				transitions, yields, lifespan, modelname, outputs, constraints);
			if (!opt.empty())
				{
				setSolverParameters(returnedmodel, opt);
				}
			//returnedmodel.passinobject(*this);
			returnedmodel.cleanActionsNTransitions();
			if (allow_mapping)
			{
				models.push_back(returnedmodel);
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTModelParser::referenceRead", __LINE__, __FILE__);
		}
		return returnedmodel;
	}

		void FMTModelParser::writeToProject(const std::string& primarym_location,
			const Models::FMTModel& model)
		{
			try {
				boost::filesystem::path primpath(primarym_location);
				const std::string filename = primpath.stem().string();
				const boost::filesystem::path basefolder = primpath.parent_path();
				if (!boost::filesystem::is_directory(basefolder))
					{
					_exhandler->raise(Exception::FMTexc::FMTinvalid_path,
						basefolder.string()+" is not a valid directory", "FMTModelParser::writeToProject", __LINE__, __FILE__);
					}
				if (!boost::filesystem::is_regular_file(primpath))//create the primary file
				{
					const std::string commonlocation = basefolder.string()+"/";
					const std::string clanname = filename + ".lan";
					const std::string carename = filename + ".are";
					const std::string cyldname = filename + ".yld";
					const std::string cactname = filename + ".act";
					const std::string ctrnname = filename + ".trn";
					const std::string coutname = filename + ".out";
					const std::string coptname = filename + ".opt";
					const std::string clifname = filename + ".lif";
					const std::string cseqname = filename + ".seq";
					const std::string clanfile = commonlocation + clanname;
					const std::string carefile = commonlocation + carename;
					const std::string cyldfile = commonlocation + cyldname;
					const std::string cactfile = commonlocation + cactname;
					const std::string ctrnfile = commonlocation + ctrnname;
					const std::string coutfile = commonlocation + coutname;
					const std::string coptfile = commonlocation + coptname;
					const std::string cliffile = commonlocation + clifname;
					const std::string cseqfile = commonlocation + cseqname;
					if (_logger->logWithLevel("Writing " + primarym_location + " ", 0))
					{
						*_logger << "\n";
						_logger->logStamp();
						_logger->logTime();
					}
					writePrimary(primarym_location,
						clanname, carename, cyldname, cactname, ctrnname, coutname, coptname, clifname, cseqname);
					writeModel(model, clanfile, carefile, cyldfile, cactfile, ctrnfile, coutfile, coptfile, cliffile, cseqfile);
				}else {//read the existing ROOT model...
					const std::vector<std::string>rootmodel(1,"ROOT");
					const std::vector<Models::FMTModel> models = readproject(primarym_location, rootmodel);
					if (models.empty())
					{
						_exhandler->raise(Exception::FMTexc::FMTrangeerror,
							"No Root model for the primary "+ primarym_location, "FMTModelParser::writeToProject", __LINE__, __FILE__);
					}
					if (!boost::filesystem::is_directory(basefolder / "Scenarios"))
						{
						boost::filesystem::create_directory(basefolder / "Scenarios");
						}
					const boost::filesystem::path scenario = basefolder / "Scenarios" / model.getName();
					if (!boost::filesystem::is_directory(scenario))
						{
						boost::filesystem::create_directory(scenario);
						}
					std::string lanfile;
					if (models.begin()->getThemes() != model.getThemes())
					{
						lanfile = boost::filesystem::path(scenario / (filename + "._lan")).string();
					}
					std::string arefile;
					if (models.begin()->getArea() != model.getArea())
					{
						arefile = boost::filesystem::path(scenario / (filename + "._are")).string();
					}
					std::string yldfile;
					if (models.begin()->getYields() != model.getYields())
					{
						yldfile = boost::filesystem::path(scenario / (filename + "._yld")).string();
					}
					std::string actfile;
					if (models.begin()->getactions() != model.getactions())
					{
						actfile = boost::filesystem::path(scenario / (filename + "._act")).string();
					}
					std::string trnfile;
					if (models.begin()->getTransitions() != model.getTransitions())
					{
						trnfile = boost::filesystem::path(scenario / (filename + "._trn")).string();
					}
					std::string outfile;
					if (models.begin()->getOutputs() != model.getOutputs())
					{
						outfile = boost::filesystem::path(scenario / (filename + "._out")).string();
					}
					std::string optfile;
					if (models.begin()->getconstraints() != model.getconstraints())
					{
						optfile = boost::filesystem::path(scenario / (filename + "._opt")).string();
					}
					std::string liffile;
					if (models.begin()->getLifespan() != model.getLifespan())
					{
						liffile = boost::filesystem::path(scenario / (filename + "._lif")).string();
					}
					const std::string seqfile = boost::filesystem::path(scenario / (filename + "._seq")).string();
					writeModel(model, lanfile, arefile, yldfile, actfile, trnfile, outfile, optfile, liffile, seqfile);
				}
			}
			catch (...)
			{
				_exhandler->printExceptions("at " + primarym_location, "FMTModelParser::writeToProject", __LINE__, __FILE__);
			}

	}

	

	std::vector<Models::FMTModel>FMTModelParser::readFromFolder(const std::string& primarym_location,
		const std::string& folder,
		std::vector<std::string>scenarios,
		bool validatescenarioname,
		bool readarea, bool readoutputs, bool readoptimize)
	{
		std::vector<Models::FMTModel>sortedmodels;
		std::chrono::time_point<std::chrono::high_resolution_clock> readstart;
		try {
			std::vector<Models::FMTModel>models;
			std::map<std::string, std::vector<int>>commons;
			if (_logger->logWithLevel("Reading " + primarym_location + " ", 0))
			{
				*_logger << "\n";
				_logger->logStamp();
				//_logger->logTime();
			}
			readstart = getClock();
			std::map<Core::FMTsection, std::string>bases = _getPrimary(primarym_location);

			if (!readarea)
			{
				bases.at(Core::FMTsection::Area) = "";
			}
			if (!readoutputs)
			{
				bases.at(Core::FMTsection::Outputs) = "";
				bases.at(Core::FMTsection::Optimize) = "";
			}
			if (!readoptimize)
			{
				bases.at(Core::FMTsection::Optimize) = "";
			}
			bool tookroot = (std::find(scenarios.begin(), scenarios.end(), "ROOT") != scenarios.end());
			if (tookroot || (validatescenarioname && scenarios.empty())) //load the modelroot!
			{
				_logger->logWithLevel("Reading scenario ROOT\n", 0);
				Models::FMTModel scenario = referenceRead(commons,
					models,
					bases.at(Core::FMTsection::Constants),
					bases.at(Core::FMTsection::Landscape),
					bases.at(Core::FMTsection::Lifespan),
					bases.at(Core::FMTsection::Area),
					bases.at(Core::FMTsection::Yield),
					bases.at(Core::FMTsection::Action),
					bases.at(Core::FMTsection::Transition),
					bases.at(Core::FMTsection::Outputs),
					bases.at(Core::FMTsection::Optimize), true);
				models.back().setName("ROOT");
			}
			const boost::filesystem::path primary_path(primarym_location);
			std::string main_name = primary_path.stem().string();
			boost::to_lower(main_name);
			const boost::filesystem::path scenarios_path = boost::filesystem::path(folder);// (primary_path.parent_path() / boost::filesystem::path("Scenarios"));
			if (boost::filesystem::is_directory(scenarios_path))
			{
				boost::filesystem::directory_iterator end_itr;
				std::string model_name;
				for (boost::filesystem::directory_iterator itr(scenarios_path); itr != end_itr; ++itr)
				{
					if (boost::filesystem::is_directory(itr->path()))
					{
						model_name = itr->path().stem().string();
						if (!validatescenarioname || std::find(scenarios.begin(), scenarios.end(), model_name) != scenarios.end())
						{
							std::map<Core::FMTsection, std::string>scenario_files = bases;
							boost::filesystem::directory_iterator end_fileitr;
							for (boost::filesystem::directory_iterator fileitr(itr->path()); fileitr != end_fileitr; ++fileitr)
							{
								if (boost::filesystem::is_regular_file(fileitr->path()))
								{
									const std::string extension = fileitr->path().extension().string();
									Core::FMTsection section = fromExtension(extension);
									std::string file_name = fileitr->path().stem().string();
									boost::to_lower(file_name);
									if (section != Core::FMTsection::Empty && (!validatescenarioname || file_name == main_name))
									{
										scenario_files[section] = fileitr->path().string();

									}
								}
							}

							if (!readarea)
							{
								scenario_files.at(Core::FMTsection::Area) = "";
							}
							if (!readoutputs)
							{
								scenario_files.at(Core::FMTsection::Outputs) = "";
								scenario_files.at(Core::FMTsection::Optimize) = "";
							}
							if (!readoptimize)
							{
								scenario_files.at(Core::FMTsection::Optimize) = "";
							}
							_logger->logWithLevel("Reading scenario " + model_name + "\n", 0);
							Models::FMTModel scenario = referenceRead(commons,
								models,
								scenario_files.at(Core::FMTsection::Constants),
								scenario_files.at(Core::FMTsection::Landscape),
								scenario_files.at(Core::FMTsection::Lifespan),
								scenario_files.at(Core::FMTsection::Area),
								scenario_files.at(Core::FMTsection::Yield),
								scenario_files.at(Core::FMTsection::Action),
								scenario_files.at(Core::FMTsection::Transition),
								scenario_files.at(Core::FMTsection::Outputs),
								scenario_files.at(Core::FMTsection::Optimize), true);
							models.back().setName(model_name);
						}
					}
				}
			}

			if (scenarios.empty())
			{
				for (Models::FMTModel& model : models)
				{
					model.cleanActionsNTransitions();
					sortedmodels.push_back(model);
				}

				//models.begin()->cleanActionsNTransitions();
				//sortedmodels.push_back(*models.begin());
			}
			else {
				for (const std::string& scenario : scenarios)
				{
					std::vector<Models::FMTModel>::iterator modelit = std::find_if(models.begin(), models.end(), Models::FMTModelComparator(scenario));
					if (modelit != models.end())
					{
						modelit->cleanActionsNTransitions();
						sortedmodels.push_back(*modelit);
					}
				}
			}
			if (!scenarios.empty() && sortedmodels.size() != scenarios.size())
			{
				std::vector<std::string> missing_scenarios;
				if (models.empty())
				{
					missing_scenarios = scenarios;
				}
				else {
					for (const std::string& scenario : scenarios)
					{
						std::vector<Models::FMTModel>::iterator modelit = std::find_if(models.begin(), models.end(), Models::FMTModelComparator(scenario));
						if (modelit == models.end())
						{
							missing_scenarios.push_back(scenario);
						}
					}
				}
				_exhandler->raise(Exception::FMTexc::FMTmissing_scenarios,
					boost::algorithm::join(missing_scenarios, " ") + " for " + primarym_location, "FMTModelParser::readFromFolder", __LINE__, __FILE__);
			}
		}
		catch (...)
		{
			_exhandler->printExceptions("at " + primarym_location, "FMTModelParser::readFromFolder", __LINE__, __FILE__);
		}

		if (_logger->logWithLevel("Done reading " + getDurationInSeconds(readstart) + " ", 0))
		{
			//_logger->logTime();
		}
		return sortedmodels;

	}

	std::vector<Models::FMTModel>FMTModelParser::readTemplates(const std::string& primarym_location, const std::string& templatefolder)
	{
		try {
			return readFromFolder(primarym_location, templatefolder, std::vector<std::string>(), false);
		}
		catch (...)
		{
			_exhandler->printExceptions("at " + primarym_location, "FMTModelParser::readtemplate", __LINE__, __FILE__);
		}
		return std::vector<Models::FMTModel>();
	}

	std::vector<Models::FMTModel>FMTModelParser::readproject(const std::string& primarym_location,
		std::vector<std::string>scenarios,
		bool readarea, bool readoutputs, bool readoptimize)
	{
		try {
			const boost::filesystem::path primary_path(primarym_location);
			const boost::filesystem::path scenarios_path = (primary_path.parent_path() / boost::filesystem::path("Scenarios"));
			const std::string scenariofolder = scenarios_path.string();
			return readFromFolder(primarym_location, scenariofolder, scenarios, true,readarea,readoutputs,readoptimize);
		}
		catch (...)
		{
			_exhandler->printExceptions("at " + primarym_location, "FMTModelParser::readproject", __LINE__, __FILE__);
		}
		return std::vector<Models::FMTModel>();
	}

	std::vector<std::vector<Core::FMTSchedule>>FMTModelParser::readschedules(const std::string& primarym_location,
		const std::vector<Models::FMTModel>& models)
	{
		std::vector<std::vector<Core::FMTSchedule>>schedules(models.size());
		try {
			const boost::filesystem::path primary_path(primarym_location);
			const std::map<Core::FMTsection, std::string>bases = _getPrimary(primarym_location);
			FMTScheduleParser scheduleparser;
			scheduleparser.passInExceptionHandler(_exhandler);
			std::vector<Models::FMTModel>::const_iterator model_it = std::find_if(models.begin(), models.end(), Models::FMTModelComparator("ROOT"));
			if (model_it != models.end())
			{
				const size_t location = std::distance<std::vector<Models::FMTModel>::const_iterator>(models.begin(), model_it);
				const boost::filesystem::path root_solution(bases.at(Core::FMTsection::Schedule));
				if (boost::filesystem::is_regular_file(root_solution))
				{
					const std::vector<Core::FMTAction>actions = model_it->getactions();
					const std::vector<Core::FMTTheme>themes = model_it->getThemes();
					schedules[location] = scheduleparser.read(themes, actions, root_solution.string());
				}
				else {
					_exhandler->raise(Exception::FMTexc::FMTempty_schedules,
						primarym_location + " for the ROOT scenario", "FMTModelParser::readschedules", __LINE__, __FILE__);
				}
			}
			const boost::filesystem::path scenarios_path = (primary_path.parent_path() / boost::filesystem::path("Scenarios"));
			if (boost::filesystem::is_directory(scenarios_path))
			{
				std::string name = boost::filesystem::path(bases.at(Core::FMTsection::Schedule)).filename().string();
				boost::replace_all(name, ".seq", "._seq");
				boost::replace_all(name, ".SEQ", "._SEQ");
				const boost::filesystem::path file_name(name);
				boost::filesystem::directory_iterator end_itr;
				std::string model_name;
				for (boost::filesystem::directory_iterator itr(scenarios_path); itr != end_itr; ++itr)
				{
					if (boost::filesystem::is_directory(itr->path()))
					{
						model_name = itr->path().stem().string();
						model_it = std::find_if(models.begin(), models.end(), Models::FMTModelComparator(model_name));
						if (model_it != models.end())
						{
							boost::filesystem::path solutionpath = (itr->path() / file_name);
							if (boost::filesystem::is_regular_file(solutionpath))
							{
								const size_t location = std::distance<std::vector<Models::FMTModel>::const_iterator>(models.begin(), model_it);
								const std::vector<Core::FMTAction>actions = model_it->getactions();
								const std::vector<Core::FMTTheme>themes = model_it->getThemes();
								const std::vector<Core::FMTActualDevelopment>area = model_it->getArea();
								schedules[location] = scheduleparser.read(themes, actions, solutionpath.string());
							}
							else {
								_exhandler->raise(Exception::FMTexc::FMTempty_schedules,
									primarym_location + " for the scenario " + model_name, "FMTModelParser::readschedules", __LINE__, __FILE__);
							}
						}
					}
				}
			}
			if (std::difftime(m_MostRecentFile, scheduleparser.getMostRecentFiletime()) > 0)
			{
				_exhandler->raise(Exception::FMTexc::FMTignore,
					"Schedules files older than the model at " + primarym_location, "FMTModelParser::readschedules", __LINE__, __FILE__);
			}
		}
		catch (...)
		{
			_exhandler->printExceptions("at " + primarym_location, "FMTModelParser::readschedules", __LINE__, __FILE__);
		}

		return schedules;
	}

	void FMTModelParser::setSolverParameters(Models::FMTModel& p_model, const std::string& p_optimize_file) const
		{
		const boost::filesystem::path BASE_FOLDER = boost::filesystem::path(p_optimize_file).parent_path();
		for (const auto& FILE : boost::make_iterator_range(boost::filesystem::directory_iterator(BASE_FOLDER), {}))
			{
			const boost::filesystem::path PATH = FILE.path();
			const std::string FILE_WITHOUT_EXTENSON = PATH.stem().string();
			const std::string FULL_PATH = PATH.string();
			Models::FMTstrmodelparameters PARAM_TYPE = Models::FMTstrmodelparameters::LastStrModelParam;
			if (FILE_WITHOUT_EXTENSON == "ColdStart")
			{
				_logger->logWithLevel("Reading ColdStart parameters " + FILE.path().filename().string() + "\n", 0);
				PARAM_TYPE = Models::FMTstrmodelparameters::SOLVER_COLD_START;
			}else if (FILE_WITHOUT_EXTENSON == "WarmStart")
				{
				_logger->logWithLevel("Reading WarmStart parameters " + FILE.path().filename().string() + "\n", 0);
				PARAM_TYPE = Models::FMTstrmodelparameters::SOLVER_WARM_START;
				}
			if (PARAM_TYPE!= Models::FMTstrmodelparameters::LastStrModelParam)
				{
				std::ifstream io(FULL_PATH);
				if (io.is_open())
					{
					const std::string VALUES((std::istreambuf_iterator<char>(io)),
												std::istreambuf_iterator<char>());
					p_model.setParameter(PARAM_TYPE, VALUES);
					}
				}
			}
		
		}

}