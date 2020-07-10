Lire dans une autre langue: [English](README.md), [Fran�ais](README.fr.md)

# FMT - Forest Management Tool

## FMT est une librairie de type source ouverte pour l'am�nagement forestier

FMT est une librarie C++ faite pour interpr�t�e des mod�les de planification foresti�re bas� sur les fichiers Woodstock. FMT peut �tre utilis� avec Python, Cran-R ou directement en C++. Son approche g�n�rique � la planification foresti�re permet � l'utilisateur de manipuler des mod�les de planification foresti�re et de g�n�rer des solutions. L'objectif principal derri�re ce projet est d'amener les diff�rents types de mod�les de planification foresti�re sur une m�me librairie g�n�rique pour aider la communaut� � d�velopper de nouvelles approches pour r�soudre les probl�mes de planification foresti�re et d'am�liorer les approches d�j� pr�sente depuis longtemps.
## License

FMT utilise la license liliQ-R 1.1 [LICENSES\FR](LILIQ-R11.txt) pour plus d'information.

## Pourquoi FMT?

### Mod�le de planification spatialement r�f�renc�

FMT permet � l'utilisateur de g�n�rer des mod�les spatialement r�f�renc� de TYPE III et de les r�soudres. L'outil peut-�tre utilis� pour r�aliser des analyses de sensibilit�s sur plusieurs param�tres du mod�le de planification. L'outil peut aussi �tre utilis� pour analyser l'impact d'�v�nements stochastique comme les �pisodes de feux de for�t et de tordeuse des bourgeons de l'�pinette en utilisant la replanification,

### Spatialy explicit forest planning

FMT offre une plateforme sur laquelle construire des heuristiques pour solutionner des probl�mes de planification foresti�re. FMT est l'outils parfait pour solutionner des probl�mes sp�cifique aux enjeux de l'utilisateur comme l'allocation de calendrier de chantier de r�colte. Il permet aussi l'impl�mentation de processus de simulation. La librairie offre aussi des outils permettant la spatialisation de solution spatialement r�f�renc� et la g�n�ration de solution spatialement explicite. FMT est tr�s utile lorsqu'on doit impl�ment� des m�ta-heuristiques comme le Recuit Simul�, g�n�tique ou la recherche tabou dans un contexte de planification foresti�re.

Si votre intrant principal est un mod�le Woodstock FMT est probablement un bon choix de plateforme pour d�velopper de nouvelles approches d'optimisation ou de simulation de calendrier de r�colte ou d��v�nements stochastiques.


## D�pendances

### Requises :
  + Boost compil� avec zlib (http://boost.org)
### Optionnelles :
  + Librairie GEOS (http://trac.orgeo.org/geos/)
  + Librairie GDAL 2.3 > (http://GDAL.org)
  + Librairie Mosek (http://mosek.com)
  + Librairie OSI avec le solveur CBC (http://github.com/coin/Cbc)
  + Librairie Boost Python
  + Rcpp et cran-R avec la version de Rtools 40 (http://cran.r-project.org)

## Comment utiliser FMT?

La fa�on la plus simple d'utiliser FMT est d'utiliser cmake >= 3.15 et g�n�rer une librairie. Le script cmake supporte la g�n�ration de FMT sur Windows � l'aide de VS2017 ou MINGW64 sur MSYS2 ou CYGWIN.

+ G�n�ration de FMT en librairie Python sur Visual Studio 2017:

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

+ G�n�ration de  FMT en librairie R sur MINGW64 avec MSYS2 (Rtools40)
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

+ Pour g�n�rer la documentation Doxygen (http://www.doxygen.nl) dans le dossier documentation:

 ``` powershell
  cd Documentation
  doxygen FMTdoxygenconfig
 ```
+ Pour populer les commentaires dans les fichiers source R et Python (fichier commen�ant par R ou PY):

 ```powershell
  cd Documentation
  python commentsPythonandR.py
 ```
 ## Exemples
 L'exemple suivant lit un mod�le, g�n�re un mod�le de programmation lin�aire de planification foresti�re de type III et solutionne le probl�me.
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
library(Rcpp)
FMT<-Module("FMT",dyn.load(paste(pathtopackage,"\\FMT.dll",sep="")))
modelparser<-new(FMT$FMTmodelparser)
models<-modelparser$readproject("pathtoprimaryfile",c("scenariox"),TRUE,TRUE,TRUE)#read scenario x from the primay file (.pri)
solverinterface<-list(CLP=1,MOSEK=2,CPLEX=3,GUROBI=4)
optimizationmodel<-new(FMT$FMTlpmodel,models[[1]],solverinterface$CLP)#Construct a type III linear programming model from the FMTmodel
emptyschedule<-new(FMT$FMTschedule)
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
``` C++11
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
