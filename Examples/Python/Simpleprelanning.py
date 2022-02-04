from FMT import Models
from FMT import Parser
from FMT import Core
from FMT import Version
from FMT import Parallel
import os


if __name__ == "__main__":
    if Version.FMTversion().hasfeature("OSI"):
        modelparser = Parser.FMTmodelparser()
        path = "../Models/TWD_land/TWD_land.pri"
        scenarios = ["Globalreplanning","Globalfire","Localreplanning"]
        models = modelparser.readproject(path, scenarios)
        globalmodel = Models.FMTlpmodel(models[0], Models.FMTsolverinterface.CLP)
        globalmodel.setparameter(Models.FMTintmodelparameters.LENGTH, 10)
        globalmodel.setparameter(Models.FMTintmodelparameters.NUMBER_OF_THREADS,1)
        stochastic = Models.FMTnssmodel(models[1],0)
        stochastic.setparameter(Models.FMTintmodelparameters.LENGTH, 1)
        localmodel = Models.FMTlpmodel(models[2], Models.FMTsolverinterface.CLP)
        localmodel.setparameter(Models.FMTintmodelparameters.LENGTH, 1)
        localmodel.setparameter(Models.FMTintmodelparameters.NUMBER_OF_THREADS,1)
        selectedoutputs=[]
        for output in globalmodel.getoutputs():
            if output.getname() in ["OVOLREC","VOLINVENT","BURNEDAREA"]:
                selectedoutputs.append(output)
        if not os.path.isdir("tests"):
            os.mkdir("tests")
        outputlocation = "tests/replanning"
        layersoptions=["SEPARATOR=SEMICOLON"]
        replanningtask=Parallel.FMTreplanningtask(globalmodel,stochastic, localmodel, selectedoutputs, outputlocation, "CSV", layersoptions,20,10,0,0.5, Core.FMToutputlevel.totalonly)
        handler = Parallel.FMTtaskhandler(replanningtask,2)
        handler.setquietlogger()
        handler.conccurentrun()
    else:
        print("FMT needs to be compiled with OSI")
