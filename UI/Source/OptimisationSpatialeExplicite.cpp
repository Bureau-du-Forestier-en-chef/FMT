#include "stdafx.h"
#include <sstream>
#include "FMTforest.h"
#include "FMTmodelparser.h"
#include "FMTsamodel.h"
#include "FMTareaparser.h"
#include "FMTscheduleparser.h"
#include <msclr\marshal_cppstd.h>
#include "FMTFormLogger.h"
#include "FMTForm.h"
#include "FMTmodel.h"
#include "FMTFormCache.h"
#include "FMTdefaultlogger.h"

bool Wrapper::FMTForm::OptimisationSpatialeExplicite(System::String^ fichierPri, System::String^ cheminRasters, int scenario,
		System::Collections::Generic::List<System::String^>^ contraintes, int periodes,
		int p_MaxMoves, int p_MaxAcceptedMoves, int p_MaxCycleMoves,
		System::Collections::Generic::List<System::String^>^ outputs, bool indicateurStanlock, int outputLevel,
		int etanduSortiesMin, int etanduSortiesMax, System::String^ cheminSorties, bool indGenererEvents, bool indSortiesSpatiales,
		System::String^ providerGdal)
{
	try
	{
		Models::FMTsamodel OptimizationModel(Cache->getmodel(scenario));
		FMTFormLogger* Logger = Cache->getformlogger();
		*Logger << Logging::FMTdefaultlogger().getlogstamp() << "\n";
		*Logger << "FMT -> Traitement pour le sc�nario : " + OptimizationModel.getname() << "\n";
		if (contraintes->Count > 0)
		{
			*Logger << "FMT -> Int�gration des contraintes s�lectionn�es" << "\n";
			OptimizationModel.setconstraints(ObtenirArrayContraintesSelectionnees(ObtenirArrayContraintes(scenario), contraintes));
		}
		*Logger << "FMT -> Modification et int�gration des transitions" << "\n";
		std::vector<Core::FMTtransition> modifiedTransitions;
		for (const Core::FMTtransition& TRANSITION : OptimizationModel.gettransitions())
		{
			modifiedTransitions.push_back(TRANSITION.single());
		}
		*Logger << "FMT -> Lecture des rasters" << "\n";
		OptimizationModel.settransitions(modifiedTransitions);
		Parser::FMTareaparser areaParser;
		const std::string RASTERS_PATH = msclr::interop::marshal_as<std::string>(cheminRasters);
		const std::string AGE_RASTER = RASTERS_PATH + "AGE.tif";
		std::vector<std::string> themeRasters;
		for (int i = 1; i <= OptimizationModel.getthemes().size(); i++)
			{
			themeRasters.push_back(RASTERS_PATH + "THEME" + std::to_string(i) + ".tif");
			}
		{
			Spatial::FMTforest initialForestMap;
			if (!indicateurStanlock)
				{
					initialForestMap = areaParser.readrasters(OptimizationModel.getthemes(), 
										themeRasters, AGE_RASTER, 1, 0.0001);
				}else
				{
					initialForestMap = areaParser.readrasters(OptimizationModel.getthemes(), 
									themeRasters, AGE_RASTER, 1, 0.0001, RASTERS_PATH + "STANLOCK.tif");
				}
				OptimizationModel.setinitialmapping(initialForestMap);
		}
		const std::string OUTPUT_PATH = msclr::interop::marshal_as<std::string>(cheminSorties);
		System::IO::DirectoryInfo^ parentDirectory = System::IO::Directory::GetParent(cheminSorties);
		System::String^ directoryFullName = parentDirectory->FullName;
		const std::string WORKING_DIRECTORY = msclr::interop::marshal_as<std::string>(directoryFullName);
		OptimizationModel.setparameter(Models::FMTstrmodelparameters::WORKING_DIRECTORY, WORKING_DIRECTORY);
		OptimizationModel.setparameter(Models::FMTintmodelparameters::LENGTH, periodes);
		OptimizationModel.setparameter(Models::FMTintmodelparameters::MAX_MOVES, p_MaxMoves);
		OptimizationModel.setparameter(Models::FMTintmodelparameters::MAX_ACCEPTED_CYCLE_MOVES, p_MaxAcceptedMoves);
		OptimizationModel.setparameter(Models::FMTintmodelparameters::MAX_CYCLE_MOVES, p_MaxCycleMoves);
		OptimizationModel.doplanning(true);

		
		

		const Spatial::FMTSpatialSchedule& SCHEDULE = OptimizationModel.getspschedule();

		RapportdeBris(OptimizationModel);
		System::Collections::Generic::List<int>^ growthThemes = gcnew System::Collections::Generic::List<int>();
		EcrituredesPerturbations(OptimizationModel, directoryFullName, periodes, growthThemes,false);

		if (indGenererEvents)
			{
			EcritureDesEvenements(OptimizationModel, directoryFullName, periodes, false);
			}

		if (outputs->Count > 0)
		{
			const std::vector<Core::FMToutput> OUTPUTS_LIST = EcritureDesOutputs(OptimizationModel, outputs, periodes, false);
			Parser::FMTscheduleparser scheduParser;
			System::String^ schedulePath = System::IO::Path::Combine(directoryFullName, 
											gcnew System::String(std::string(OptimizationModel.getname() + "_.seq").c_str()));
			const std::string stdSchedulePath = msclr::interop::marshal_as<std::string>(schedulePath);
			scheduParser.write(OptimizationModel.GetSchedules(SCHEDULE), stdSchedulePath);
				Parser::FMTmodelparser Modelparser;
				*Logger << "FMT -> Exportations des sorties " << "\n";
				Modelparser.writeresults(
					OptimizationModel,
					OUTPUTS_LIST,
					etanduSortiesMin,
					etanduSortiesMax,
					OUTPUT_PATH,
					static_cast<Core::FMToutputlevel>(outputLevel),
					msclr::interop::marshal_as<std::string>(providerGdal)
				);
			if (indSortiesSpatiales)
				{
				EcrituredesOutputsSpatiaux(OptimizationModel, OUTPUTS_LIST, etanduSortiesMin, etanduSortiesMax, directoryFullName);
				}
		}
	}catch (...)
		{
		raisefromcatch("", "Wrapper::FMTForm::OptimisationSpatialeExplicite", __LINE__, __FILE__);
		return false;
		}

	return true;
}