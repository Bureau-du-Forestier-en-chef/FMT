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
        path = "../Models/Jesus_land/jesus_land.pri"
        scenarios = ["LP"]
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
            spatialactions = []
            for action in simulationmodel.getactions():
                spatialaction=Spatial.FMTspatialaction(action)
                spatialaction.neighbors = ["coupetotale"]
                spatialaction.green_up=1
                spatialaction.adjacency=1
                spatialaction.minimal_size=1
                spatialaction.maximal_size=2
                spatialaction.neighbors_size=4
                spatialactions.append(spatialaction)
            simulationmodel.setspactions(spatialactions)
            areaparser = Parser.FMTareaparser()
            rasterslocation = "../Models/Jesus_land/rasters/"
            ageraster = os.path.join(rasterslocation,"AGE.tif")
            themesrasters = []
            themeid=1
            for theme in simulationmodel.getthemes():
                themesrasters.append(os.path.join(rasterslocation,"THEME"+str(themeid)+".tif"))
                themeid+=1
            initialforestmap=areaparser.readrasters(simulationmodel.getthemes(),themesrasters,ageraster,1,0.0001)
            simulationmodel.setinitialmapping(initialforestmap)
            for period in range(1,11):
                print(simulationmodel.simulate(lpmodel.getsolution(period),1,False))
    else:
        print("FMT needs to be compiled with OSI and GDAL")
        