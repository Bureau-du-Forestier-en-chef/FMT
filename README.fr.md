<a href = "./README.md"><img src = "https://img.shields.io/badge/%F0%9F%87%A8%F0%9F%87%A6-Cliquez%20ici%20pour%20la%20version%20anglaise-red?style=flat-square" height="25" /></a>

<p align="center">
  <img src="https://raw.githubusercontent.com/Bureau-du-Forestier-en-chef/FMT/master/Documentation/FMTlogo.png" />
</p>
<h1 align="center">FMT (Forêt Modélisée dans le Temps)</h1>
<br><br>
<p align="center">
	<a href = "https://bureau-du-forestier-en-chef.github.io/FMTdocs/fr/"><img src = "https://img.shields.io/badge/%F0%9F%93%9A-Regardez%20la%20documentation%20!-green?style=flat-square" height="45" /></a>
</p>
<br><br>

*Cette page ne donne qu'un aperçu du processus d'installation et des fonctionnalités de FMT. Pour plus détails, visitez note page de documentation pour [FMT](https://bureau-du-forestier-en-chef.github.io/FMTdocs/fr/).*


# 📑 Description

FMT (**F**orêt **M**odélisée dans le **T**emps) est une **librairie de type logiciel libre pour l'aménagement forestier**.

- C'est une librairie C++17 faite pour interpréter des modèles de planification forestière basés sur les fichiers Woodstock.
- Elle peut être utilisé avec Python, Cran-R ou directement en C++.
- Son approche générique de la planification forestière permet à l'utilisateur de manipuler des modèles de planification forestière et de générer des solutions.
- Son objectif principal est d'amener les différents types de modèles de planification forestière sur une même librairie générique pour aider la communauté à développer de nouvelles approches pour résoudre les problèmes de planification forestière, et d'améliorer les approches déjà présentes depuis longtemps.

# ✨ Capacités

## Modèle de planification spatialement référencé

FMT permet à l'utilisateur de générer et de résoudre des modèles spatialement référencé de TYPE II ou III. L'outil peut être utilisé pour réaliser des analyses de sensibilité sur plusieurs paramètres du modèle de planification. L'outil peut aussi être utilisé pour analyser l'impact d'évènements stochastiques comme les épisodes de feux de forêt et de tordeuse des bourgeons de l'épinette en utilisant la replanification.

## Modèle de planification spatialement explicite

FMT offre une plateforme sur laquelle construire des heuristiques pour solutionner des problèmes de planification forestière. FMT est l'outil parfait pour solutionner des problèmes spécifiques aux enjeux de l'utilisateur comme l'allocation de calendrier de chantier de récolte. Il permet aussi l'implémentation de processus de simulation. La librairie offre aussi des outils permettant la spatialisation de solution spatialement référencée et la génération de solution spatialement explicite.

FMT est très utile lorsqu'on doit implémenter des méta-heuristiques comme le [Recuit Simulé](https://en.wikipedia.org/wiki/Simulated_annealing), la recherche [génétique](https://en.wikipedia.org/wiki/Genetic_algorithm) ou la recherche [tabou](https://en.wikipedia.org/wiki/Tabu_search) dans un contexte de planification forestière.

Si votre intrant principal est un modèle Woodstock, FMT est probablement un bon choix de plateforme pour développer de nouvelles approches d'optimisation ou de simulation de calendrier de récolte ou d’événements stochastiques.

# 💽 Installation

La manière la plus simple d'utiliser FMT est d'utiliser Cmake >= 3.15 pour génerer une librairie partagée pour R ou pour Python, ou bien directement en C++. Le script Cmake de ce *repository* permet de génerer FMT sous Windows en utilisant MSVC >= 2017 ou MINGW-64 sur MSYS2 ou CYGWIN.

<br><br>
<p align="center">
	<a href = "https://bureau-du-forestier-en-chef.github.io/FMTdocs/fr/docs/download_install/"><img src = "https://img.shields.io/badge/%F0%9F%92%BD-Installation%20pas%20%C3%A0%20pas%20dans%20la%20documentation-yellow?style=flat-square" height="45" /></a>
</p>
<br><br>

# 📦 Dépendances

### Requises :
  + Boost (http://boost.org)
### Optionelles :
  + Librairie GEOS (http://trac.orgeo.org/geos/)
  + Librairie GDAL (http://GDAL.org)
  + Librairie Mosek (http://mosek.com)
  + Librairie GLPK (https://github.com/Sonderfall/GLPK)
  + Librairie CLP (https://github.com/coin-or/Clp)
  + Librairie OSI  (http://github.com/coin/Cbc)
  + Librairie Boost Python
  + Rcpp and cran-R with at least Rtools40 (http://cran.r-project.org)
  + Librairie OnnxRuntime (https://github.com/microsoft/onnxruntime)

# 📖 Documentation

FMT possède une documentation écrite à la main, détaillée et avec des exercices; ainsi qu'une documentation générée par le biais de Doxygen. Les deux sont accessibles via la page web de documentation de FMT.

<br><br>
<p align="center">
	<a href = "https://bureau-du-forestier-en-chef.github.io/FMTdocs/fr/"><img src = "https://img.shields.io/badge/%F0%9F%93%9A-Regardez%20la%20documentation%20!-green?style=flat-square" height="45" /></a>
</p>
<br><br>

Voici une présentation pour vous former à l'API FMT(R/Python) [Formation_FMT_R_Python.pptx](Documentation/Formation_FMT_R_Python.pptx) 

# 👉 Exemples

L'exemple suivant lit un modèle, génère un modèle de programmation linéaire de planification forestière de type II et solutionne ce modèle.
 + En utilisant Python
  ```python
from FMT import Core
from FMT import Models
from FMT import Parser

modelParser = Parser.FMTmodelparser()
### Lecture du modèle
MODELS = modelParser.readproject("Examples/Models/TWD_land/TWD_land.pri",["LP"])
TypeIIModel=Models.FMTlpmodel(MODELS[0],Models.FMTsolverinterface.CLP)
## Configurer pour 10 périodes
LENGTH = 10
TypeIIModel.setparameter(Models.FMTintmodelparameters.LENGTH,LENGTH)
## Solutionner
OPTIMAL = TypeIIModel.doplanning(True) #Solutionne le modèle
## Obtenir l'objectif et les outputs
if OPTIMAL:
    print("OBJECTIVE: ",TypeIIModel.getobjectivevalue())
    for OUTPUT in TypeIIModel.getoutputs():
        for PERIOD in range(0,LENGTH):
            print(PERIOD,OUTPUT.getname(),TypeIIModel.getoutput(OUTPUT,PERIOD,Core.FMToutputlevel.totalonly))
 ```
 + En utilisant R
 ```R
library(FMT)
modelParser<-new(FMTmodelparser)
### Lecture du modèle
MODELS<-modelParser$readproject("Examples/Models/TWD_land/TWD_land.pri",c("LP"),TRUE,TRUE,TRUE)
TypeIIModel<-new(FMTlpmodel,MODELS[[1]],FMTsolverinterface$CLP)
EMPTY_SCHEDULE<-new(FMTschedule)
## Configurer pour 10 périodes
LENGTH <- 10L
TypeIIModel$setintparameter(FMTintmodelparameters$LENGTH,LENGTH)
EMPTY_SCHEDULES<-list()
## Solutionner
OPTIMAL <- TypeIIModel$doplanning(TRUE,EMPTY_SCHEDULES)
## Obtenir l'objectif et les outputs
if (OPTIMAL)
    {
    print(paste("OBJECTIVE: ", TypeIIModel$getobjectivevalue()))
    OUTPUTS <- TypeIIModel$getoutputsdataframe(TypeIIModel$getoutputs(), 1, LENGTH)
    print(OUTPUTS)
    }
 ```
+ En utilisant C++
``` C++
#include <iostream>
#include <vector>
#include "FMTLpModel.h"
#include "FMTModelParser.h"

Parser::FMTModelParser modelParser;
const std::vector<std::string>SCENARIOS(1, "LP");
// Lecture du modèle
const std::vector<Models::FMTModel> MODELS = modelparser.readproject("Examples/Models/TWD_land/TWD_land.pri", SCENARIOS);
Models::FMTLpModel TypeIIModel(MODELS.at(0), Models::FMTsolverinterface::CLP);
// Configurer pour 10 périodes
const int LENGTH = 10;
TypeIIModel.setparameter(Models::FMTintmodelparameters::LENGTH,LENGTH);
// Solutionner
const bool OPTIMAL = TypeIIModel.doPlanning(true);
// Obtenir l'objectif et les outputs
if (OPTIMAL)
    {
    std::cout<<"OBJECTIVE: "<<TypeIIModel.getObjectiveValue()<<std::endl;
    for (const Core::FMToutput& OUTPUT : TypeIIModel.getOutputs())
        {
        for(int PERIOD = 0; PERIOD < LENGTH;++PERIOD)
            {
            std::cout<<PERIOD <<" "<<OUTPUT.getName()<<" "<<TypeIIModel.getOutput(OUTPUT,PERIOD,Core::FMToutputlevel::totalonly).at("Total")<<std::endl;
            }
        }
    }
```

# 🔑 License 

FMT utilise la license [LiLiQ-R 1.1](https://github.com/gcyr/FMT/blob/master/LICENSES/EN/LILIQ-R11EN.pdf).

[![License](http://img.shields.io/:license-liliqR11-blue.svg?style=flat-square)](https://forge.gouv.qc.ca/licence/liliq-v1-1/#r%C3%A9ciprocit%C3%A9-liliq-r)

## 📈 Coverage
GNU ![](https://img.shields.io/badge/coverage-71%25-orange)
MSVC ![](https://img.shields.io/badge/coverage-0%25-red)


## 💻 Build
GNU ![](https://img.shields.io/badge/build-pass-brightgreen)
MSVC ![](https://img.shields.io/badge/build-pass-brightgreen)


