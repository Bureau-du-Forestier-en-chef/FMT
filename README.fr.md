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

- C'est une librairie C++ faite pour interpréter des modèles de planification forestière basés sur les fichiers Woodstock.
- Elle peut être utilisé avec Python, Cran-R ou directement en C++.
- Son approche générique de la planification forestière permet à l'utilisateur de manipuler des modèles de planification forestière et de générer des solutions.
- Son objectif principal est d'amener les différents types de modèles de planification forestière sur une même librairie générique pour aider la communauté à développer de nouvelles approches pour résoudre les problèmes de planification forestière, et d'améliorer les approches déjà présentes depuis longtemps.

# ✨ Capacités

## Modèle de planification spatialement référencé

FMT permet à l'utilisateur de générer et de résoudre des modèles spatialement référencé de TYPE III. L'outil peut être utilisé pour réaliser des analyses de sensibilité sur plusieurs paramètres du modèle de planification. L'outil peut aussi être utilisé pour analyser l'impact d'évènements stochastiques comme les épisodes de feux de forêt et de tordeuse des bourgeons de l'épinette en utilisant la replanification.

## Modèle de planification spatialement explicite

FMT offre une plateforme sur laquelle construire des heuristiques pour solutionner des problèmes de planification forestière. FMT est l'outil parfait pour solutionner des problèmes spécifiques aux enjeux de l'utilisateur comme l'allocation de calendrier de chantier de récolte. Il permet aussi l'implémentation de processus de simulation. La librairie offre aussi des outils permettant la spatialisation de solution spatialement référencée et la génération de solution spatialement explicite.

FMT est très utile lorsqu'on doit implémenter des méta-heuristiques comme le [Recuit Simulé](https://en.wikipedia.org/wiki/Simulated_annealing), la recherche [génétique](https://en.wikipedia.org/wiki/Genetic_algorithm) ou la recherche [tabou](https://en.wikipedia.org/wiki/Tabu_search) dans un contexte de planification forestière.

Si votre intrant principal est un modèle Woodstock, FMT est probablement un bon choix de plateforme pour développer de nouvelles approches d'optimisation ou de simulation de calendrier de récolte ou d’événements stochastiques.

# 💽 Installation

La manière la plus simple d'utiliser FMT est d'utiliser Cmake >= 3.15 pour génerer une librairie partagée pour R ou pour Python, ou bien directement en C++. Le script Cmake de ce *repository* permet de génerer FMT sous Windows en utilisant VS2017/2019 ou MINGW-64 sur MSYS2 ou CYGWIN.

<br><br>
<p align="center">
	<a href = "https://bureau-du-forestier-en-chef.github.io/FMTdocs/fr/docs/download_install/"><img src = "https://img.shields.io/badge/%F0%9F%92%BD-Installation%20pas%20%C3%A0%20pas%20dans%20la%20documentation-yellow?style=flat-square" height="45" /></a>
</p>
<br><br>

# 📦 Dépendances

