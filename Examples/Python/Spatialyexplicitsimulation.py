import sys,os
sys.path.append("../../")
sys.path.append("../../Release")
from FMT import Models
from FMT import Parser
from FMT import Spatial
from FMT import Core
from FMT import Version


if __name__ == "__main__":
    if Version.FMTversion().hasfeature("OSI") and Version.FMTversion().hasfeature("GDAL"):
        modelparser = Parser.FMTmodelparser()
        path = "../Models/TWD_land/TWD_land.pri"
        scenarios = ["Spatial"]
        models = modelparser.readproject(path, scenarios)
        lpmodel = Models.FMTlpmodel(models[0], Models.FMTsolverinterface.CLP)
        for period in range(0,10):
            lpmodel.buildperiod()
        constraints = lpmodel.getconstraints()
        objective = constraints.pop(0)
        lpmodel.setobjective(objective)
        for constraint in constraints:
            lpmodel.setconstraint(constraint)
        if lpmodel.initialsolve():
            simulationmodel = Models.FMTsesmodel(lpmodel)
            singletransitions = []
            for transition in simulationmodel.gettransitions():
                singletransitions.append(transition.single())
            simulationmodel.settransitions(singletransitions)
            areaparser = Parser.FMTareaparser()
            rasterslocation = "../Models/TWD_land/rasters/"
            ageraster = os.path.join(rasterslocation,"AGE.tif")
            themesrasters = []
            themeid=1
            for theme in simulationmodel.getthemes():
                themesrasters.append(os.path.join(rasterslocation,"THEME"+str(themeid)+".tif"))
                themeid+=1
            initialforestmap=areaparser.readrasters(simulationmodel.getthemes(),themesrasters,ageraster,1,0.0001)
            simulationmodel.setinitialmapping(initialforestmap)
            #Stop aftet 10 search and no decrease into infeasibility and objective
            for period in range(1,11):
                print(simulationmodel.greedyreferencebuild(lpmodel.getsolution(period),10))
    else:
        print("FMT needs to be compiled with OSI and GDAL")
        
