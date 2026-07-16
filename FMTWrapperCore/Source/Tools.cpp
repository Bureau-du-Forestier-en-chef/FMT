#include  "Tools.h"
#include "FMTmodel.h"
#include "FMTlpmodel.h"
#include "FMTerror.h"
#include "FMTmodelparser.h"
#include "FMTareaparser.h"
#include "FMTforest.h"
#include <vector>
#include <string>



int FMTWrapperCore::Tools::getMaxAge(const Models::FMTmodel& p_model)
{
	int result = 0;
	try
	{
		const Core::FMTYields YIELDS = p_model.getYields();
		std::vector<const Core::FMTYieldHandler*> handler;
		for (const auto& DATA : YIELDS)
		{
            if (DATA.second->getType() == Core::FMTyldtype::FMTageyld){

                handler.push_back(DATA.second.get());
            }
		}
		result = YIELDS.getMaxBase(handler);

	}
	catch (...)
	{
		Exception::FMTExceptionHandler* modelExceptionHandler = p_model.getExceptionHandler();
		modelExceptionHandler->raiseFromCatch("", "FMTWrapperCore::Tools::getMaxAge", __LINE__, __FILE__);
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
		Exception::FMTExceptionHandler* modelExceptionHandler = p_model.getExceptionHandler();
		modelExceptionHandler->raiseFromCatch("", "FMTWrapperCore::Tools::getYield", __LINE__, __FILE__);
	}
	return result;
}


Core::FMTMask FMTWrapperCore::Tools::_GetFullMask(const std::vector<Core::FMTTheme>& p_themes)
{
	std::string mask;
	for (const Core::FMTTheme& THEME: p_themes)
		{
		mask += "? ";
		}
	mask.pop_back();
	return Core::FMTMask(mask, p_themes);
}
std::set<std::string> FMTWrapperCore::Tools::_GetThemesDecomposition(
	const Core::FMTMask& p_mask,
	const std::vector<Core::FMTTheme>& p_themes)
{
	std::vector<Core::FMTMask>Allmasks;
	Allmasks.push_back(p_mask);
	for (const Core::FMTTheme& THEME : p_themes)
	{
		std::vector<Core::FMTMask>NewMasks;
		for (const Core::FMTMask& MASK : Allmasks)
			{
			const std::vector<Core::FMTMask> NEW = MASK.decompose(THEME);
			NewMasks.insert(NewMasks.end(), NEW.begin(), NEW.end());
			}
		Allmasks.swap(NewMasks);
	}
return std::set<std::string>(Allmasks.begin(), Allmasks.end());
}

std::set<std::string> FMTWrapperCore::Tools::getAllMasks(
	const Models::FMTmodel& p_model, 
	const int p_periods, 
	const std::vector<int>& p_themesNumbers, 
	const std::string& p_rasterPath) 
{
	std::set<std::string> masks;
	try
	{
		// On cr�e une copie du mod�le car on dois faire un setArea avec les donn�es du raster.
		Models::FMTmodel modelCopy = p_model;
		modelCopy.setParameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD, false);
		if (!p_rasterPath.empty())
		{
			modelCopy.setArea(getRasterArea(modelCopy, p_rasterPath));
		}
		// On va chercher tous les th�mes dans le mod�le
		std::vector<Core::FMTTheme> themes;
		size_t numberOfAttributes = 1;
		const std::vector<Core::FMTTheme> THEMESMODELS = modelCopy.getThemes();
		for (const int themeNumber : p_themesNumbers)
			{
			themes.push_back(THEMESMODELS.at(themeNumber - 1));
			numberOfAttributes *= themes.back().size();
			}
		if (numberOfAttributes > m_GET_ALL_MASKS_THRESHOLD)
		{
			
			modelCopy.setConstraints(std::vector<Core::FMTConstraint>());
			modelCopy.setParameter(Models::FMTintmodelparameters::LENGTH, 1);
			//modelCopy.setParameter(Models::FMTintmodelparameters::LENGTH, 30);
			/**Models::FMTlpmodel optModel(modelCopy, Models::FMTsolverinterface::MOSEK);
			optModel.setparameter(Models::FMTintmodelparameters::LENGTH, p_periods);
			optModel.doPlanning(false);
			masks = optModel.getAllMasks(themes);*/
			//1938
			std::vector<Core::FMTActualDevelopment> area = modelCopy.getArea();
			for (int i = 1; i <= p_periods; ++i)
			{
				Models::FMTlpmodel optModel(modelCopy, Models::FMTsolverinterface::MOSEK);
				for (auto& dev : area)
					{
					dev.setPeriod(i-1);
					}
				optModel.setArea(area);
				optModel.doPlanning(false);
				std::set<std::string> tempMasks = optModel.getAllMasks(themes);
				masks.insert(tempMasks.begin(), tempMasks.end());
				area = optModel.getPotentialArea(i + 1);
			}
		}else {
			const Core::FMTMask PLAIN_MASK = _GetFullMask(THEMESMODELS);
			masks = _GetThemesDecomposition(PLAIN_MASK, themes);
			}
		
	}
	catch (...)
	{
		Exception::FMTExceptionHandler* modelExceptionHandler = p_model.getExceptionHandler();
		modelExceptionHandler->raiseFromCatch("", "FMTWrapperCore::Tools::getAllMasks", __LINE__, __FILE__);
	}
	return masks;
}
void FMTWrapperCore::Tools::writeToProject(const Models::FMTmodel& p_model, const std::string& p_primaryLocation)
{
	try
	{
		Parser::FMTmodelparser ModelParser;
		ModelParser.writeToProject(p_primaryLocation, p_model);
	}
	catch (...)
	{
		Exception::FMTExceptionHandler* modelExceptionHandler = p_model.getExceptionHandler();
		modelExceptionHandler->raiseFromCatch("", "FMTWrapperCore::Tools::writetoproject", __LINE__, __FILE__);
	}
}

std::vector<Core::FMTActualDevelopment> FMTWrapperCore::Tools::getRasterArea(const Models::FMTmodel& p_model, const std::string& p_rasterPath)
{
	std::vector<std::string> themesrast;
	// Assurez-vous que le chemin se termine par un s�parateur pour une construction propre
	const std::string AGE_RASTER_PATH = p_rasterPath + "AGE.tif";
	// Si le stanlock est présent dans le dossier raster on doit le prendre en compte
	const std::string STANLOCK_RASTER_PATH = p_rasterPath + "STANLOCK.tif";
	
	// On vérifie si le fichier Standlock est présent
	bool stanlockExists = false;
	Parser::FMTareaparser areaparser;
	Spatial::FMTforest initialforestmap;
	std::vector<Core::FMTActualDevelopment> area;

	// On vérifie si le fichier standlock est présent
	std::ifstream file(STANLOCK_RASTER_PATH);
	if (file) {
		stanlockExists = true;
	}

	for (size_t i = 1; i <= p_model.getThemes().size(); ++i)
	{
		themesrast.push_back(p_rasterPath + "THEME" + std::to_string(i) + ".tif");
	}
	if (!stanlockExists) {
		initialforestmap = areaparser.readRasters(p_model.getThemes(), themesrast, AGE_RASTER_PATH, 1, 0.0001);

	}
	else {
		initialforestmap = areaparser.readRasters(p_model.getThemes(), themesrast, AGE_RASTER_PATH, 1, 0.0001, STANLOCK_RASTER_PATH);

	}

	area = initialforestmap.getArea();

	return area;
}


