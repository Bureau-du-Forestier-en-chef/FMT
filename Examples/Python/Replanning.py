import sys
sys.path.append("../../")
sys.path.append("../../Release")
sys.path.append("../../../x64/Release")
from FMT import Models
from FMT import Parser
from FMT import Core
from FMT import Version
from FMT import Graph


if __name__ == "__main__":
    if Version.FMTversion().hasfeature("OSI"):
        modelparser = Parser.FMTmodelparser()
        modelparser.setquietexceptionhandler()
        modelparser.setquietlogger()
        path = "../Models/TWD_land/TWD_land.pri"
        scenarios = ["Globalreplanning","Localreplanning","Globalfire"]
        outputnames = ["OVOLREC"]
        models = modelparser.readproject(path, scenarios)
        maxperiod = 10
        replanningperiods = min(10,maxperiod)
        iterations = 50
        globalmodel = Models.FMTlpmodel(models[0], Models.FMTsolverinterface.CLP)
        for period in range(0,maxperiod):
            globalmodel.buildperiod()
        globalconstraints = globalmodel.getconstraints()
        globalobjective = globalconstraints[0]
        globalconstraints.pop(0)
        alloutputs = []
        for output in globalmodel.getoutputs():
            for name in outputnames:
                if (output.getname() == name):
                    alloutputs.append(output)
                    break
        for constraint in globalconstraints:
            globalmodel.setconstraint(constraint)
        globalmodel.setobjective(globalobjective)
        if globalmodel.initialsolve():
            print('Ba:', end = '')
            for period in range(1,maxperiod+1):
                for output in alloutputs:
                    print(int(globalmodel.getoutput(output,period,Graph.FMToutputlevel.totalonly)["Total"]),end=' ')
            print()
            for iteration in range(1,iterations+1):
               localglobal = Models.FMTlpmodel(globalmodel)
               print( "It ", iteration, end=' ')
               localsimulation = Models.FMTnssmodel(models[2], iteration)
               localconstraints = models[1].getconstraints()
               localobjective = localconstraints[0]
               localconstraints.pop(0)
               for replanningperiod in range(1,replanningperiods+1):
                   basearea = localglobal.getarea(replanningperiod)
                   localsimulation.setarea(basearea)
                   disturbed = localsimulation.simulate(False,True)
                   
                   localmodel = Models.FMTlpmodel(models[1], Models.FMTsolverinterface.CLP)
                   simulatedarea = localsimulation.getarea()
                   localmodel.setarea(basearea)
                   newpotential = localmodel.getpotentialschedule(basearea, simulatedarea)
                   localmodel.setarea(simulatedarea)
                   localmodel.setareaperiod(replanningperiod-1)
                   potentialpaths = localglobal.getsolution(replanningperiod,True) + newpotential
                   localmodel.buildperiod(potentialpaths,True)
                   globalysetconstraints = localglobal.getlocalconstraints(localconstraints,replanningperiod)
                   globalysetconstraints.insert(0,localobjective)
                   localmodel.setconstraints(globalysetconstraints)
                   for constraint in globalysetconstraints:
                       localmodel.setconstraint(constraint)
                   localmodel.setobjective(localobjective)
                   if localmodel.initialsolve():
                       for output in alloutputs:
                           print(int(localmodel.getoutput(output,replanningperiod,Graph.FMToutputlevel.totalonly)["Total"]), end=' ')
                       completelocalschedule = localmodel.getsolution(replanningperiod,True)+ disturbed
                       schparser=Parser.FMTscheduleparser() 
                       localglobal.setsolution(replanningperiod, completelocalschedule,0.001)
                       localglobal.eraseperiod(True)
                       if localglobal.boundsolution(replanningperiod,0.00001):
                           localglobal.eraseperiod()
                           #localglobal.buildperiod()
                          
                           for constraint in globalconstraints:
                               localglobal.setconstraint(constraint)
                           localglobal.setobjective(globalobjective)
                           if not localglobal.resolve():
                              print("Global model infeasible at replanning-period ", replanningperiod)
                       else:
                           print("Global model infeasible after bounding at replanning-period ",replanningperiod)
                   else:
                        print("Local model infeasible at replanning-period ",replanningperiod)
               print()
        else:
            print("Base global model infeasible")
    else:
        print("FMT needs to be compiled with OSI")
