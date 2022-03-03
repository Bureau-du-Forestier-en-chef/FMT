---
title: Exercise 1
linktitle: Exercise 1
type: book
date: "2019-05-05T00:00:00+01:00"
# Prev/next pager order (if `docs_section_pager` enabled in `params.toml`)
weight: 4
---

In this exercise, you will try to use FMT to read a scenario, solve it, read one of its outputs, and export the solution.

In order to do the exercise, **you will need to have FMT installed on your computer, and to have downloaded the example files from the GitHub repository of FMT**.

{{< cta cta_text="💾 Downloading and installing FMT" cta_link="../../download_install" cta_new_tab="true" >}}

{{< cta cta_text="💾 Downloading the example files of FMT" cta_link="https://downgit.github.io/#/home?url=https://github.com/gcyr/FMT/tree/master/Examples" cta_new_tab="true" >}}

## Goals of the exercise

Use FMT to :

1. Read the Woodstock model `TWD_land` in the example files by selecting its scenario named `LP`.
	- Remember to select the `CLP` solver while parsing the model.
2. Solve the model by
	- Constructing its full graph for 10 periods
	- Setting the objective and constraints of the model
	- Using the `FMTmodel.initialsolve()` function to solve it.
3. Read the values of the output `OSUPREC` of the model by
	- Getting a list of the outputs of the model with the function `FMTmodel.getouputs()`
	- Doing a `for` loop to select the right output with the `FMToutput.getname()` function
	- Printing the output values using the function `FMTmodel.getvariabilities(FMToutput)`
4. Write the optimized schedule of the model in a `.txt` file by
	- Creating a new `FMTscheduleparser` object
	- Getting the solution of the model at each period with a list using `LPModel.getsolution(period)`
	- Transforming the list into a vector
	- Writing the vector into a `.txt` file with `FMTscheduleparser.write()`

**You can use the information from the previous pages of documentation to help you.**

When you are done, or if you are stuck, **you can display the correction of this exercise for Python by clicking on the "Reveal correction" below**.

## Correction

{{< spoiler text="Reveal correction" >}}

```python
#%% LOADING FMT

# Here, we load FMT directly, as if it were installed
# via pip.

from FMT import Models
from FMT import Parser
from FMT import Version

#%% LOADING THE WOODSTOCK MODEL

# Creation of a parser to read the model
modelParser = Parser.FMTmodelparser()

# Defining the path leading to the Woodstock model .pri file.
# WARNING : try to have a path without spaces inside it.
pathToWoodstockModel = "<Path to example folder>/Examples/Models/TWD_land/TWD_land.pri"

# Defining the scenario to be read, which contains a set of constraints and objectives.
scenariosToSelect = ["LP"]

# Reading the model using the parser and the scenario name. We can read multiple at once,
# but here, we'll only use one.
listOfModelsParsed = modelParser.readproject(pathToWoodstockModel, scenariosToSelect)

# Loading the model from the list of models given by the parser. We only take the first
# one, which is the only one that we want. We also indicate the solver interface that we
# will use to solve the linear programming model; here, we will use CLP, the free and
# open-source solver.
LPModel = Models.FMTlpmodel(listOfModelsParsed[0], Models.FMTsolverinterface.CLP)

#%% SOLVING THE MODEL

# We build the full graph of the model for 10 periods.
for period in range(1, 11):
    print(LPModel.buildperiod())

# The objective is simply the first item of the
# constraints list, that we take away from the list with pop().
constraintsOfModel = LPModel.getconstraints()
objectiveOfModel = constraintsOfModel.pop(0)

# We set the contraints and objectives of the model.
for constraint in constraintsOfModel:
    print(LPModel.setconstraint(constraint))
print(LPModel.setobjective(objectiveOfModel))

# We solve the model.
LPModel.initialsolve()

#%% READING THE OSUPREC OUTPUT

# We can read OSUPREC before the model is solved, but the values won't be interesting
# to us.
outputWanted = "OSUPREC"

# We get the outputs of the model
outputsOfModel = LPModel.getoutputs()
# We loop around the list of output to select the one with the
# right name
for output in outputsOfModel:
    print(output.getname())
    if output.getname() == outputWanted:
    	# We print the measures of "OSUPREC" in the python prompt
        print(LPModel.getvariabilities([output]))
        # We save the values of OSUPREC to format them better
        osuprecValues = LPModel.getvariabilities([output])
       
# Here is a small code to display the values better,
# adapting to the fact that the getvariabilities() function
# returns the minimum, mean and maximum value of the output.
print("\n\n ###############################")
print("Values of " + outputWanted + " :\n")
print("PERIOD\t\tMINIMUM\t\tMEAN\t\tMAXIMUM")
for period in range(1, 11):
    print(str(period) + "\t\t\t" + str(round(osuprecValues["L"+outputWanted][period-1], 2))
          + "\t\t" + str(round(osuprecValues["M"+outputWanted][period-1], 2))
          + "\t\t" + str(round(osuprecValues["U"+outputWanted][period-1], 2)))

#%% WRITING THE SCHEDULE

# We create a new scheduleparser object
scheduleParser = Parser.FMTscheduleparser()

# We get the solution of the model
modelSolution = list()
for period in range(1, 11):
    modelSolution.append(LPModel.getsolution(period))
    
# We write everything into a .txt file after putting everything as a vector.
modelSolutionAsVector = [[i] for i in modelSolution]
scheduleParser.write(modelSolution, r"D:/TempCode/FMT/FMT/Examples/programme.txt")
```

{{< /spoiler >}}