from FMT import Models
from FMT import Parser
from FMT import Core
from FMT import Version
from FMT import Exception as FMTexception
import csv,argparse
                     


if __name__ == "__main__":
    ap = argparse.ArgumentParser()
    ap.add_argument("-pri", "--primary", required=True,help="Fichier primaire")
    ap.add_argument("-scn", "--scenario", required=True,help="Nom du scenario")
    ap.add_argument("-out", "--outputs", required=True,help="Liste d'outputs separe d'une virgule")
    ap.add_argument("-fil", "--file", required=True,help="Fichier de sortie")
    ap.add_argument("-len", "--length", required=True,help="Nombre de periode du scenario")
    args = vars(ap.parse_args())
    if Version.FMTversion().hasfeature("OSI"):
        primarylocation = args["primary"]
        modelparser = Parser.FMTmodelparser()
        print(primarylocation,"|",args["scenario"])
        modelparser.seterrorstowarnings([FMTexception.FMTexc.FMToveridedyield,FMTexception.FMTexc.FMTmissingyield,FMTexception.FMTexc.FMTdeathwithlock,FMTexception.FMTexc.FMToutput_too_much_operator])
        models = modelparser.readproject(primarylocation,[args["scenario"]])
        optimizationmodel=Models.FMTlpmodel(models[0],Models.FMTsolverinterface.MOSEK)
        outputtolookfor=args["outputs"].split(",")
        csvlocation = args["file"]
        length = int(args["length"])
        outputstocheck=[]
        for output in optimizationmodel.getoutputs():
            if output.getname() in outputtolookfor:
                outputstocheck.append(output)
        for period in range(0,length):
                print(optimizationmodel.buildperiod())
        allconstraints = optimizationmodel.getconstraints()
        objective = allconstraints.pop(0)
        for constraint in allconstraints:
            print(optimizationmodel.setconstraint(constraint))
        print(optimizationmodel.setobjective(objective))
        if optimizationmodel.initialsolve():
            basevalues=[]
            #optimizationmodel.setquietlogger()
            for output in outputstocheck:
                values = []
                for period in range(1,length+1):
                    values.append(optimizationmodel.getoutput(output,period,Core.FMToutputlevel.totalonly)["Total"])
                basevalues.append(values)
            with open(csvlocation, 'w',buffering=1,newline='') as f:
                writer = csv.writer(f,delimiter=';')
                writer.writerow(["CONSTRAINT","OUTPUT","PERIOD","PERCENTAGE"])
                for constraint in allconstraints:
                    optimizationmodel.eraseconstraint(constraint)
                    constraintname = str(constraint).rstrip("\n")
                    if optimizationmodel.initialsolve():
                        outid = 0
                        for output in outputstocheck:
                            perid = 0
                            for period in range(1,length+1):
                                value = optimizationmodel.getoutput(output,period,Core.FMToutputlevel.totalonly)["Total"]
                                ratio = ((value-basevalues[outid][perid])/basevalues[outid][perid])*100
                                writer.writerow([constraintname,output.getname(),period,ratio])
                                perid+=1
                            outid+=1
                    optimizationmodel.setconstraint(constraint)
        
    else:
        print("FMT needs to be compiled with OSI")
