import sys
sys.path.append("../../")
sys.path.append("../../Release")
from FMT import Models
from FMT import Parser
from FMT import Core
from FMT import Heuristics
from FMT import Version                  


if __name__ == "__main__":
        if Version.FMTversion().hasfeature("OSI"):
            primarylocation = "../Models/Jesus_land/jesus_land.pri"
            modelparser = Parser.FMTmodelparser()
            models = modelparser.readproject(primarylocation,["LP"])
            optimizationmodel=Models.FMTlpmodel(models[0],Models.FMTsolverinterface.CLP)
            themes = optimizationmodel.getthemes()
            opareas = []
            for attribute in themes[2].getattributes("?"):
                mask = ["?" for theme in themes]
                mask[2] = attribute
                opareas.append(Heuristics.FMToperatingareascheme(Heuristics.FMToperatingarea(Core.FMTmask(mask,themes),0),2,1,4,0,1))
            for period in range(0,10):
                    print(optimizationmodel.buildperiod())
            allconstraints = optimizationmodel.getconstraints()
            optimizationmodel.setobjective(allconstraints.pop(0))
            for constraint in allconstraints:
                    optimizationmodel.setconstraint(constraint)
            optimizationmodel.initialsolve()
            nodeofoutput = None
            for output in optimizationmodel.getoutputs():
                    if "OSUPREC" == output.getname():
                          nodeofoutput = output.getnodes(optimizationmodel.getarea(),optimizationmodel.getactions(),optimizationmodel.getyields())[0]
                          break;
            opareaheuristics = optimizationmodel.getoperatingareaschedulerheuristics(opareas,nodeofoutput,1,False)
            opareaheuristics[0].initialsolve()
            for handler in opareaheuristics[0].getsolution("youvert"):
                print(handler)
        else:
            print("FMT needs to be compiled with OSI")
        
