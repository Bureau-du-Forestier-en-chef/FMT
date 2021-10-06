[🇼🇫 Click here for a french version](README.fr.md)

# 📑 Description

FMT (**F**orest **M**anagement **T**ool) is an open source Forest Management library.

- It is a C++ library made to interpret forest planning models based on Woodstock files formulation. 
- It can be used through Python, Cran-R or directly from C++. 
- FMT's generic approach to forest planning gives the opportunity to the user to manipulate forest planning models and generate forest planning solutions. 
- The objective behind this project is to bring all forest planning models types onto the same generic library to help the community to develop new approaches to forest planning problems or improve the one that have been there for years.


# ✨ Features

## Spatially referenced forest planning

FMT allows the user to generate and solve spatially referenced type III forest planning model problems. The tool can be used to do sensitivity analyses over multiple parameters of the forest planning model. It can also be used for analysing impacts of stochastic events such as fire or spruce budworm using Replanning.

## Spatially explicit forest planning

FMT gives a framework to construct heuristics to solve forest planning model. FMT is the perfect tool to solve user specific problems related to forest planning like operating area scheduling. It can also be used to implement simple simulation process. The library offers some tools to spatialize spatially referenced solutions or to generate spatially explicit solutions.

FMT is extremely usefull when it comes to implement metaheuristics such has Simulated Annealing, Genetic or Tabu Search into a forest planning context.

If your main inputs are Woodstock files FMT is probably the way to go to develop new approaches for optimization or simulation of harvest schedules or even stochastic events.

# 💽 Installation

The easiest way to use FMT is to use Cmake >= 3.15 to generate a shared library for R or Python or directly from C++. The Cmake script of this repository supports generating FMT on Windows using VS2017/2019 or MINGW-64 on MSYS2 or CYGWIN.

To create a Python library from FMT on Windows, follow these instructions :

1. Download and install [Visual Studio Community](https://visualstudio.microsoft.com/vs/community/)
2. Download and install [Mosek](https://www.mosek.com/)
3. Create a `FMT-Installation` folder in your computer. Make sure that there is no spaces or accents in the path leading to the folder.
4. Open a command prompt, and use the `cd` command to navigate to the `FMT-Installation` folder.
5. Use `git clone https://github.com/microsoft/vcpkg.git` in the command prompt to download [vcpkg](https://github.com/microsoft/vcpkg). If you don't have git installed on your computer, install it from [here](https://git-scm.com/).
6. Use `cd ./vcpkg` to navigate into the vcpkg folder.
7. Use the following command to install the dependencies necessary to compile FMT (⚠️ **This will take time, and will use some space on your computer**): `vcpkg install boost:x64-windows boost-dll:x64-windows boost-icl:x64-windows boost-python:x64-windows gdal:x64-windows geos:x64-windows pthreads:x64-windows clp:x64-windows`
8. Use `cd ..` to go back to the `FMT-Installation` folder.
9. Clone this repository using `git clone https://github.com/gcyr/FMT.git`
10. Use `cd ./FMT` to navigate into the FMT folder.
11. Use the following commands to compile FMT (⚠️ **This will take time**):

```bash
cmake CMakeLists.txt -G "Visual Studio 16 2019" -DVCPKG_TARGET_TRIPLET=x64-windows -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake -DBOOST_DIR="../vcpkg/installed/x64-windows/" -DCMAKE_BUILD_TYPE=Release -DGDAL_DIR="../vcpkg/installed/x64-windows/" -DGEOS_DIR="../vcpkg/installed/x64-windows/" -DOSI_DIR="../vcpkg/installed/x64-windows/" -DPYTHON_DIR="../vcpkg/packages/python3_x64-windows/" -DMOSEK_DIR="C:/PROGRA~1/Mosek/"`
cmake --build . --config Release
cmake --install . --config Release
```

12. Import FMT into Python by using:

```python
import sys
sys.path.append(r"<Path to FMT folder>/")
sys.path.append(r"<Path to FMT folder>/Release/")
from FMT import Models
from FMT import Parser
from FMT import Core
from FMT import Version
```

# 📦 Dependencies

## Required :
  + Boost compiled with zlib. (http://boost.org)
## Optional :
  + GEOS library (http://trac.orgeo.org/geos/)
  + GDAL library (http://GDAL.org)
  + Mosek library (http://mosek.com)
  + OSI library throught the CBC solver (http://github.com/coin/Cbc)
  + Boost Python library
  + Rcpp and cran-R with at least Rtools40 (http://cran.r-project.org)

# 📖 Documentation

The documentation of FMT must be compiled with Doxygen (http://www.doxygen.nl).

With a powershell prompt, go into the FMT folder and use:

 ``` powershell
  cd Documentation
  doxygen FMTdoxygenconfig
 ```
To populate comments in R and Python source files (files starting with R and PY):

 ```powershell
  cd Documentation
  python commentsPythonandR.py
 ```

# 👉 Examples

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
constraints.erase(constraints.begin());
for (const Core::FMTconstraint& constraint : constraints)
	{
	optimizationmodel.setconstraint(constraint);
	}
optimizationmodel.setobjective(objective);
optimizationmodel.initialsolve();
```

# 🔑 License 

FMT is a [LiLiQ-R 1.1](https://github.com/gcyr/FMT/blob/master/LICENSES/EN/LILIQ-R11EN.pdf) licensed library.

[![License](http://img.shields.io/:license-liliqR11-blue.svg?style=flat-square)](https://forge.gouv.qc.ca/licence/liliq-v1-1/#r%C3%A9ciprocit%C3%A9-liliq-r)
