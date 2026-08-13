<a href = "./README.fr.md"><img src = "https://img.shields.io/badge/%F0%9F%87%AB%F0%9F%87%B7-Click%20here%20for%20a%20french%20version-blue?style=flat-square" height="25" /></a>

<p align="center">
  <img src="https://raw.githubusercontent.com/Bureau-du-Forestier-en-chef/FMT/master/Documentation/FMTlogo.png" />
</p>
<h1 align="center">FMT (Forest Management Tool)</h1>
<br><br>
<p align="center">
	<a href = "https://bureau-du-forestier-en-chef.github.io/FMTdocs/"><img src = "https://img.shields.io/badge/%F0%9F%93%9A-Take%20a%20look%20at%20the%20documentation%20!-green?style=flat-square" height="45" /></a>
</p>
<br><br>

*This page provides only an overview of the installation process and features of FMT. For more details, visit our documentation page [here](https://bureau-du-forestier-en-chef.github.io/FMTdocs/).*

## 📑 Description

FMT (**F**orest **M**anagement **T**ool) is an **open source Forest Management library**.

- It is a C++17 library made to interpret forest planning models based on the Woodstock files formulation. 
- It can be used through Python, Cran-R or directly from C++. 
- It's generic approach to forest planning gives the opportunity to the user to manipulate forest planning models and generate forest planning solutions. 
- It's objective is to bring all forest planning models types onto the same generic library to help the community to develop new approaches to forest planning problems or improve the one that have been there for years.

## ✨ Features

### Spatially referenced forest planning

FMT allows the user to generate and solve spatially referenced type II or III forest planning model problems. The tool can be used to do sensitivity analyses over multiple parameters of the forest planning model. It can also be used for analysing impacts of stochastic events such as fire or spruce budworm using Replanning.

### Spatially explicit forest planning

FMT gives a framework to construct heuristics to solve forest planning model. FMT is the perfect tool to solve user specific problems related to forest planning like operating area scheduling. It can also be used to implement simple simulation process. The library offers some tools to spatialize spatially referenced solutions or to generate spatially explicit solutions.

FMT is extremely usefull when it comes to implement metaheuristics such as [Simulated Annealing](https://en.wikipedia.org/wiki/Simulated_annealing), [Genetic](https://en.wikipedia.org/wiki/Genetic_algorithm) or [Tabu](https://en.wikipedia.org/wiki/Tabu_search) Search into a forest planning context.

If your main inputs are Woodstock files, FMT is probably the way to go to develop new approaches for optimization or simulation of harvest schedules or even stochastic events.

## 💽 Installation

The easiest way to use FMT is to use Cmake >= 3.15 to generate a shared library for R or Python, or directly from C++. The Cmake script of this repository supports generating FMT on Windows using MSVS >= 2017 or MINGW-64 on MSYS2 or CYGWIN.

<br><br>
<p align="center">
	<a href = "https://bureau-du-forestier-en-chef.github.io/FMTdocs/docs/download_install/"><img src = "https://img.shields.io/badge/%F0%9F%92%BD-See%20installation%20instructions%20in%20the%20documentation-yellow?style=flat-square" height="45" /></a>
</p>
<br><br>

## 📦 Dependencies

### Required :
  + Boost (http://boost.org)
### Optional :
  + GEOS library (http://trac.orgeo.org/geos/)
  + GDAL library (http://GDAL.org)
  + Mosek library (http://mosek.com)
  + GLPK library (https://github.com/Sonderfall/GLPK)
  + CLP library (https://github.com/coin-or/Clp)
  + OSI library  (http://github.com/coin/Cbc)
  + Boost Python library
  + Rcpp and cran-R with at least Rtools40 (http://cran.r-project.org)
  + OnnxRuntime library (https://github.com/microsoft/onnxruntime)

## 📖 Documentation

**FMT has a handwritten extensive documentation with exercises and examples**, along with a doxygen-generated documentation that are both available on the FMT documentation webpage.

<br><br>
<p align="center">
	<a href = "https://bureau-du-forestier-en-chef.github.io/FMTdocs/"><img src = "https://img.shields.io/badge/%F0%9F%93%9A-Take%20a%20look%20at%20the%20documentation%20!-green?style=flat-square" height="45" /></a>
</p>
<br><br>

## 👉 Examples

 Here's a short example for reading a forest planning model and solve the type II Linear programming formulation.
 + Using Python
  ```python
from FMT import Core
from FMT import Models
from FMT import Parser

modelParser = Parser.FMTmodelparser()
### Read the model
MODELS = modelParser.readproject("Examples/Models/TWD_land/TWD_land.pri",["LP"])
TypeIIModel=Models.FMTlpmodel(MODELS[0],Models.FMTsolverinterface.CLP)
## Set a length of 10 periods
LENGTH = 10
TypeIIModel.setparameter(Models.FMTintmodelparameters.LENGTH,10)
## Solve
OPTIMAL = TypeIIModel.doplanning(True) #Solutionne le modèle
## Get the objective and outputs values
if OPTIMAL:
    print("OBJECTIVE: ",TypeIIModel.getobjectivevalue())
    for OUTPUT in TypeIIModel.getoutputs():
        for PERIOD in range(0,LENGTH):
            print(PERIOD,TypeIIModel.getoutput(OUTPUT,PERIOD,Core.FMToutputlevel.totalonly))
 ```
 + Using R
 ```R
library(FMT)
modelParser<-new(FMTmodelparser)
### Read the model
MODELS<-modelParser$readproject("Examples/Models/TWD_land/TWD_land.pri",c("LP"),TRUE,TRUE,TRUE)
TypeIIModel<-new(FMTlpmodel,MODELS[[1]],FMTsolverinterface$CLP)
EMPTY_SCHEDULE<-new(FMTschedule)
## Set a length of 10 periods
LENGTH <- 10L
TypeIIModel$setintparameter(FMTintmodelparameters$LENGTH,LENGTH)
EMPTY_SCHEDULES<-list()
## Solve
OPTIMAL <- TypeIIModel$doplanning(TRUE,EMPTY_SCHEDULES)
## Get the objective and outputs values
if (OPTIMAL)
    {
    print(paste("OBJECTIVE: ", TypeIIModel$getobjectivevalue()))
    OUTPUTS <- TypeIIModel$getoutputsdataframe(TypeIIModel$getoutputs(), 1, LENGTH)
    print(OUTPUTS)
    }
 ```
+ Using C++
``` C++
#include <iostream>
#include <vector>
#include "FMTLpModel.h"
#include "FMTModelParser.h"

Parser::FMTModelParser modelParser;
const std::vector<std::string>SCENARIOS(1, "LP");
// Read the model
const std::vector<Models::FMTModel> MODELS = modelparser.readproject("Examples/Models/TWD_land/TWD_land.pri", SCENARIOS);
Models::FMTLpModel TypeIIModel(MODELS.at(0), Models::FMTsolverinterface::CLP);
// Set a length of 10 periods
const int LENGTH = 10;
TypeIIModel.setparameter(Models::FMTintmodelparameters::LENGTH,LENGTH);
// Solve
const bool OPTIMAL = TypeIIModel.doPlanning(true);
// Get the objective and outputs values
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

## 🔑 License 

FMT is a [LiLiQ-R 1.1](https://github.com/gcyr/FMT/blob/master/LICENSES/EN/LILIQ-R11EN.pdf) licensed library.

[![License](http://img.shields.io/:license-liliqR11-blue.svg?style=flat-square)](https://forge.gouv.qc.ca/licence/liliq-v1-1/#r%C3%A9ciprocit%C3%A9-liliq-r)


## 📈 Coverage
GNU ![](https://img.shields.io/badge/coverage-71%25-orange)
MSVC ![](https://img.shields.io/badge/coverage-0%25-red)


## 💻 Build
GNU ![](https://img.shields.io/badge/build-pass-brightgreen)
MSVC ![](https://img.shields.io/badge/build-pass-brightgreen)n)n)n)
