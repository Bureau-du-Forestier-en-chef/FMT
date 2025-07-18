#include  "Tools.h"
#include "FMTmodel.h"
#include "FMTlpmodel.h"
#include "FMTerror.h"
#include "FMTmodelparser.h"


int FMTWrapperCore::Tools::getMaxAge(const Models::FMTmodel& p_model)
{
	double result = 0;
	try
	{
		const Core::FMTyields YIELDS = p_model.getyields();
		std::vector<const Core::FMTyieldhandler*> handler;
		for (const auto& DATA : YIELDS)
		{
            if (DATA.second->gettype() == Core::FMTyldtype::FMTageyld){

                handler.push_back(DATA.second.get());
            }
		}
		result = YIELDS.getmaxbase(handler);

	}
	catch (...)
	{
		Exception::FMTexceptionhandler* modelExceptionHandler = p_model.getExceptionHandler();
		modelExceptionHandler->raisefromcatch("", "FMTWrapperCore::Tools::getMaxAge", __LINE__, __FILE__);
	}
	return result;
}

double FMTWrapperCore::Tools::getYield(const Models::FMTmodel& p_model, const std::string& p_mask, const std::string& p_yield, int p_age)
{
	double result = 0;
	try
	{
        result = p_model.getYieldValue(p_mask, p_yield, p_age, 0);
	}
	catch (...)
	{
		Exception::FMTexceptionhandler* modelExceptionHandler = p_model.getExceptionHandler();
		modelExceptionHandler->raisefromcatch("", "FMTWrapperCore::Tools::getYield", __LINE__, __FILE__);
	}
	return result;
}

std::set<std::string> FMTWrapperCore::Tools::getAllMasks(const Models::FMTmodel& p_model, const int p_periods, const std::vector<int>& p_themesNumbers) {
	std::set<std::string> masks;
	try
	{

		// On va chercher tous les th�mes dans le mod�le
		std::vector<Core::FMTtheme> themes;
		const std::vector<Core::FMTtheme> THEMESMODELS = p_model.getthemes();

		for (const int themeNumber : p_themesNumbers)
		{
			themes.push_back(THEMESMODELS.at(themeNumber - 1));
		}

		// On transforme notre mod�le en lpModel qui lui peut faire un getAllMasks et on ajoute les param�tres qu'on a besoin.
		Models::FMTlpmodel optModel(p_model, Models::FMTsolverinterface::MOSEK);
		optModel.setparameter(Models::FMTintmodelparameters::LENGTH, p_periods);
		optModel.setparameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD, true);
		optModel.doplanning(false);

		masks = optModel.getAllMasks(themes);
		
	}
	catch (...)
	{
		Exception::FMTexceptionhandler* modelExceptionHandler = p_model.getExceptionHandler();
		modelExceptionHandler->raisefromcatch("", "FMTWrapperCore::Tools::getAllMasks", __LINE__, __FILE__);
	}
	return masks;
}

std::vector<Core::FMTconstraint> FMTWrapperCore::Tools::getSelectedConstraints(
	std::vector<Core::FMTconstraint> p_baseConstraints, 
	std::vector<std::string>& p_selectedConstraints)
{
	std::vector<Core::FMTconstraint> retour;
	try
	{
		for (const Core::FMTconstraint& constraint : p_baseConstraints)
		{
			std::string stdContraite = std::string(constraint);
			stdContraite.erase(std::remove(stdContraite.begin(), stdContraite.end(), '\n'), stdContraite.end());
			stdContraite.erase(std::remove(stdContraite.begin(), stdContraite.end(), '\r'), stdContraite.end());

			for (const std::string& selection : p_selectedConstraints)
			{
				std::string stdSelection = selection;
				stdSelection.erase(std::remove(stdSelection.begin(), stdSelection.end(), '\n'), stdSelection.end());
				stdSelection.erase(std::remove(stdSelection.begin(), stdSelection.end(), '\r'), stdSelection.end());
	
				if (stdContraite == stdSelection)
				{
					retour.push_back(constraint);
				}
			}
		}
	}
	catch (...)
	{
		// TODO	
	}

	return retour;
}

std::vector<Core::FMTschedule> FMTWrapperCore::Tools::getSchedule(
	std::string& priFileName,
	Models::FMTsesmodel simulationModel)
{
	std::vector<Core::FMTschedule> retour;
	try
	{
		Parser::FMTmodelparser Modelparser;
		std::vector<Models::FMTmodel> models;
		models.push_back(simulationModel);
		std::vector<Core::FMTschedule> liste = Modelparser.readschedules(priFileName, models).at(0);
		if (liste.size() > 0)
		{
			retour = liste;
		}
	}
	catch (...)
	{
		// TODO
	}

	return retour;
}


