import sys
sys.path.append("../../")
sys.path.append("../../../x64/Release")
sys.path.append("../../Release")
from FMT import Models
from FMT import Parser
from FMT import Core
from FMT import Version
                     


if __name__ == "__main__":
        if Version.FMTversion().hasfeature("OSI"):
            primarylocation = "../Models/TWD_land/TWD_land.pri"
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
            themes = optimizationmodel.getthemes()
            masktargets=[Core.FMTmask("? PEUPLEMENT2 ?",themes),Core.FMTmask("? PEUPLEMENT3 ?",themes)]#
            proportions=[0.01,#add 1% to peuplement2
                         -0.10]#Remove 10% to Peuplement3
            #Validate that globalmasks does not intersect
            for development in optimizationmodel.getarea():
                count= 0
                for target in masktargets:
                    if development.mask.issubsetof(target):
                        count+=1
                if (count>1):
                    print("Intersecting globalmask!")
                    exit(-1)
            outputstocheck = []
            for output in optimizationmodel.getoutputs():
                    if output.getname() in ["OVOLREC"]:
                          outputstocheck.append(output)
            print(optimizationmodel.getareavariabilities(outputstocheck,masktargets,proportions))
        else:
            print("FMT needs to be compiled with OSI")
