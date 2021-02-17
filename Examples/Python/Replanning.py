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
        models = modelparser.readproject(path, scenarios)
        maxperiod = 10
        replanningperiods = 10
        globalmodel = Models.FMTlpmodel(models[0], Models.FMTsolverinterface.CLP)
        for period in range(0,maxperiod+replanningperiods):
            globalmodel.buildperiod()
        globalconstraints = globalmodel.getconstraints()
        globalobjective = globalconstraints[0]
        globalmodel.setobjective(globalobjective)
        globalconstraints.pop(0)
        for output in globalmodel.getoutputs():
            if (output.getname() == "OVOLREC"):
                baseoutput = output
        for constraint in globalconstraints:
            globalmodel.setconstraint(constraint)
        if globalmodel.initialsolve():
            print('Ba:', end = '')
            for period in range(1,maxperiod+1):
                print(int(globalmodel.getoutput(baseoutput, period,Graph.FMToutputlevel.totalonly)["Total"]),end=' ')
            print()
            for iteration in range(1,replanningperiods+1):
               localglobal = Models.FMTlpmodel(globalmodel)
               print( "It ", iteration, end=' ')
               localsimulation = Models.FMTnssmodel(models[2], iteration)
               localconstraints = models[1].getconstraints()
               localobjective = localconstraints[0]
               localconstraints.pop(0)
               for replanningperiod in range(1,maxperiod+1):
                   basearea = localglobal.getarea(replanningperiod)
                   localsimulation.setarea(localglobal.getarea(replanningperiod))
                   disturbed = localsimulation.simulate(False,True)
                   potentialpaths = localglobal.getsolution(replanningperiod,True)
                   localmodel = Models.FMTlpmodel(models[1], Models.FMTsolverinterface.CLP)
                   localmodel.setarea(localsimulation.getarea())
                   localmodel.setareaperiod(0)
                   potentialpaths.setperiod(1)
                   localmodel.buildperiod()
                   localmodel.setobjective(localobjective)
                   globalysetconstraints = localglobal.getlocalconstraints(localconstraints,1)
                   for constraint in globalysetconstraints:
                       localmodel.setconstraint(constraint)
                   if localmodel.initialsolve():
                       print(int(localmodel.getoutput(baseoutput,1,Graph.FMToutputlevel.totalonly)["Total"]), end=' ')
                       completelocalschedule = localmodel.getsolution(1,True)+ disturbed
                       completelocalschedule.setperiod(replanningperiod)
                       schparser=Parser.FMTscheduleparser() 
                       localglobal.setsolution(replanningperiod, completelocalschedule,0.001)
                       localglobal.eraseperiod(True)
                       if localglobal.boundsolution(replanningperiod,0.00001):
                           localglobal.eraseperiod()
                           localglobal.buildperiod()
                           localglobal.setobjective(globalobjective)
                           for constraint in globalconstraints:
                               localglobal.setconstraint(constraint)
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