std::vector<Core::FMToutput> FMTWrapperCore::Tools::writeOutputs(
	const Models::FMTsemodel& semodel, 
	const std::vector<std::string>& outputs, 
	const int& nombredeperiodes, 
	const bool& incarbon,
	std::function<void(const std::string&)> report)
{
	std::vector<Core::FMToutput> listeOutputs;
	try {
		std::vector<Core::FMToutput> listeOutputs;
		for (const Core::FMToutput& fmtOutput : semodel.getoutputs())
		{
			const std::string name = fmtOutput.getname();
			if (std::find(outputs.begin(), outputs.end(), name) != outputs.end())
			{
				listeOutputs.push_back(fmtOutput);
			}
		}

		if (incarbon)
		{
			const Spatial::FMTspatialschedule& schedule = semodel.getspschedule();
			for (const Core::FMToutput& output : listeOutputs)
			{
				std::map<std::string, std::vector<double>> resultatOutputsCarbon = schedule.getoutput(semodel, output, 1, nombredeperiodes);

				for (double valeurCarbon : resultatOutputsCarbon["Total"])
				{
					report("outputs;" + output.getname() + ";" + std::to_string(valeurCarbon));
				}
			}
		}
	}catch (...)
	{
		// TODO
	}
	return listeOutputs;
}

void FMTWrapperCore::Tools::writePredictors(
	const Models::FMTsemodel& semodel, 
	const std::string& rastpath, 
	const int& periodes, 
	const std::vector<std::string>& predictoryields,
	std::function<void(const std::string&)> report)
{
	try {
		Parser::FMTareaparser areaparser;
		const Spatial::FMTspatialschedule& schedule = semodel.getspschedule();
		std::vector<std::vector<std::vector<double>>> allpredictors;
		std::vector<std::string> allpredictornames;

		for (size_t period = 1; period <= periodes; ++period)
		{
			std::vector<std::vector<Graph::FMTpredictor>> predictors = areaparser.writepredictors(rastpath, schedule, yieldsforpredictors, semodel, period);
			std::vector<std::vector<double>> periodpredictors;
			allpredictornames = predictors.back().back().getpredictornames(yieldsforpredictors);
			size_t indexPredictors = 0;
			
			for (const auto& predictorslist : predictors)
			{
				std::vector<double> graphpred;
				int indexPeriode = period - 1;
				std::string outof = std::to_string(indexPeriode + ";" + indexPredictors + ";");
				for (const double& predval : predictorslist.back().getpredictors())
				{
					graphpred.push_back(predval);
					outof += std::to_string(predval);
					outof += ";";
				}

				++indexPredictors;
				outof.pop_back();
				report("allpredictionsnodes;" + outof);
				periodpredictors.push_back(graphpred);
			}
			allpredictors.push_back(periodpredictors);
		}
		for (const std::string& value : allpredictornames)
		{
			report("allpredictornames;" + value);
		}
	}
	catch (...)
	{
		// TODO 
	}

}


void FMTWrapperCore::Tools::writeSpatialOutputs(
	const Models::FMTsemodel& semodel,
	const std::vector<Core::FMToutput>& outputs,
	const int& sortiemin, const int& sortiemax, 
	std::string& localisation,
	std::function<void(const std::string&)> report)
	)
{
	try {
		Parser::FMTareaparser areaparser;
		report("FMT -> Écriture des outputs spatiaux");
		for (int period = sortiemin; period <= sortiemax; ++period)
		{
			for (const Core::FMToutput& output : outputs)
			{
				const std::string outputname = output.getname() + "_" + std::to_string(period) + ".tif";
				std::filesystem::path outputpath = std::filesystem::path(localisation) / outputname;
                const std::string stdoutputpath = outputpath.string();
				const Spatial::FMTlayer<double> outputlayer = semodel.getspatialoutput(output, period);
				areaparser.writelayer(outputlayer, stdoutputpath);
			}
		}

		report("FMT -> Écriture des outputs spatiaux terminée");
		
	}
	catch (...)
	{
		Cache->getformhandler()->raisefromcatch("", "Wrapper::FMTForm::EcrituredesOutputsSpatiaux", __LINE__, __FILE__);
	}
}