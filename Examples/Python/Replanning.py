import sys
sys.path.append("../../")
sys.path.append("../../Release")
from FMT import Models
from FMT import Parser
from FMT import Core
from FMT import Version

def getstochasticactionsntransitions(basemodel,actionames):
    stochasticactions=[]
    stochastictransitions=[]
    for action,transition in zip(basemodel.getactions(),basemodel.gettransitions()):
        if (action.getname() in actionames):
            stochasticactions.append(action)
            stochastictransitions.append(transition)
    return stochasticactions,stochastictransitions

def doreplanning(solvedglobal,baselocal,basestochastic,seed,replanningrange):
    simulationmodel = Models.FMTnssmodel(basestochastic,seed)
    objectivevalues = []
    for replanningperiod in replanningrange:
        localmodel=Models.FMTlpmodel(baselocal,Models.FMTsolverinterface.CLP)
        simulationmodel.setarea(solvedglobal.getarea(replanningperiod))
        disturbed = simulationmodel.simulate(actionsproportions)
        potentialpaths = (disturbed + solvedglobal.getsolution(replanningperiod))
        potentialpaths.setperiod(1)
        localmodel.setarea(simulationmodel.getarea())
        localmodel.setareaperiod(0)
        localconstraints = localmodel.getconstraints()
        localmodel.setobjective(localconstraints[0])
        for constraint in localconstraints:
            localmodel.setconstraint(constraint)
        if localmodel.initialsolve():
            completelocalschedule = localmodel.getsolution(1) + disturbed
            completelocalschedule.setperiod(replanningperiod)
            solvedglobal.setsolution(replanningperiod, completelocalschedule)
            solvedglobal.eraseperiod(true)
            if solvedglobal.boundsolution(replanningperiod):
                solvedglobal.eraseperiod()
                solvedglobal.buildperiod()
                solvedglobal.setobjective(objective)
                for constraint in constraints:
                    solvedglobal.setconstraint(constraint)
                if not solvedglobal.resolve():
                    print("Infeasible after constraints added")
                    break
            else:
                print("Infeasible bound solve")
                break;
        else:
            print("Infeasible local model")
            break;
        objectivevalues.append(localmodel.getobjectivevalue())
    return objectivevalues




if __name__ == "__main__":
    if Version.FMTversion().hasfeature("OSI"):
        modelparser = Parser.FMTmodelparser()
        path = "../Models/Jesus_land/jesus_land.pri"
        scenarios = ["LP","LPwfire"]
        models = modelparser.readproject(path, scenarios)
        print(models)
        globalmodel = Models.FMTlpmodel(models[0], Models.FMTsolverinterface.CLP)
        stochasticactions,stochastictransitions=getstochasticactionsntransitions(models[1],["AFIRE"])
        stochasticmodel = Models.FMTmodel(models[1])
        stochasticmodel.setactions(stochasticactions)
        stochasticmodel.settransitions(stochastictransitions)
        globalactions = globalmodel.getactions()
        globaltransitions = globalmodel.gettransitions()
        globalactions+=stochasticactions
        globaltransitions+=stochastictransitions
        globalmodel.setactions(globalactions)
        globalmodel.settransitions(globaltransitions)
        for period in range(0,10):
            globalmodel.buildperiod()
        constraints = globalmodel.getconstraints()
        objective = constraints.pop(0)
        globalmodel.setobjective(objective)
        for constraint in constraints:
            globalmodel.setconstraint(constraint)
        actionsproportions = [0.001]
        globalmodel.initialsolve()
        iterationvalues = []
        for iteration in range(0,100):
            iterationvalues.append(doreplanning(globalmodel,models[1],stochasticmodel,iteration,range(1,11)))
        print(iterationvalues)
    else:
        print("FMT needs to be compiled with OSI")
