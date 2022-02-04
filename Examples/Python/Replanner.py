from FMT import Models
from FMT import Parser
from FMT import Core
from FMT import Version
from FMT import Parallel
import os,argparse


if __name__ == "__main__":
    ap = argparse.ArgumentParser()
    ap.add_argument("-pri", "--primary", required=True,help="Fichier primaire")
    ap.add_argument("-str", "--strategic", required=True,help="Nom du scenario strategique")
    ap.add_argument("-tac", "--tactic", required=True,help="Nom du scenario tactique")
    ap.add_argument("-sto", "--stochastic", required=True,help="Nom du scenario stochastique")
    ap.add_argument("-out", "--outputs", required=True,help="Liste d'outputs separe d'une virgule")
    ap.add_argument("-fol", "--folder", required=True,help="Dossier de sortie")
    ap.add_argument("-rep", "--replicates", required=True,help="Nombre de replicas")
    ap.add_argument("-len", "--strategiclength", required=True,help="Nombre de periode du strategique")
    ap.add_argument("-rel", "--replanninglength", required=True,help="Nombre de periode de re-plannification")
    ap.add_argument("-thr", "--threads", required=True,help="Nombre de threads")
    ap.add_argument("-swe", "--Strategicweight", required=True,help="Poids de la cedule strategique")
    args = vars(ap.parse_args())
    if Version.FMTversion().hasfeature("OSI"):
        modelparser = Parser.FMTmodelparser()
        path = args["primary"]
        scenarios = [args["strategic"],args["stochastic"],args["tactic"]]
        models = modelparser.readproject(path, scenarios)
        globalmodel = Models.FMTlpmodel(models[0], Models.FMTsolverinterface.MOSEK)
        globalmodel.setparameter(Models.FMTintmodelparameters.LENGTH,int(args["strategiclength"]))
        globalmodel.setparameter(Models.FMTintmodelparameters.NUMBER_OF_THREADS,1)
        stochastic = Models.FMTnssmodel(models[1],0)
        stochastic.setparameter(Models.FMTintmodelparameters.LENGTH, 1)
        localmodel = Models.FMTlpmodel(models[2], Models.FMTsolverinterface.MOSEK)
        localmodel.setparameter(Models.FMTintmodelparameters.LENGTH, 1)
        localmodel.setparameter(Models.FMTintmodelparameters.NUMBER_OF_THREADS,1)
        selectedoutputs=[]
        for output in globalmodel.getoutputs():
            if output.getname() in args["outputs"].split(","):
                selectedoutputs.append(output)
        outputlocation = args["folder"]
        layersoptions=["SEPARATOR=SEMICOLON"]
        replanningtask=Parallel.FMTreplanningtask(globalmodel,stochastic, localmodel, selectedoutputs, outputlocation, "CSV", layersoptions,int(args["replicates"]),int(args["replanninglength"]),float(args["Strategicweight"]),0.5, Core.FMToutputlevel.totalonly)
        handler = Parallel.FMTtaskhandler(replanningtask,int(args["threads"]))
        handler.setquietlogger()
        handler.conccurentrun()
    else:
        print("FMT needs to be compiled with OSI")
