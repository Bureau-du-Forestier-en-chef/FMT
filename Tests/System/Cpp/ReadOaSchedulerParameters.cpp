/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTAreaParser.h"
#include "FMTModelParser.h"
#include "FMTModel.h"
#include "FMTDefaultLogger.h"
#ifdef FMTWITHOSI
    #include "FMTOperatingAreaScheme.h"
#endif

int main(int argc, char *argv[])
    {   
#ifdef FMTWITHOSI
    Logging::FMTDefaultLogger().logStamp();
    std::string primarylocation;
    std::string scenario;
    std::string fichierParam;
    if (argc>1)
    {
        primarylocation = std::string(argv[2]);
        scenario = std::string(argv[3]);
        fichierParam = std::string(argv[1]);
    }else{
        primarylocation = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/Interne/FMT/Entretien/Modeles_test/02751/PC_9509_U02751_4_Vg2_2023_vRp2.pri";
        scenario = "14_Sc5_Determin_apsp";
        fichierParam = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/Interne/FMT/Entretien/Modeles_test/parameters/parameters_2751_old.csv";

    }
    Parser::FMTModelParser modelparser;
    modelparser.setDefaultExceptionHandler();
    std::vector<Exception::FMTexc>errors;
    errors.push_back(Exception::FMTexc::FMTmissingyield);
    errors.push_back(Exception::FMTexc::FMToutput_missing_operator);
    errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
    errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
    errors.push_back(Exception::FMTexc::FMTundefinedoutput_attribute);
    errors.push_back(Exception::FMTexc::FMToveridedyield);
    errors.push_back(Exception::FMTexc::FMToutofrangeyield);
    errors.push_back(Exception::FMTexc::FMTsame_transitiontargets);
    errors.push_back(Exception::FMTexc::FMTmissingyield);
    modelparser.setErrorsToWarnings(errors);
    std::vector<std::string>scenarios(1, scenario);
    const std::vector<Models::FMTModel> models = modelparser.readproject(primarylocation, scenarios);
    Parser::FMTAreaParser areaParser;
    const std::vector<Heuristics::FMTOperatingAreaScheme> opeareas = areaParser.readOaSchedulerParameters(fichierParam, models.at(0).getThemes(), 13, 1);
#endif
    return 0;
	}

