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

std::set<std::string> FMTWrapperCore::Tools::getAllMasks(const Models::FMTmodel& p_model, const int p_periods, const std::vector<int>& p_themesNumbers, const std::string& p_rasterPath) {
	std::set<std::string> masks;
	try
	{
		// On cr�e une copie du mod�le car on dois faire un setArea avec les donn�es du raster.
		Models::FMTmodel modelCopy = p_model;
		modelCopy.setarea(getRasterArea(modelCopy, p_rasterPath));


		// On va chercher tous les th�mes dans le mod�le
		std::vector<Core::FMTtheme> themes;
		const std::vector<Core::FMTtheme> THEMESMODELS = modelCopy.getthemes();

		for (const int themeNumber : p_themesNumbers)
		{
			themes.push_back(THEMESMODELS.at(themeNumber - 1));
		}

		// On transforme notre mod�le en lpModel qui lui peut faire un getAllMasks et on ajoute les param�tres qu'on a besoin.
		Models::FMTlpmodel optModel(modelCopy, Models::FMTsolverinterface::MOSEK);
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
std::vector<Core::FMTactualdevelopment> FMTWrapperCore::Tools::getRasterArea(const Models::FMTmodel& p_model, const std::string& p_rasterPath)
{
	std::vector<std::string> themesrast;
	// Assurez-vous que le chemin se termine par un s�parateur pour une construction propre
	const std::string agerast = p_rasterPath + "AGE.tif";
	// On avais dit sans stanlock mais je l'ai mis ici au cas ou on voulais lire avec dans le TurFu
	//const std::string lockrast = p_rasterPath + "STANLOCK.tif";

	Parser::FMTareaparser areaparser;
	std::vector<Core::FMTactualdevelopment> area;

	for (size_t i = 1; i <= p_model.getthemes().size(); ++i)
	{
		themesrast.push_back(p_rasterPath + "THEME" + std::to_string(i) + ".tif");
	}

	Spatial::FMTforest initialforestmap = areaparser.readrasters(
		p_model.getthemes(), themesrast, agerast, 1, 0.0001);

	area = initialforestmap.getarea();

	// Correction majeure : on retourne par VALEUR pour �viter une r�f�rence invalide.
	return area;
}


