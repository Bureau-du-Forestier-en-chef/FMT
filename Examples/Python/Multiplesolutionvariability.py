import sys
sys.path.append("../../")
sys.path.append("../../Release")
from FMT import Models
from FMT import Parser
from FMT import Core
from FMT import Version
                     


if __name__ == "__main__":
    if Version.FMTversion().hasfeature("OSI"):
        primarylocation = "../Models/Jesus_land/jesus_land.pri"
        modelparser = Parser.FMTmodelparser()
        models = modelparser.readproject(primarylocation,["LP"])
        optimizationmodel=Models.FMTlpmodel(models[0],Models.FMTsolverinterface.CLP)
        areaparser = Parser.FMTareaparser()
        for period in range(0,10):
                print(optimizationmodel.buildperiod())
        allconstraints = optimizationmodel.getconstraints()
        print(optimizationmodel.setobjective(allconstraints.pop(0)))
        for constraint in allconstraints:
                optimizationmodel.setconstraint(constraint)
        optimizationmodel.initialsolve()
        outputstocheck = []
        for output in optimizationmodel.getoutputs():
                if output.getname() in ["P2AREA"]:
                      outputstocheck.append(output)
        print(optimizationmodel.getvariabilities(outputstocheck))
    else:
        print("FMT needs to be compiled with OSI")