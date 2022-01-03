from FMT import Models
from FMT import Parser
from FMT import Core
from FMT import Heuristics
from FMT import Version                  


if __name__ == "__main__":
        if Version.FMTversion().hasfeature("OSI") and Version.FMTversion().hasfeature("GDAL"):
            primarylocation = "../Models/TWD_land/TWD_land.pri"
            maplocation = "../Models/TWD_land/Carte/TWD_land.shp"
            modelparser = Parser.FMTmodelparser()
            models = modelparser.readproject(primarylocation,["LP"])
            optimizationmodel=Models.FMTlpmodel(models[0],Models.FMTsolverinterface.CLP)
            themes = optimizationmodel.getthemes()
            opareas = []
            for attribute in themes[2].getattributes("?"):
                mask = ["?" for theme in themes]
                mask[2] = attribute
                opareas.append(Heuristics.FMToperatingarea(Core.FMTmask(mask,themes),0.25))
            areaparser = Parser.FMTareaparser()
            themesfields=["THEME"+str(themeid) for themeid in range(1,len(themes)+1)]
            agefield="AGE"
            areafield="SUPERFICIE"
            opareasclusters = areaparser.getclusters(opareas,themes,maplocation,agefield,areafield,20000)
            opareaclusterswithareabounds = []
            for oparea in opareasclusters:
                opareaclusterswithareabounds.append(Heuristics.FMToperatingareacluster(oparea,100,1000))
            #Only Build one period
            for period in range(0,2):
                    print(optimizationmodel.buildperiod())
            allconstraints = optimizationmodel.getconstraints()
            objective = allconstraints.pop(0)
            for constraint in allconstraints:
                    optimizationmodel.setconstraint(constraint)
            optimizationmodel.setobjective(objective)
            optimizationmodel.initialsolve()
            opareastatisticsoutput = None
            opareaareasoutput = None
            for output in optimizationmodel.getoutputs():
                    if "OSUPREC" == output.getname():
                        opareastatisticsoutput=output
                    elif "TOTALAREA" == output.getname():
                        opareaareasoutput=output
            opareaheuristics = optimizationmodel.getoperatingareaclustererheuristics(opareaclusterswithareabounds,opareastatisticsoutput,opareaareasoutput,1)
            opareaheuristics[0].initialsolve()
            for clusters in opareaheuristics[0].getsolution():
                for opareacluster in clusters:
                    print("Clustering together "+[str(mask) for mask in oparea.getallmasks()])
        else:
            print("FMT needs to be compiled with OSI and GDAL")
        
