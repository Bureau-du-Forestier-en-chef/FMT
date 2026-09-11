#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>

#include "FMTAreaParser.h"
#include "FMTException.h"
#include "FMTForest.h"
#include "FMTModel.h"
#include "FMTModelParser.h"
#include "FMTTheme.h"
#include "Rasterization.h"

// Test manuel de FMTWrapperCore::Rasterization.
//
// Sans argument, il tourne sur le modèle public TWD_land, avec les mêmes données
// que le test de base maptoFMTforest.
//
// Avec arguments, même forme que maptoFMTforest, pour pouvoir rejoindre
// basetests.csv en une ligne le jour où les tests du Core seront dans ctest :
//   argv[1] = "<fichier .pri>|<scénario>|<résolution>"
//   argv[2] = fichier vectoriel
//   argv[3] = dossier de sortie
//   argv[4] = champ de verrou (optionnel)
//
// AGE.tif et THEME<i>.tif sont supprimés du dossier de sortie avant l'appel, pour
// que leur présence ensuite prouve qu'ils viennent bien d'être écrits.
int main(int argc, char* argv[])
{
	std::string primaryLocation;
	std::string scenario;

	FMTWrapperCore::RasterizationParameters params;
	params.ageField = "AGE";
	params.areaField = "SUPERFICIE";

	if (argc > 3)
	{
		std::vector<std::string> values;
		const std::string ARGUMENT = argv[1];
		boost::split(values, ARGUMENT, boost::is_any_of("|"));
		primaryLocation = values.at(0);
		scenario = values.at(1);
		params.resolution = std::stoi(values.at(2));
		params.vectorFilePath = argv[2];
		params.outputFolder = argv[3];

		if (argc > 4)
		{
			params.lockField = argv[4];
		}
	}
	else
	{
		primaryLocation = "../../../../Examples/Models/TWD_land/TWD_land.pri";
		scenario = "ROOT";
		params.resolution = 1420;
		params.vectorFilePath = "../../../../Examples/Models/TWD_land/Carte/TWD_land.shp";
		params.outputFolder = "../../tests/testWrapperCoreRasterization/";
	}

	Parser::FMTModelParser modelParser;
	std::vector<Exception::FMTexc> errors;
	errors.push_back(Exception::FMTexc::FMTmissingyield);
	errors.push_back(Exception::FMTexc::FMToutput_missing_operator);
	errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
	errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
	errors.push_back(Exception::FMTexc::FMTundefinedoutput_attribute);
	errors.push_back(Exception::FMTexc::FMToveridedyield);
	errors.push_back(Exception::FMTexc::FMTsourcetotarget_transition);
	errors.push_back(Exception::FMTexc::FMTsame_transitiontargets);
	errors.push_back(Exception::FMTexc::FMTinvalid_geometry);
	modelParser.setErrorsToWarnings(errors);

	const std::vector<Models::FMTModel> MODELS =
		modelParser.readproject(primaryLocation, std::vector<std::string>(1, scenario));
	const Models::FMTModel& MODEL = MODELS.at(0);
	const std::vector<Core::FMTTheme> THEMES = MODEL.getThemes();

	const std::filesystem::path OUTPUT(params.outputFolder);
	std::filesystem::create_directories(OUTPUT);

	const std::filesystem::path AGE_RASTER = OUTPUT / "AGE.tif";
	std::vector<std::string> themesRasters;

	for (size_t theme = 1; theme <= THEMES.size(); ++theme)
	{
		themesRasters.push_back((OUTPUT / ("THEME" + std::to_string(theme) + ".tif")).string());
	}

	std::filesystem::remove(AGE_RASTER);

	for (const std::string& RASTER : themesRasters)
	{
		std::filesystem::remove(RASTER);
	}

	try
	{
		FMTWrapperCore::Rasterization::rasterize(params, MODEL);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		try { std::rethrow_if_nested(e); }
		catch (const std::exception& nested) { std::cerr << nested.what() << std::endl; }
		return 1;
	}
	catch (...)
	{
		std::cerr << "Unknown error" << std::endl;
		return 1;
	}

	if (!std::filesystem::exists(AGE_RASTER))
	{
		std::cerr << "Raster manquant : " << AGE_RASTER.string() << std::endl;
		return 1;
	}

	for (const std::string& RASTER : themesRasters)
	{
		if (!std::filesystem::exists(RASTER))
		{
			std::cerr << "Raster manquant : " << RASTER << std::endl;
			return 1;
		}
	}

	// Aller-retour : les rasters écrits doivent se relire en une forêt non vide.
	Parser::FMTAreaParser areaParser;
	Spatial::FMTForest forest = areaParser.readRasters(
		THEMES, themesRasters, AGE_RASTER.string(), 1, 0.0001);

	const size_t DEVELOPMENTS = forest.getArea().size();

	if (DEVELOPMENTS == 0)
	{
		std::cerr << "La forêt relue depuis les rasters est vide" << std::endl;
		return 1;
	}

	std::cout << "test Rasterization passed! (" << DEVELOPMENTS
		<< " développements relus depuis les rasters)" << std::endl;

	return 0;
}
