import sys,os,time,csv
from FMT import Models
from FMT import Parser
from FMT import Core
from FMT import Heuristics


def getOAparameters(lpmodel,parameterslocation,themeid):
        themes=lpmodel.getthemes()
        opareas = []
        with open(parameterslocation,mode='rb') as parameters_file:
                csv_reader = csv.DictReader(parameters_file,delimiter=';')
                for row in csv_reader:
                        mask = ["?" for theme in themes]
                        mask[themeid] = row["OA"]
                        opareas.append(Heuristics.FMToperatingarea(Core.FMTmask(mask,themes),int(row["OPT"]),int(row["RET"]),
                                                    int(row["REP"]),int(row["GUP"]),float(row["OPR"]),float(row["NPE"])))
        return opareas                       


if __name__ == "__main__":
        parameterslocation = "T:/Donnees/Courant/Provincial/COS_sapiniere/Outil_optimisation_BFEC/BFECopt1.4.1.1/parameters2651.csv"
        primarylocation = "T:/Donnees/Archives/CPF_2013-2018/Sources/Territoires/02651/Resultats/Determination/projet_ws/PC_4514_U02651_3FF_v06.pri"
        modelparser = Parser.FMTmodelparser()
        models = modelparser.readproject(primarylocation,["scenario_3"])
        optimizationmodel=Models.FMTlpmodel(models[0],Models.FMTsolverinterface.MOSEK)
        operatingareas = getOAparameters(optimizationmodel,parameterslocation,13)
        areaparser = Parser.FMTareaparser()
        shapelocation = "T:/Donnees/Archives/CPF_2013-2018/Sources/Territoires/02651/Resultats/Determination/projet_ws/Carte/PC_4514.shp"
        themes = optimizationmodel.getthemes()
        opareawithneighbors = areaparser.getneighbors(operatingareas,themes,shapelocation,"AGE","SUPERFICIE")
##        for oparea in opareawithneighbors:
##                print([str(mask) for mask in oparea.getneighbors()])
        for period in range(0,5):
                print(optimizationmodel.buildperiod())
        allconstraints = optimizationmodel.getconstraints()
        optimizationmodel.setobjective(allconstraints.pop(0))
        for constraint in allconstraints:
                optimizationmodel.setconstraint(constraint)
        optimizationmodel.initialsolve()
        nodeofoutput = None
        for output in optimizationmodel.getoutputs():
                if "OSUPREALREGAFINCOS" == output.name:
                      nodeofoutput = output.getnodes()[0]
                      print(nodeofoutput)
                      break;
        opareaheuristics = optimizationmodel.getoperatingareaheuristics(opareawithneighbors,nodeofoutput,1,False)
        opareaheuristics[0].initialsolve()
        for handler in opareaheuristics[0].getsolution("youvert"):
            print(handler)
        
