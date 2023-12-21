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

- It is a C++ library made to interpret forest planning models based on the Woodstock files formulation. 
- It can be used through Python, Cran-R or directly from C++. 
- It's generic approach to forest planning gives the opportunity to the user to manipulate forest planning models and generate forest planning solutions. 
- It's objective is to bring all forest planning models types onto the same generic library to help the community to develop new approaches to forest planning problems or improve the one that have been there for years.

## ✨ Features

### Spatially referenced forest planning

FMT allows the user to generate and solve spatially referenced type III forest planning model problems. The tool can be used to do sensitivity analyses over multiple parameters of the forest planning model. It can also be used for analysing impacts of stochastic events such as fire or spruce budworm using Replanning.

### Spatially explicit forest planning

FMT gives a framework to construct heuristics to solve forest planning model. FMT is the perfect tool to solve user specific problems related to forest planning like operating area scheduling. It can also be used to implement simple simulation process. The library offers some tools to spatialize spatially referenced solutions or to generate spatially explicit solutions.

FMT is extremely usefull when it comes to implement metaheuristics such as [Simulated Annealing](https://en.wikipedia.org/wiki/Simulated_annealing), [Genetic](https://en.wikipedia.org/wiki/Genetic_algorithm) or [Tabu](https://en.wikipedia.org/wiki/Tabu_search) Search into a forest planning context.

If your main inputs are Woodstock files, FMT is probably the way to go to develop new approaches for optimization or simulation of harvest schedules or even stochastic events.

## 💽 Installation

The easiest way to use FMT is to use Cmake >= 3.15 to generate a shared library for R or Python, or directly from C++. The Cmake script of this repository supports generating FMT on Windows using VS2017/2019 or MINGW-64 on MSYS2 or CYGWIN.

<br><br>
<p align="center">
	<a href = "https://bureau-du-forestier-en-chef.github.io/FMTdocs/docs/download_install/"><img src = "https://img.shields.io/badge/%F0%9F%92%BD-See%20installation%20instructions%20in%20the%20documentation-yellow?style=flat-square" height="45" /></a>
</p>
<br><br>

## 📦 Dependencies

### Required :
  + Boost compiled with zlib. (http://boost.org)
### Optional :
  + GEOS library (http://trac.orgeo.org/geos/)
  + GDAL library (http://GDAL.org)
  + Mosek library (http://mosek.com)
  + OSI library throught the CBC solver (http://github.com/coin/Cbc)
  + Boost Python library
  + Rcpp and cran-R with at least Rtools40 (http://cran.r-project.org)

## 📖 Documentation

**FMT has a handwritten extensive documentation with exercises and examples**, along with a doxygen-generated documentation that are both available on the FMT documentation webpage.

<br><br>
<p align="center">
	<a href = "https://bureau-du-forestier-en-chef.github.io/FMTdocs/"><img src = "https://img.shields.io/badge/%F0%9F%93%9A-Take%20a%20look%20at%20the%20documentation%20!-green?style=flat-square" height="45" /></a>
</p>
<br><br>

**If you want to compile the doxygen documentation by yourself**, with a powershell prompt, go into the FMT folder and use:

 ```powershell
  cd Documentation
  doxygen FMTdoxygenconfig
 ```
To populate comments in R and Python source files (files starting with R and PY):

 ```powershell
  cd Documentation
  python commentsPythonandR.py
 ```

## 👉 Examples

 Here's a short example for reading a forest planning model and solve the type III Linear programming formulation.
 + Using Python
  ```python
  from FMT import Models
from FMT import Parser

modelparser = Parser.FMTmodelparser()
models = modelparser.readproject("pathtoprimaryfile",["scenariox"])#read scenario x from the primay file (.pri)
optimizationmodel=Models.FMTlpmodel(models[0],Models.FMTsolverinterface.CLP)#Construct a type III linear programming model from the FMTmodel

###Build the graph for 10 periods
for period in range(0,10):
        print(optimizationmodel.buildperiod())

#Now add objective function to the FMTlpmodel and all the constraints
constraints = optimizationmodel.getconstraints()
objective = constraints.pop(0)
for constraint in constraints:
    print(optimizationmodel.setconstraint(constraint))
print(optimizationmodel.setobjective(objective))
#Do a initial solve of the forest planning model
optimizationmodel.initialsolve()
 ```
 + Using R
 ```R
library(FMT)
modelparser<-new(FMTmodelparser)
models<-modelparser$readproject("pathtoprimaryfile",c("scenariox"),TRUE,TRUE,TRUE)#read scenario x from the primay file (.pri)
optimizationmodel<-new(FMTlpmodel,models[[1]],FMTsolverinterface$CLP)#Construct a type III linear programming model from the FMTmodel
emptyschedule<-new(FMTschedule)
#Build the graph for 10 periods
for (period in 1:10)
	{
	print(optimizationmodel$buildperiod(emptyschedule,FALSE)$str())
	}
#Now add objective function to the FMTlpmodel and all the constraints
allmodelconstraints<-optimizationmodel$getconstraints()
modelobjective<-allmodelconstraints[[1]]
modelconstraints<-allmodelconstraints[2:length(allmodelconstraints)]
for (constraint in modelconstraints)
	{
	print(optimizationmodel$setconstraint(constraint)$str())
	}
print(optimizationmodel$setobjective(modelobjective)$str())
#Do a initial solve of the forest planning model
optimizationmodel$initialsolve()
 ```
+ Using C++
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

## 🔑 License 

FMT is a [LiLiQ-R 1.1](https://github.com/gcyr/FMT/blob/master/LICENSES/EN/LILIQ-R11EN.pdf) licensed library.

[![License](http://img.shields.io/:license-liliqR11-blue.svg?style=flat-square)](https://forge.gouv.qc.ca/licence/liliq-v1-1/#r%C3%A9ciprocit%C3%A9-liliq-r)


## 📈 Coverage
GNU ![](https://img.shields.io/badge/coverage-70%25-orange)
MSVC ![](https://img.shields.io/badge/coverage-0%25-red)


## 💻 Build
GNU ![](https://img.shields.io/badge/build-pass-brightgreen)
MSVC ![](https://img.shields.io/badge/build-broken-red)
