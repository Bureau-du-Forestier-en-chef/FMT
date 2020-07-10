import sys,os,time,csv
from FMT import Models
from FMT import Parser
from FMT import Core

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
        localmodel=Models.FMTlpmodel(baselocal,Models.FMTsolverinterface.MOSEK)
        simulationmodel.setarea(optmodel.getarea(replanningperiod))
        disturbed = simulationmodel.simulate(actionsproportions)
        potentialpaths = (disturbed + optmodel.getsolution(replanningperiod))
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
            optmodel.setsolution(replanningperiod, completelocalschedule)
            optmodel.eraseperiod(true)
            if optmodel.boundsolution(replanningperiod):
                optmodel.eraseperiod()
                optmodel.buildperiod()
                optmodel.setobjective(objective)
                for constraint in constraints:
                    optmodel.setconstraint(constraint)
                if not optmodel.resolve():
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
    modelparser = Parser.FMTmodelparser()
    path = "D:/MrnMicro/localQCmodel/PC_PROV.pri";
    scenarios = ["04_psa","04_fire"]
    models = mparser.readproject(path, scenarios)
    globalmodel = Models.FMTlpmodel(models[0], Models.FMTsolverinterface.MOSEK)
    stochasticactions,stochastictransitions=getstochasticactionsntransitions(models[1],["AFIRE","ASBW"])
    stochasticmodel = Models.FMTmodel(models[1])
    stochasticmodel.setactions(stochasticactions)
    stochasticmodel.settransitions(stochastictransitions)
    simulationmodel.setactions(stochasticactions)
    simulationmodel.settransitions(stochastictransitions)
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
    actionsproportions = [0.001,0]
    globalmodel.initialsolve()
    iterationvalues = []
    for iteration in range(0,100):
        iterationvalues.append(doreplanning(globalmodel,models[1],stochasticmodel,iteration,range(1,11)))
    print(iterationvalues)
