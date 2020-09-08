import sys
sys.path.append("../../")
sys.path.append("../../Release")
from FMT import Models
from FMT import Parser
from FMT import Core
from FMT import Version


if __name__ == "__main__":
    if Version.FMTversion().hasfeature("OSI"):
        modelparser = Parser.FMTmodelparser()
        path = "../Models/TWD_land/TWD_land.pri"
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
        print("Global optimality? " + str(lpmodel.initialsolve()))
    else:
        print("FMT needs to be compiled with OSI")
