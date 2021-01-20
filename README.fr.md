Lire dans une autre langue: [English](README.md), [Français](README.fr.md)

# FMT - Forest Management Tool

## FMT est une librairie de type logiciel libre pour l'aménagement forestier

FMT est une librairie C++ faite pour interpréter des modèles de planification forestière basés sur les fichiers Woodstock. FMT peut être utilisé avec Python, Cran-R ou directement en C++. Son approche générique à la planification forestière permet à l'utilisateur de manipuler des modèles de planification forestière et de générer des solutions. L'objectif principal derrière ce projet est d'amener les différents types de modèles de planification forestière sur une même librairie générique pour aider la communauté à développer de nouvelles approches pour résoudre les problèmes de planification forestière et d'améliorer les approches déjà présente depuis longtemps.
## License 

FMT utilise la license LiLiQ-R 1.1 [LICENSES\FR](LILIQ-R11.txt) pour plus d'information.

## Pourquoi FMT?

### Modèle de planification spatialement référencé

FMT permet à l'utilisateur de générer et de résoudre des modèles spatialement référencé de TYPE III. L'outil peut être utilisé pour réaliser des analyses de sensibilité sur plusieurs paramètres du modèle de planification. L'outil peut aussi être utilisé pour analyser l'impact d'évènements stochastiques comme les épisodes de feux de forêt et de tordeuse des bourgeons de l'épinette en utilisant la replanification, 

### Modèle de planification spatialement explicite

FMT offre une plateforme sur laquelle construire des heuristiques pour solutionner des problèmes de planification forestière. FMT est l'outil parfait pour solutionner des problèmes spécifiques aux enjeux de l'utilisateur comme l'allocation de calendrier de chantier de récolte. Il permet aussi l'implémentation de processus de simulation. La librairie offre aussi des outils permettant la spatialisation de solution spatialement référencée et la génération de solution spatialement explicite. FMT est très utile lorsqu'on doit implémenter des méta-heuristiques comme le Recuit Simulé, génétique ou la recherche tabou dans un contexte de planification forestière. 

Si votre intrant principal est un modèle Woodstock FMT est probablement un bon choix de plateforme pour développer de nouvelles approches d'optimisation ou de simulation de calendrier de récolte ou d’événements stochastiques.


## Dépendances

