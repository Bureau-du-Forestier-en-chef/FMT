/*
Copyright (c) 2019 Gouvernement du Qu?bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTmodelparser.h"
#include "FMTparser.h"
#include "FMTconstantparser.h"
#include "FMTlandscapeparser.h"
#include "FMTareaparser.h"
#include "FMTyieldparser.h"
#include "FMTactionparser.h"
#include "FMTtransitionparser.h"
#include "FMTlifespanparser.h"
#include "FMToutputparser.h"
#include "FMToptimizationparser.h"
#include "FMTscheduleparser.h"
#include "FMTmodel.h"
#include "FMTschedule.h"
#include "FMTconstraint.h"
#include "FMToutput.h"
#include <map>
#include "FMTexceptionhandler.h"
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


	FMTmodelparser::FMTmodelparser() :FMTparser()
	{

	}

	FMTmodelparser::FMTmodelparser(const FMTmodelparser& rhs) : FMTparser(rhs)
	{

	}
	FMTmodelparser& FMTmodelparser::operator = (const FMTmodelparser& rhs)
	{
		if (this != &rhs)
		{
			FMTparser::operator=(rhs);
		}
		return *this;
	}

#ifdef FMTWITHGDAL

	OGRLayer* FMTmodelparser::createdriftlayer(GDALDataset* dataset, std::vector<std::string> creationoptions) const
	{
		OGRLayer* newlayer = nullptr;
		try {
			newlayer = createlayer(dataset, "outputsdrift", creationoptions);
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
					"Cannote create new fields outputsdrift", "FMTmodelparser::createdriftlayer", __LINE__, __FILE__, _section);
				//Cannot create field
			}

		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTmodelparser::createdriftlayer", __LINE__, __FILE__);
		}
		return newlayer;
	}

	void FMTmodelparser::writedrift(OGRLayer* layer, const std::map<std::string, std::map<double, std::vector<double>>>& lowervalues,
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
								"Cannote generate new feature ", "FMTmodelparser::writefeatures", __LINE__, __FILE__, _section);
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
								"Cannote create new feature id " + std::to_string(layer->GetFeatureCount()), "FMTmodelparser::writefeatures", __LINE__, __FILE__, _section);
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
			_exhandler->raisefromcatch("", "FMTmodelparser::writedrift", __LINE__, __FILE__);
		}
	}

	std::map<std::string, std::vector<std::vector<double>>>FMTmodelparser::getiterationsvalues(OGRLayer* layer) const
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
			_exhandler->raisefromcatch("", "FMTmodelparser::getiterationsvalues", __LINE__, __FILE__);
		}
		return results;
	}


	OGRLayer* FMTmodelparser::createResultsLayer(const std::string& p_modelName,
		GDALDataset* p_dataset,
		std::vector<std::string> p_creationOptions)const
	{
		OGRLayer* newlayer = nullptr;
		try {
			newlayer = createlayer(p_dataset, p_modelName, p_creationOptions);
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
						"Cannote create new field " + std::string(ValidDefinition->GetNameRef()), "FMTmodelparser::writeresults", __LINE__, __FILE__, _section);
					}

				}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTmodelparser::createresultslayer", __LINE__, __FILE__, _section);
		}
		return newlayer;
	}

	void FMTmodelparser::fillupinfeasibles(OGRLayer* layer,
		const std::vector<Core::FMToutput>& theoutputs,
		const int& iteration, const int& firstperiod, const int& lastperiod) const
	{
		try {
			boost::lock_guard<boost::recursive_mutex> guard(mtx);
			std::map<std::string, std::vector<std::vector<double>>>allvalues;
			//Cannot fill up infeasible for non-total ... i guess
			allvalues["Total"] = std::vector<std::vector<double>>(theoutputs.size(), std::vector<double>((lastperiod - firstperiod) + 1, std::numeric_limits<double>::quiet_NaN()));
			writefeatures(layer, firstperiod, iteration, theoutputs, allvalues, true);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTmodelparser::fillupinfeasibles", __LINE__, __FILE__, _section);
		}

	}

	void FMTmodelparser::writefeatures(OGRLayer* layer, const int& firstperiod, const int& iteration,
		const std::vector<Core::FMToutput>& theoutputs,
		const std::map<std::string, std::vector<std::vector<double>>>& values, bool writeNaN)const
	{
		try {
			boost::lock_guard<boost::recursive_mutex> guard(mtx);
			for (const auto& toutputvalues : values)
			{
				size_t outputid = 0;
				for (const std::vector<double>& outputvalues : toutputvalues.second)
				{
					int period = firstperiod;
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
									"Cannote generate new feature ", "FMTmodelparser::writefeatures", __LINE__, __FILE__, _section);
								//Failed to generate feature
							}
							newfeature->SetField("Iteration", iteration);
							newfeature->SetField("Period", period);
							newfeature->SetField("Output", theoutputs.at(outputid).getname().c_str());
							newfeature->SetField("Type", outputtype.c_str());
							newfeature->SetField("Value", value);
							if (layer->CreateFeature(newfeature) != OGRERR_NONE)
							{
								_exhandler->raise(Exception::FMTexc::FMTgdal_constructor_error,
									"Cannote create new feature id " + std::to_string(layer->GetFeatureCount()), "FMTmodelparser::writefeatures", __LINE__, __FILE__, _section);
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
			_exhandler->raisefromcatch("", "FMTmodelparser::writefeatures", __LINE__, __FILE__, _section);
		}

	}

	void FMTmodelparser::writeresults(const Models::FMTmodel& model,
		const std::vector<Core::FMToutput>& theoutputs,
		const int& firstperiod, const int& lastperiod,
		const std::string& location,
		Core::FMToutputlevel level,
		std::string gdaldrivername) const
	{
		try {
			GDALDataset* newdataset = createOGRdataset(location, gdaldrivername);
			OGRLayer* newlayer = createResultsLayer(model.getname(), newdataset);
			writefeatures(newlayer, firstperiod, 0, theoutputs, model.getoutputsfromperiods(theoutputs, firstperiod, lastperiod, level));
			GDALClose(newdataset);
		}
		catch (...)
		{
			_exhandler->printexceptions(" at " + location, "FMTmodelparser::writeresults", __LINE__, __FILE__, _section);
		}
	}
#endif 


	void FMTmodelparser::writeprimary(
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
			if (tryopening(pristream, location))
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
			_exhandler->printexceptions(" at " + location, "FMTmodelparser::writeprimary", __LINE__, __FILE__, _section);
		}
	}

	void  FMTmodelparser::writemodel(const Models::FMTmodel& model,
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
			FMTlandscapeparser landparser;
			landparser.write(model.getthemes(),lanfile);
			const std::vector<Core::FMTactualdevelopment>devs = model.getarea();
			if (!devs.empty())
			{
				FMTareaparser areaparser;
				double sumarea = 0;
				for (const Core::FMTactualdevelopment& dev : devs)
				{
					sumarea += dev.getarea();
				}
				const std::string header = "Total area: " + std::to_string(sumarea) + "\n";
				areaparser.setheader(header);
				areaparser.write(devs, arefile);
			}

		}
		if (!yldfile.empty())
		{
			FMTyieldparser yldparser;
			yldparser.write(model.getyields(), yldfile);
		}

		if (!actfile.empty())
		{
			FMTactionparser actparser;
			actparser.write(model.getactions(), actfile);
		}
		
		if (!trnfile.empty())
		{
			FMTtransitionparser trnparser;
			trnparser.write(model.gettransitions(), trnfile);
		}
		if (!liffile.empty())
		{
			FMTlifespanparser lifparser;
			lifparser.write(model.getlifespan(), liffile);
		}
		if (!outfile.empty())
		{
			const std::vector<Core::FMToutput>outputs = model.getoutputs();
			if (!outputs.empty())
			{
				FMToutputparser outparser;
				outparser.write(outputs, outfile);
			}
		}
		if (!optfile.empty())
		{
			const std::vector<Core::FMTconstraint>constraints = model.getconstraints();
			if (!constraints.empty())
			{
				FMToptimizationparser optparser;
				optparser.write(constraints, optfile);
			}
		}
		std::vector<Core::FMTschedule>schedules;
		for (int period = 1; period <= model.getparameter(Models::FMTintmodelparameters::LENGTH); ++period)
		{
			const Core::FMTschedule periodschedule = model.getsolution(period, true);
			if (!periodschedule.empty())
			{
				schedules.push_back(periodschedule);
			}
		}
		if (!seqfile.empty())
		{
			FMTscheduleparser scheduleparser;
			std::string status = "Non Optimal";
			std::string ObjectiveValue = "NA";
			if (model.isoptimal())
				{
				status = "Optimal";
				ObjectiveValue =  std::to_string(model.getobjectivevalue());
				}
			const std::string modelobjective = "Objective value: " + ObjectiveValue + "\n";
			const std::string modelstatus = "Status: " + status + "\n";
			scheduleparser.setheader(modelstatus+ modelobjective);
			scheduleparser.write(schedules, seqfile, false);
		}
	}catch (...)
	{
		_exhandler->printexceptions("", "FMTmodelparser::writemodel", __LINE__, __FILE__, _section);
	}
}



void FMTmodelparser::write(const Models::FMTmodel& model,const std::string& folder) const
    {
	try {
		//Ajout de la section pri
		//retirer les aggr�gats de BFECgcbm et �crire les contraintes sans les 
		const std::string modelname = model.getname();
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
			writeprimary(folder + modelname + ".pri",
				lanname, arename, yldname, actname, trnname, outname, optname, lifname, seqname);
			writemodel(model, lanfile, arefile, yldfile, actfile, trnfile, outfile, optfile, liffile, seqfile);
		
	}catch (...)
		{
		_exhandler->printexceptions(" at " + folder, "FMTmodelparser::write", __LINE__, __FILE__, _section);
		}
	}

	Models::FMTmodel FMTmodelparser::read(const std::string& con, const std::string& lan,
		const std::string& lif, const std::string& are, const std::string& yld,
		const std::string& act, const std::string& trn, const std::string& out, std::string opt)
	{
		try {
			std::map<std::string, std::vector<int>> commons;
			std::vector<Models::FMTmodel>models;
			return referenceread(commons, models,
				con, lan, lif, are, yld, act, trn, out, opt);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTmodelparser::read", __LINE__, __FILE__, _section);
		}
		return Models::FMTmodel();
	}

	std::vector<Core::FMTconstraint>FMTmodelparser::getconstraintsfromstring(std::string constraintstr, const Models::FMTmodel& model, Core::FMTconstants constants)
	{
		std::vector<Core::FMTconstraint>constraints;
		try {
			FMToptimizationparser optparser;
			boost::to_upper(constraintstr);
			for (Core::FMTconstraint constraint : optparser.getConstraints(constraintstr, constants,model.yields,
																			model.outputs, model.themes, model.actions))
			{
				//constraint.passinobject(model);
				constraints.push_back(constraint);
			}
		}
		catch (...)
		{
			_exhandler->printexceptions("While getting constraint from " + constraintstr, "FMTmodelparser::getconstraintsfromstring", __LINE__, __FILE__, _section);
		}
		return constraints;
	}


	Models::FMTmodel FMTmodelparser::referenceread(std::map<std::string, std::vector<int>>& common_sections,
		std::vector<Models::FMTmodel>& models,
		const std::string& con, const std::string& lan,
		const std::string& lif, const std::string& are, const std::string& yld,
		const std::string& act, const std::string& tr, const std::string& out,
		std::string opt, bool allow_mapping)
	{
		Models::FMTmodel returnedmodel;
		try {
			Core::FMTconstants constants;
			std::vector<Core::FMTtheme>themes;
			std::vector<Core::FMTactualdevelopment>areas;
			Core::FMTyields yields;
			Core::FMTlifespans lifespan;
			std::vector<Core::FMTaction>actions;
			std::vector<Core::FMTtransition>transitions;
			std::vector<Core::FMToutput>outputs;
			std::vector<Core::FMTconstraint>constraints;
			if (allow_mapping)
			{
				std::map<std::string, std::vector<int>>::const_iterator constants_it = common_sections.find(con);
				std::map<std::string, std::vector<int>>::const_iterator themes_it = common_sections.find(lan);
				std::map<std::string, std::vector<int>>::const_iterator lifespan_it = common_sections.find(lif);
				std::map<std::string, std::vector<int>>::const_iterator area_it = common_sections.find(are);
				std::map<std::string, std::vector<int>>::const_iterator yield_it = common_sections.find(yld);
				std::map<std::string, std::vector<int>>::const_iterator actions_it = common_sections.find(act);
				std::map<std::string, std::vector<int>>::const_iterator transitions_it = common_sections.find(tr);
				std::map<std::string, std::vector<int>>::const_iterator outputs_it = common_sections.find(out);
				std::map<std::string, std::vector<int>>::const_iterator optimize_it = common_sections.find(opt);
				std::vector<int>::iterator common_it;
				if (constants_it != common_sections.end() && themes_it != common_sections.end())
				{
					std::vector<int>common_sets(themes_it->second.size() + constants_it->second.size());
					common_it = std::set_intersection(themes_it->second.begin(), themes_it->second.end(),
						constants_it->second.begin(), constants_it->second.end(), common_sets.begin());
					common_sets.resize(common_it - common_sets.begin());
					if (!common_sets.empty())
					{
						std::sort(common_sets.begin(), common_sets.end());
						themes = models.at(*common_sets.begin()).getthemes();
						if (area_it != common_sections.end())
						{
							std::vector<int>common_area(common_sets.size() + area_it->second.size());
							common_it = std::set_intersection(common_sets.begin(), common_sets.end(),
								area_it->second.begin(), area_it->second.end(), common_area.begin());
							common_area.resize(common_it - common_area.begin());
							if (!common_area.empty())
							{
								std::sort(common_area.begin(), common_area.end());
								areas = models.at(*common_area.begin()).getarea();
							}
						}
						if (yield_it != common_sections.end())
						{
							std::vector<int>common_yield(common_sets.size() + yield_it->second.size());
							common_it = std::set_intersection(common_sets.begin(), common_sets.end(),
								yield_it->second.begin(), yield_it->second.end(), common_yield.begin());
							common_yield.resize(common_it - common_yield.begin());
							if (!common_yield.empty())
							{
								std::sort(common_yield.begin(), common_yield.end());
								yields = models.at(*common_yield.begin()).getyields();
							}
						}
						if (lifespan_it != common_sections.end())
						{
							std::vector<int>common_lif(common_sets.size() + lifespan_it->second.size());
							common_it = std::set_intersection(common_sets.begin(), common_sets.end(),
								lifespan_it->second.begin(), lifespan_it->second.end(), common_lif.begin());
							common_lif.resize(common_it - common_lif.begin());
							if (!common_lif.empty())
							{
								std::sort(common_lif.begin(), common_lif.end());
								lifespan = models.at(*common_lif.begin()).getlifespan();
								if (actions_it != common_sections.end())
								{
									std::vector<int>common_actions(common_lif.size() + actions_it->second.size());
									common_it = std::set_intersection(common_lif.begin(), common_lif.end(),
										actions_it->second.begin(), actions_it->second.end(), common_actions.begin());
									common_actions.resize(common_it - common_actions.begin());
									if (!common_actions.empty())
									{
										std::sort(common_actions.begin(), common_actions.end());
										actions = models.at(*common_actions.begin()).getactions();
										if (transitions_it != common_sections.end())
										{
											std::vector<int>common_transitions(common_actions.size() + transitions_it->second.size());
											common_it = std::set_intersection(common_actions.begin(), common_actions.end(),
												transitions_it->second.begin(), transitions_it->second.end(), common_transitions.begin());
											common_transitions.resize(common_it - common_transitions.begin());
											if (!common_transitions.empty())
											{
												std::sort(common_transitions.begin(), common_transitions.end());
												transitions = models.at(*common_transitions.begin()).gettransitions();
											}
										}
									}
								}
							}
						}

						if (outputs_it != common_sections.end())//should be with action??
						{
							std::vector<int>common_output(common_sets.size() + outputs_it->second.size());
							common_it = std::set_intersection(common_sets.begin(), common_sets.end(),
								outputs_it->second.begin(), outputs_it->second.end(), common_output.begin());
							common_output.resize(common_it - common_output.begin());
							if (!common_output.empty())
							{
								std::sort(common_output.begin(), common_output.end());
								outputs = models.at(*common_output.begin()).getoutputs();
								if (optimize_it != common_sections.end())
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

				const int model_location = static_cast<int>(models.size());
				if (constants_it == common_sections.end())
				{
					common_sections[con] = std::vector<int>();
				}
				common_sections[con].push_back(model_location);
				if (themes_it == common_sections.end())
				{
					common_sections[lan] = std::vector<int>();
				}
				common_sections[lan].push_back(model_location);
				if (area_it == common_sections.end())
				{
					common_sections[are] = std::vector<int>();
				}
				common_sections[are].push_back(model_location);
				if (lifespan_it == common_sections.end())
				{
					common_sections[lif] = std::vector<int>();
				}
				common_sections[lif].push_back(model_location);
				if (yield_it == common_sections.end())
				{
					common_sections[yld] = std::vector<int>();
				}
				common_sections[yld].push_back(model_location);
				if (actions_it == common_sections.end())
				{
					common_sections[act] = std::vector<int>();
				}
				common_sections[act].push_back(model_location);
				if (transitions_it == common_sections.end())
				{
					common_sections[tr] = std::vector<int>();
				}
				common_sections[tr].push_back(model_location);
				if (outputs_it == common_sections.end())
				{
					common_sections[out] = std::vector<int>();
				}
				common_sections[out].push_back(model_location);
				if (optimize_it == common_sections.end())
				{
					common_sections[opt] = std::vector<int>();
				}
				common_sections[opt].push_back(model_location);
			}
			const boost::filesystem::path landfile(lan);
			const std::string modelname = landfile.stem().string();

			FMTconstantparser cparser;
			//cparser.passinobject(*this);
			constants = cparser.read(con);
			mostrecentfile = std::max(cparser.getmostrecentfiletime(), mostrecentfile);
			if (themes.empty())
			{
				FMTlandscapeparser landparser;
				//landparser.passinobject(*this);
				themes = landparser.read(constants, lan);
				mostrecentfile = std::max(landparser.getmostrecentfiletime(), mostrecentfile);
			}
			if (areas.empty())
			{
				FMTareaparser areaparser;
				//areaparser.passinobject(*this);
				areas = areaparser.read(themes, constants, are);
				mostrecentfile = std::max(areaparser.getmostrecentfiletime(), mostrecentfile);
			}
			if (lifespan.empty())
			{
				FMTlifespanparser lifespanparser;
				//lifespanparser.passinobject(*this);
				lifespan = lifespanparser.read(themes, constants, lif);
				mostrecentfile = std::max(lifespanparser.getmostrecentfiletime(), mostrecentfile);
			}
			if (yields.empty())
			{
				FMTyieldparser yldparser;
				//yldparser.passinobject(*this);
				yields = yldparser.read(themes, constants, yld);
				mostrecentfile = std::max(yldparser.getmostrecentfiletime(), mostrecentfile);
			}
			if (actions.empty())
			{
				FMTactionparser actparser;
				//actparser.passinobject(*this);
				actions = actparser.read(themes, yields, constants, act);
				if (find_if(actions.begin(), actions.end(), Core::FMTactioncomparator("_DEATH")) == actions.end())
				{
					_exhandler->raise(Exception::FMTexc::FMTundefineddeathaction,
						"_DEATH", "FMTmodelparser::referenceread", __LINE__, __FILE__, Core::FMTsection::Action);
					actions.push_back(Models::FMTmodel::defaultdeathaction(lifespan, themes));
				}
				mostrecentfile = std::max(actparser.getmostrecentfiletime(), mostrecentfile);

			}
			if (transitions.empty())
			{
				FMTtransitionparser trnparser;
				//trnparser.passinobject(*this);
				transitions = trnparser.read(themes, actions, yields, constants, tr);
				if (find_if(transitions.begin(), transitions.end(), Core::FMTtransitioncomparator("_DEATH")) == transitions.end())
				{
					_exhandler->raise(Exception::FMTexc::FMTundefineddeathtransition,
						"_DEATH", "FMTmodelparser::referenceread", __LINE__, __FILE__, Core::FMTsection::Transition);
					transitions.push_back(Models::FMTmodel::defaultdeathtransition(lifespan, themes));
				}
				mostrecentfile = std::max(trnparser.getmostrecentfiletime(), mostrecentfile);
			}
			if (outputs.empty())
			{
				FMToutputparser outparser;
				//outparser.passinobject(*this);
				outputs = outparser.read(themes, actions, yields, constants, out);
				mostrecentfile = std::max(outparser.getmostrecentfiletime(), mostrecentfile);
			}
			if (!opt.empty() && constraints.empty())
			{

				FMToptimizationparser optzparser;
				std::vector<Core::FMTaction>excluded(actions); //should we realy use? excluded is actualy the same actions but with more period specification...
				//optzparser.passinobject(*this);
				constraints = optzparser.read(themes, actions, constants, outputs,yields, excluded, opt);
				mostrecentfile = std::max(optzparser.getmostrecentfiletime(), mostrecentfile);
				bool shouldcrapreference = (actions.size() != excluded.size());
				if (!shouldcrapreference)
				{
					size_t location = 0;
					for (const Core::FMTaction& action : actions)
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
					common_sections[act].pop_back();
					if (common_sections.at(act).empty())
					{
						common_sections.erase(act);
					}
					common_sections[tr].pop_back();
					if (common_sections.at(tr).empty())
					{
						common_sections.erase(tr);
					}
				}
				actions = excluded; //here we go
			}
			returnedmodel = Models::FMTmodel(areas, themes, actions,
				transitions, yields, lifespan, modelname, outputs, constraints);
			if (!opt.empty())
				{
				setSolverParameters(returnedmodel, opt);
				}
			//returnedmodel.passinobject(*this);
			returnedmodel.cleanactionsntransitions();
			if (allow_mapping)
			{
				models.push_back(returnedmodel);
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTmodelparser::referenceread", __LINE__, __FILE__);
		}
		return returnedmodel;
	}

		void FMTmodelparser::writetoproject(const std::string& primary_location,
			const Models::FMTmodel& model)
		{
			try {
				boost::filesystem::path primpath(primary_location);
				const std::string filename = primpath.stem().string();
				const boost::filesystem::path basefolder = primpath.parent_path();
				if (!boost::filesystem::is_directory(basefolder))
					{
					_exhandler->raise(Exception::FMTexc::FMTinvalid_path,
						basefolder.string()+" is not a valid directory", "FMTmodelparser::writetoproject", __LINE__, __FILE__);
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
					if (_logger->logwithlevel("Writing " + primary_location + " ", 0))
					{
						*_logger << "\n";
						_logger->logstamp();
						_logger->logtime();
					}
					writeprimary(primary_location,
						clanname, carename, cyldname, cactname, ctrnname, coutname, coptname, clifname, cseqname);
					writemodel(model, clanfile, carefile, cyldfile, cactfile, ctrnfile, coutfile, coptfile, cliffile, cseqfile);
				}else {//read the existing ROOT model...
					const std::vector<std::string>rootmodel(1,"ROOT");
					const std::vector<Models::FMTmodel> models = readproject(primary_location, rootmodel);
					if (models.empty())
					{
						_exhandler->raise(Exception::FMTexc::FMTrangeerror,
							"No Root model for the primary "+ primary_location, "FMTmodelparser::writetoproject", __LINE__, __FILE__);
					}
					if (!boost::filesystem::is_directory(basefolder / "Scenarios"))
						{
						boost::filesystem::create_directory(basefolder / "Scenarios");
						}
						const boost::filesystem::path scenario = basefolder / "Scenarios" / model.getname();
						if (!boost::filesystem::is_directory(scenario))
							{
							boost::filesystem::create_directory(scenario);
							}
						std::string lanfile;
						if (models.begin()->getthemes() != model.getthemes())
						{
							lanfile = boost::filesystem::path(scenario / (filename + "._lan")).string();
						}
						std::string arefile;
						if (models.begin()->getarea() != model.getarea())
						{
							arefile = boost::filesystem::path(scenario / (filename + "._are")).string();
						}
						std::string yldfile;
						if (models.begin()->getyields() != model.getyields())
						{
							yldfile = boost::filesystem::path(scenario / (filename + "._yld")).string();
						}
						std::string actfile;
						if (models.begin()->getactions() != model.getactions())
						{
							actfile = boost::filesystem::path(scenario / (filename + "._act")).string();
						}
						std::string trnfile;
						if (models.begin()->gettransitions() != model.gettransitions())
						{
							trnfile = boost::filesystem::path(scenario / (filename + "._trn")).string();
						}
						std::string outfile;
						if (models.begin()->getoutputs() != model.getoutputs())
						{
							outfile = boost::filesystem::path(scenario / (filename + "._out")).string();
						}
						std::string optfile;
						if (models.begin()->getconstraints() != model.getconstraints())
						{
							optfile = boost::filesystem::path(scenario / (filename + "._opt")).string();
						}
						std::string liffile;
						if (models.begin()->getlifespan() != model.getlifespan())
						{
							liffile = boost::filesystem::path(scenario / (filename + "._lif")).string();
						}
						const std::string seqfile = boost::filesystem::path(scenario / (filename + "._seq")).string();
						writemodel(model, lanfile, arefile, yldfile, actfile, trnfile, outfile, optfile, liffile, seqfile);
					}
			}
			catch (...)
			{
				_exhandler->printexceptions("at " + primary_location, "FMTmodelparser::writetoproject", __LINE__, __FILE__);
			}

	}

	

	std::vector<Models::FMTmodel>FMTmodelparser::readfromfolder(const std::string& primary_location,
		const std::string& folder,
		std::vector<std::string>scenarios,
		bool validatescenarioname,
		bool readarea, bool readoutputs, bool readoptimize)
	{
		std::vector<Models::FMTmodel>sortedmodels;
		std::chrono::time_point<std::chrono::high_resolution_clock> readstart;
		try {
			std::vector<Models::FMTmodel>models;
			std::map<std::string, std::vector<int>>commons;
			if (_logger->logwithlevel("Reading " + primary_location + " ", 0))
			{
				*_logger << "\n";
				_logger->logstamp();
				_logger->logtime();
			}
			readstart = getclock();
			std::map<Core::FMTsection, std::string>bases = getprimary(primary_location);

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
				_logger->logwithlevel("Reading scenario ROOT\n", 0);
				Models::FMTmodel scenario = referenceread(commons,
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
				models.back().setname("ROOT");
			}
			const boost::filesystem::path primary_path(primary_location);
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
									Core::FMTsection section = from_extension(extension);
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
							_logger->logwithlevel("Reading scenario " + model_name + "\n", 0);
							Models::FMTmodel scenario = referenceread(commons,
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
							models.back().setname(model_name);
						}
					}
				}
			}

			if (scenarios.empty())
			{
				for (Models::FMTmodel& model : models)
				{
					model.cleanactionsntransitions();
					sortedmodels.push_back(model);
				}

				//models.begin()->cleanactionsntransitions();
				//sortedmodels.push_back(*models.begin());
			}
			else {
				for (const std::string& scenario : scenarios)
				{
					std::vector<Models::FMTmodel>::iterator modelit = std::find_if(models.begin(), models.end(), Models::FMTmodelcomparator(scenario));
					if (modelit != models.end())
					{
						modelit->cleanactionsntransitions();
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
						std::vector<Models::FMTmodel>::iterator modelit = std::find_if(models.begin(), models.end(), Models::FMTmodelcomparator(scenario));
						if (modelit == models.end())
						{
							missing_scenarios.push_back(scenario);
						}
					}
				}
				_exhandler->raise(Exception::FMTexc::FMTmissing_scenarios,
					boost::algorithm::join(missing_scenarios, " ") + " for " + primary_location, "FMTmodelparser::readfromfolder", __LINE__, __FILE__);
			}
		}
		catch (...)
		{
			_exhandler->printexceptions("at " + primary_location, "FMTmodelparser::readfromfolder", __LINE__, __FILE__);
		}

		if (_logger->logwithlevel("Done reading " + getdurationinseconds(readstart) + " ", 0))
		{
			_logger->logtime();
		}
		return sortedmodels;

	}

	std::vector<Models::FMTmodel>FMTmodelparser::readtemplates(const std::string& primary_location, const std::string& templatefolder)
	{
		try {
			return readfromfolder(primary_location, templatefolder, std::vector<std::string>(), false);
		}
		catch (...)
		{
			_exhandler->printexceptions("at " + primary_location, "FMTmodelparser::readtemplate", __LINE__, __FILE__);
		}
		return std::vector<Models::FMTmodel>();
	}

	std::vector<Models::FMTmodel>FMTmodelparser::readproject(const std::string& primary_location,
		std::vector<std::string>scenarios,
		bool readarea, bool readoutputs, bool readoptimize)
	{
		try {
			const boost::filesystem::path primary_path(primary_location);
			const boost::filesystem::path scenarios_path = (primary_path.parent_path() / boost::filesystem::path("Scenarios"));
			const std::string scenariofolder = scenarios_path.string();
			return readfromfolder(primary_location, scenariofolder, scenarios, true,readarea,readoutputs,readoptimize);
		}
		catch (...)
		{
			_exhandler->printexceptions("at " + primary_location, "FMTmodelparser::readproject", __LINE__, __FILE__);
		}
		return std::vector<Models::FMTmodel>();
	}

	std::vector<std::vector<Core::FMTschedule>>FMTmodelparser::readschedules(const std::string& primary_location,
		const std::vector<Models::FMTmodel>& models)
	{
		std::vector<std::vector<Core::FMTschedule>>schedules(models.size());
		try {
			const boost::filesystem::path primary_path(primary_location);
			const std::map<Core::FMTsection, std::string>bases = getprimary(primary_location);
			FMTscheduleparser scheduleparser;
			scheduleparser.passinexceptionhandler(_exhandler);
			std::vector<Models::FMTmodel>::const_iterator model_it = std::find_if(models.begin(), models.end(), Models::FMTmodelcomparator("ROOT"));
			if (model_it != models.end())
			{
				const size_t location = std::distance<std::vector<Models::FMTmodel>::const_iterator>(models.begin(), model_it);
				const boost::filesystem::path root_solution(bases.at(Core::FMTsection::Schedule));
				if (boost::filesystem::is_regular_file(root_solution))
				{
					const std::vector<Core::FMTaction>actions = model_it->getactions();
					const std::vector<Core::FMTtheme>themes = model_it->getthemes();
					schedules[location] = scheduleparser.read(themes, actions, root_solution.string());
				}
				else {
					_exhandler->raise(Exception::FMTexc::FMTempty_schedules,
						primary_location + " for the ROOT scenario", "FMTmodelparser::readschedules", __LINE__, __FILE__);
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
						model_it = std::find_if(models.begin(), models.end(), Models::FMTmodelcomparator(model_name));
						if (model_it != models.end())
						{
							boost::filesystem::path solutionpath = (itr->path() / file_name);
							if (boost::filesystem::is_regular_file(solutionpath))
							{
								const size_t location = std::distance<std::vector<Models::FMTmodel>::const_iterator>(models.begin(), model_it);
								const std::vector<Core::FMTaction>actions = model_it->getactions();
								const std::vector<Core::FMTtheme>themes = model_it->getthemes();
								const std::vector<Core::FMTactualdevelopment>area = model_it->getarea();
								schedules[location] = scheduleparser.read(themes, actions, solutionpath.string());
							}
							else {
								_exhandler->raise(Exception::FMTexc::FMTempty_schedules,
									primary_location + " for the scenario " + model_name, "FMTmodelparser::readschedules", __LINE__, __FILE__);
							}
						}
					}
				}
			}
			if (std::difftime(mostrecentfile, scheduleparser.getmostrecentfiletime()) > 0)
			{
				_exhandler->raise(Exception::FMTexc::FMTignore,
					"Schedules files older than the model at " + primary_location, "FMTmodelparser::readschedules", __LINE__, __FILE__);
			}
		}
		catch (...)
		{
			_exhandler->printexceptions("at " + primary_location, "FMTmodelparser::readschedules", __LINE__, __FILE__);
		}

		return schedules;
	}

	void FMTmodelparser::setSolverParameters(Models::FMTmodel& p_model, const std::string& p_optimize_file) const
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
				_logger->logwithlevel("Reading ColdStart parameters " + FILE.path().filename().string() + "\n", 0);
				PARAM_TYPE = Models::FMTstrmodelparameters::SOLVER_COLD_START;
			}else if (FILE_WITHOUT_EXTENSON == "WarmStart")
				{
				_logger->logwithlevel("Reading WarmStart parameters " + FILE.path().filename().string() + "\n", 0);
				PARAM_TYPE = Models::FMTstrmodelparameters::SOLVER_WARM_START;
				}
			if (PARAM_TYPE!= Models::FMTstrmodelparameters::LastStrModelParam)
				{
				std::ifstream io(FULL_PATH);
				if (io.is_open())
					{
					const std::string VALUES((std::istreambuf_iterator<char>(io)),
												std::istreambuf_iterator<char>());
					p_model.setparameter(PARAM_TYPE, VALUES);
					}
				}
			}
		
		}

}