## Requises :
  + Boost compilé avec zlib. (http://boost.org)
## Optionelles :
  + Librairie GEOS (http://trac.orgeo.org/geos/)
  + Librairie GDAL (http://GDAL.org)
  + Librairie Mosek (http://mosek.com)
  + Librairie OSI par le biais du solveur CBC (http://github.com/coin/Cbc)
  + Librairie Boost Python
  + Rcpp et cran-R avec la version la plus récente de Rtools40 (http://cran.r-project.org)

# 📖 Documentation

FMT possède une documentation écrite à la main, détaillée et avec des exercices; ainsi qu'une documentation générée par le biais de Doxygen. Les deux sont accessibles via la page web de documentation de FMT.

<br><br>
<p align="center">
	<a href = "https://bureau-du-forestier-en-chef.github.io/FMTdocs/fr/"><img src = "https://img.shields.io/badge/%F0%9F%93%9A-Regardez%20la%20documentation%20!-green?style=flat-square" height="45" /></a>
</p>
<br><br>

**Si vous souhaitez compiler la documentation Doxygen par vous même**, dans une invite de commande Powershell, allez dans le dossier FMT et utilisez :

 ```powershell
  cd Documentation
  doxygen FMTdoxygenconfig
 ```
Pour remplir les commentaires dans les fichiers sources R (`.R`) et Python (`.py`), utilisez :

 ```powershell
  cd Documentation
  python commentsPythonandR.py
 ```
Voici une présentation pour vous former à l'API FMT(R/Python) [Formation_FMT_R_Python.pptx](Documentation/Formation_FMT_R_Python.pptx) 

# 👉 Exemples

L'exemple suivant lit un modèle, génère un modèle de programmation linéaire de planification forestière de type III et solutionne ce modèle.
 + En utilisant Python
  ```python
  from FMT import Models
from FMT import Parser

modelparser = Parser.FMTmodelparser()
models = modelparser.readproject("pathtoprimaryfile",["scenariox"])# Lit le scénario x depuis un fichier primaire (.pri)
optimizationmodel=Models.FMTlpmodel(models[0],Models.FMTsolverinterface.CLP)# Construit un modèle de programmation linéaire de type III depuis un FMTmodel

### Construit le graphe pendant 10 périodes
for period in range(0,10):
        print(optimizationmodel.buildperiod())

# Maintenant, on ajoute la fonction d'object au FMTlpmodel et toutes les contraintes
constraints = optimizationmodel.getconstraints()
objective = constraints.pop(0)
for constraint in constraints:
    print(optimizationmodel.setconstraint(constraint))
print(optimizationmodel.setobjective(objective))
# On fait une résolution initiale du modèle de planification forestière
optimizationmodel.initialsolve()
 ```
 + En utilisant R
 ```R
library(FMT)
modelparser<-new(FMTmodelparser)
models<-modelparser$readproject("pathtoprimaryfile",c("scenariox"),TRUE,TRUE,TRUE)# Lit le scénario x depuis un fichier primaire (.pri)
optimizationmodel<-new(FMTlpmodel,models[[1]],FMTsolverinterface$CLP)# Construit un modèle de programmation linéaire de type III depuis un FMTmodel
emptyschedule<-new(FMTschedule)

### Construit le graphe pendant 10 périodes
for (period in 1:10)
  {
  print(optimizationmodel$buildperiod(emptyschedule,FALSE)$str())
  }
# Maintenant, on ajoute la fonction d'object au FMTlpmodel et toutes les contraintes
allmodelconstraints<-optimizationmodel$getconstraints()
modelobjective<-allmodelconstraints[[1]]
modelconstraints<-allmodelconstraints[2:length(allmodelconstraints)]
for (constraint in modelconstraints)
  {
  print(optimizationmodel$setconstraint(constraint)$str())
  }
print(optimizationmodel$setobjective(modelobjective)$str())
# On fait une résolution initiale du modèle de planification forestière
optimizationmodel$initialsolve()
 ```
+ En utilisant C++
``` C++
#include <vector>
#include "FMTlpmodel.hpp"
#include "FMTmodelparser.hpp"

Parser::FMTmodelparser modelparser;
const std::vector<std::string>scenarios(1, "scenariox");
const std::vector<Models::FMTmodel> models = modelparser.readproject("pathtoprimaryfile", scenarios);
Models::FMTlpmodel optimizationmodel(models.at(0), Models::FMTsolverinterface::MOSEK);
for (size_t period = 0; period < 10; ++period)
  {
  optimizationmodel.buildperiod();
  }
std::vector<Core::FMTconstraint>constraints = optimizationmodel.getconstraints();
const Core::FMTconstraint objective = constraints.at(0);
constraints.erase(constraints.begin());
for (const Core::FMTconstraint& constraint : constraints)
  {
  optimizationmodel.setconstraint(constraint);
  }
optimizationmodel.setobjective(objective);
optimizationmodel.initialsolve();
```

# 🔑 License 

FMT utilise la license [LiLiQ-R 1.1](https://github.com/gcyr/FMT/blob/master/LICENSES/EN/LILIQ-R11EN.pdf).

[![License](http://img.shields.io/:license-liliqR11-blue.svg?style=flat-square)](https://forge.gouv.qc.ca/licence/liliq-v1-1/#r%C3%A9ciprocit%C3%A9-liliq-r)