### Requises :
  + Boost compilé avec zlib (http://boost.org)
### Optionnelles :
  + Librairie GEOS (http://trac.orgeo.org/geos/)
  + Librairie GDAL (http://GDAL.org)
  + Librairie Mosek (http://mosek.com)
  + Librairie OSI avec le solveur CBC (http://github.com/coin/Cbc)
  + Librairie Boost Python
  + Rcpp et cran-R avec la version de Rtools 40 (http://cran.r-project.org)
  
## Comment utiliser FMT?

La façon la plus simple d'utiliser FMT est d'utiliser cmake >= 3.15 et de générer une librairie R ou Python our directement en C++ (voir les exemples: FMTSimpleplanning.exe and FMTSpatialyexplicitsimulation.exe). Le script cmake supporte la génération de FMT sur Windows à l'aide de VS2017 ou MINGW64 sur MSYS2 ou CYGWIN.

+ Génération de FMT en librairie Python sur Visual Studio 2017: 
 
 ```powershell
 ::Providing the -DPYTHON_DIR will make the script generate a .pyd using Boost Python
 ::You cannot generate an Rcpp module .dll with VS so dont use -DR_DIR with VS
 call vcvars64.bat
 cmake CMakeLists.txt -DCMAKE_BUILD_TYPE=Release ^
	-DMOSEK_DIR="path to MOSEK" ^
	-DOSI_DIR="path to OSI" ^
	-DBOOST_DIR="path to BOOST" ^
	-DGDAL_DIR="path to GDAL" ^
	-DGEOS_DIR="path to GEOS" ^
	-DPYTHON_DIR="path to PYTHON" ^
	-G "Visual Studio 15 2017 Win64"
  cmake --build . --config Release
  ::This will copy dependencies next to FMT.pyd.
  cmake --install . --config Release
```

+ Génération de  FMT en librairie R sur MINGW64 avec MSYS2 (Rtools40)
 ```bash
 #Here is a exemple for generating FMT module .dll for R
 #Python .pyd could also be generated using -DPYTHON_DIR but dont use -DPYTHON_DIR and -DR_DIR at the same time
 #To generate a R .dll you need to use at least Rtools40 R4 >.
 cmake CMakeLists.txt -DCMAKE_BUILD_TYPE=Release \
	-DMOSEK_DIR="path to MOSEK" \
	-DOSI_DIR="path to OSI" \
	-DBOOST_DIR="path to BOOST" \
	-DGDAL_DIR="path to GDAL" \
	-DR_DIR="path to R" \
	-DGEOS_DIR="path to GEOS" \
	-G "MSYS Makefiles"
 cmake --build . --config Release
 #This will copy dependencies next to FMT.dll.
 cmake --install . --config Release
 ```

## Documentation

+ Pour générer la documentation Doxygen (http://www.doxygen.nl) dans le dossier documentation:

 ``` powershell
  cd Documentation
  doxygen FMTdoxygenconfig
 ```
+ Pour charger les commentaires dans les fichiers source R et Python (fichier commençant par R ou PY):

 ```powershell
  cd Documentation
  python commentsPythonandR.py
 ```
 ## Exemples
 L'exemple suivant lit un modèle, génère un modèle de programmation linéaire de planification forestière de type III et solutionne le problème.
 + En utilisant Python
  ```python
  from FMT import Models
from FMT import Parser

modelparser = WSParser.FMTmodelparser()
models = modelparser.readproject("pathtoprimaryfile",["scenariox"])#read scenario x from the primay file (.pri)
optimizationmodel=Models.FMTlpmodel(models[0],Models.FMTsolverinterface.CLP)#Construct a type III linear programming model from the FMTmodel

#Build the graph for 10 periods
for period in range(0,10):
        print(optimizationmodel.buildperiod())

#Now add objective function to the FMTlpmodel and all the constraints
constraints = optimizationmodel.getconstraints()
objective = constraints.pop(0)
print(optimizationmodel.setobjective(objective))
for constraint in constraints:
    print(optimizationmodel.setconstraint(constraint))

#Do a initial solve of the forest planning model
optimizationmodel.initialsolve()
 ```
  + En utilisant R
  ```R
library(FMT)
modelparser<-new(FMTmodelparser)
models<-modelparser$readproject("pathtoprimaryfile",c("scenariox"),TRUE,TRUE,TRUE)#read scenario x from the primay file (.pri)
solverinterface<-list(CLP=1,MOSEK=2,CPLEX=3,GUROBI=4)
optimizationmodel<-new(FMTlpmodel,models[[1]],solverinterface$CLP)#Construct a type III linear programming model from the FMTmodel
emptyschedule<-new(FMTschedule)
#Build the graph for 10 periods
for (period in 1:10)
	{
	print(optimizationmodel$buildperiod(emptyschedule,FALSE)$str())
	}
#Now add objective function to the FMTlpmodel and all the constraints
allmodelconstraints<-optimizationmodel$getconstraints()
modelobjective<-allmodelconstraints[[1]]
print(optimizationmodel$setobjective(modelobjective)$str())
modelconstraints<-allmodelconstraints[2:length(allmodelconstraints)]
for (constraint in modelconstraints)
	{
	print(optimizationmodel$setconstraint(constraint)$str())
	}
#Do a initial solve of the forest planning model
optimizationmodel$initialsolve()
```
+ En utilisant C++
``` C++
#include <vector>
#include "FMTlpmodel.h"
#include "FMTmodelparser.h"

Parser::FMTmodelparser modelparserparser;
const std::vector<std::string>scenarios(1, "scenariox");
const std::vector<Models::FMTmodel> models = modelparserparser.readproject("pathtoprimaryfile", scenarios);
Models::FMTlpmodel optimizationmodel(models.at(0), Models::FMTsolverinterface::MOSEK);
for (size_t period = 0; period < 10; ++period)
	{
	optimizationmodel.buildperiod();
	}
std::vector<Core::FMTconstraint>constraints = optimizationmodel.getconstraints();
const Core::FMTconstraint objective = constraints.at(0);
optimizationmodel.setobjective(objective);
constraints.erase(constraints.begin());
for (const Core::FMTconstraint& constraint : constraints)
	{
	optimizationmodel.setconstraint(constraint);
	}
optimizationmodel.initialsolve();
```
[![License](http://img.shields.io/:license-liliqR11-blue.svg?style=flat-square)](https://forge.gouv.qc.ca/licence/liliq-v1-1/#r%C3%A9ciprocit%C3%A9-liliq-r)
