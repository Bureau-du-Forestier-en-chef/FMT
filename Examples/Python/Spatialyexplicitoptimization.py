import sys,os
sys.path.append("../../")
sys.path.append("../../../x64/Release")
sys.path.append("../../Release")
from FMT import Models
from FMT import Parser
from FMT import Spatial
from FMT import Core
from FMT import Version


if __name__ == "__main__":
    if Version.FMTversion().hasfeature("GDAL"):
        modelparser = Parser.FMTmodelparser()
        path = "../Models/TWD_land/TWD_land.pri"
        scenarios = ["Spatial"]
        models = modelparser.readproject(path, scenarios)
        samodel = Models.FMTsamodel(models[0])
        singletransitions = []
        for transition in samodel.gettransitions():
            singletransitions.append(transition.single())
        samodel.settransitions(singletransitions)
        #for action in samodel.getactions():
        #    print(action.getname())
        areaparser = Parser.FMTareaparser()
        rasterslocation = "../Models/TWD_land/rasters/"
        ageraster = os.path.join(rasterslocation,"AGE.tif")
        themesrasters = []
        themeid=1
        for theme in samodel.getthemes():
            themesrasters.append(os.path.join(rasterslocation,"THEME"+str(themeid)+".tif"))
            themeid+=1
        initialforestmap=areaparser.readrasters(samodel.getthemes(),themesrasters,ageraster,1,0.0001)
        samodel.setinitialmapping(initialforestmap)
        for period in range(0,10):
            samodel.buildperiod()
        if samodel.initialsolve():
            print("Optimal")
    else:
        print("FMT needs to be compiled with GDAL")
        
