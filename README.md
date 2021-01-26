Read this using other languages: [English](README.md), [Français](README.fr.md)

## FMT is an open source Forest Management library

FMT is a C++ library made to interpret forest planning models based on Woodstock files formulation. FMT can be used through Python, Cran-R or directly from C++. FMT's generic approach to forest planning gives the opportunity to the user to manipulate forest planning models and generate forest planning solutions. The objective behind this project is to bring all forest planning models types onto the same generic library to help the community to develop new approaches to forest planning problems or improve the one that have been there for years.

## License 

FMT is a LiLiQ-R 1.1 licensed library [LICENSES\EN](LILIQ-R11EN.txt) for more information.

## Why FMT?

### Spatially referenced forest planning

FMT gives the opportunity to the user to generate and solve spatially referenced type III forest planning model problems. The tool can be used to do sensitivity analyses over multiple parameters of the forest planning model. It can also be used for analysing impacts of stochastic events such as fire or spruce budworm using Replanning.


### Spatially explicit forest planning

FMT gives a framework to construct heuristics to solve forest planning model. FMT is the perfect tool to solve user specific problems related to forest planning like operating area scheduling. It can also be used to implement simple simulation process. The library offers some tools to spatialize spatially referenced solutions or to generate spatially explicit solutions.
FMT is extremely usefull when it comes to implement metaheuristics such has Simulated Annealing, Genetic or Tabu Search into a forest planning context.

If your main inputs are Woodstock files FMT is probably the way to go to develop new approaches for optimization or simulation
of harvest schedules or even stochastic events.


## Dependencies

### Required :
  + Boost compiled with zlib. (http://boost.org)
### Optional :
  + GEOS library (http://trac.orgeo.org/geos/)
  + GDAL library (http://GDAL.org)
  + Mosek library (http://mosek.com)
  + OSI library throught the CBC solver (http://github.com/coin/Cbc)
  + Boost Python library
  + Rcpp and cran-R with at least Rtools40 (http://cran.r-project.org)
  
## How to use FMT?

The easiest way to use FMT is to use the cmake >= 3.15 and generate a shared library for R or Python or directly from C++ (see examples: FMTSimpleplanning.exe and FMTSpatialyexplicitsimulation.exe). The cmake script supports generating FMT on Windows using VS2017 or MINGW-64 on MSYS2 or CYGWIN.

+ Generating FMT as a Python library on Visual studio 2017:
 
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

+ Generating FMT as a R library on MINGW-64 with MSYS2 (Rtools40)
 ```bash
 #Here is a example for generating FMT module .dll for R
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

+ For generating Doxygen documentation (http://www.doxygen.nl) into the documentation folder:

 ``` powershell
  cd Documentation
  doxygen FMTdoxygenconfig
 ```
+ To populate comments into R and Python source files (files starting with R and PY):

 ```powershell
  cd Documentation
  python commentsPythonandR.py
 ```
 ## Examples
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
print(optimizationmodel.setobjective(objective))
for constraint in constraints:
    print(optimizationmodel.setconstraint(constraint))

#Do a initial solve of the forest planning model
optimizationmodel.initialsolve()
 ```
 + Using R
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
+ Using C++
``` C++
#include <vector>
#include "FMTlpmodel.h"
#include "FMTmodelparser.h"

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
optimizationmodel.setobjective(objective);
constraints.erase(constraints.begin());
for (const Core::FMTconstraint& constraint : constraints)
	{
	optimizationmodel.setconstraint(constraint);
	}
optimizationmodel.initialsolve();
```
[![License](http://img.shields.io/:license-liliqR11-blue.svg?style=flat-square)](https://forge.gouv.qc.ca/licence/liliq-v1-1/#r%C3%A9ciprocit%C3%A9-liliq-r)
