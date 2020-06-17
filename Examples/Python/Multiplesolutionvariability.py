import sys,os,time,csv
from FMT import Models
from FMT import Parser
from FMT import Core
                     


if __name__ == "__main__":
        primarylocation = "D:/MrnMicro/Mod_Ws_PrototypeRP1/PC_8285_U08551_Vg1_2023_vPF03.pri"
        modelparser = Parser.FMTmodelparser()
        models = modelparser.readproject(primarylocation,["Sc1a_PrototypeRP1_avsp_BFECopt"])
        optimizationmodel=Models.FMTlpmodel(models[0],Models.FMTsolverinterface.MOSEK)
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
                if output.getname() in ["OSUPREALREGAEDU","OSUPREALREGAREG"]:
                      outputstocheck.append(output)
        print(optimizationmodel.getvariabilities(outputstocheck))
