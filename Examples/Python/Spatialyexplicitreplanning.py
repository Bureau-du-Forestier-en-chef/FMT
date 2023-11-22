from FMT import Models
from FMT import Parser
from FMT import Core
from FMT import Version
from FMT import Spatial
import os,sys

gettrace = getattr(sys, 'gettrace', None)


def get_initial_inventory(model:Models.FMTmodel,rasterslocation:str)->Spatial.FMTforest:
    ageraster = os.path.join(rasterslocation,"AGE.tif")
    areaparser = Parser.FMTareaparser()
    themesrasters = []
    themeid=1
    for theme in model.getthemes():
        themesrasters.append(os.path.join(rasterslocation,"THEME"+str(themeid)+".tif"))
        themeid+=1
    initialforestmap=areaparser.readrasters(model.getthemes(),themesrasters,ageraster,1,0.0001)
    return initialforestmap
#.opt keywords
#_SETFROMGLOBAL    
def get_replanning_constraints(model:Models.FMTmodel,model_type:str,constraints:[Core.FMTconstraint],period:int)->[Core.FMTconstraint]:
    new_constraints = constraints.copy()
    for old_constraint,new_constraint in zip(constraints,new_constraints):
        if old_constraint.issetfrom(model_type):
            constraint_value = model.getoutput(old_constraint,period,Core.FMToutputlevel.totalonly)["Total"]
            new_constraint = constraint_value.setfrom(model_type,constraint_value)
    return new_constraints
#_SETGLOBALSCHEDULE   
def get_strategic_schedule(model:Models.FMTmodel,replanning_period:int)->Core.FMTschedule:
    schedule = model.getsolution(replanning_period,True)
    schedule.setperiod(1)
    return schedule
#_REPLICATE
def set_replicate(model:Models.FMTnssmodel,replanning_period:int,replicate:int):
    new_constraints = []
    for constraint in model.getconstraints():
        new_constraints.append(constraint.getfromreplicate(replicate+1, replanning_period))
    model.setconstraints(new_constraints)
    
def to_area(model:Models.FMTlpmodel,forest:Spatial.FMTforest,period:int):
    new_devs = forest.getarea()
    for dev in new_devs:
        dev.setperiod(period)
    model.setarea(new_devs)

def print_forest_state(model:Models.FMTmodel,period:int,repliate:int,lookat:int):
    if gettrace is not None and gettrace():
        print("-"*100)
        print("Forest state of",model.getname(),"at period",period,"for replicate",replicate)
        areas = model.getarea(lookat)
        areas.sort()
        for dev in areas:
            print(dev)
        print("-"*100)
    
   
   


if __name__ == "__main__":
    if Version.FMTversion().hasfeature("OSI") and Version.FMTversion().hasfeature("GDAL"):
        modelparser = Parser.FMTmodelparser()
        path = "../Models/TWD_land/TWD_land.pri"
        scenarios = ["Globalreplanning","Globalfire","Localreplanning"]
        models = modelparser.readproject(path, scenarios)
        spatial_forest = get_initial_inventory(models[0],"../Models/TWD_land/rasters/") #Always with period = 0
        replicates = 10 # Number of time you want to replicate your replanning
        length = 10 #Length of the planning horizon
        burned_area = [[0.0 for period in range(length)] for replicate in range(replicates)]
        harvested_volume = [[0.0 for period in range(length)] for replicate in range(replicates)]
        salvaged_area = [[0.0 for period in range(length)] for replicate in range(replicates)]
        for replicate in range(replicates):
            for replanning_period in range(1,length+1):
                #Build the strategic spatialy referenced model and solve it
                strategic = Models.FMTlpmodel(models[0], Models.FMTsolverinterface.CLP)
                strategic.setparameter(Models.FMTintmodelparameters.LENGTH, length) #The length of the planning horizon 
                strategic.setparameter(Models.FMTintmodelparameters.NUMBER_OF_THREADS,1)
                to_area(strategic,spatial_forest,replanning_period-1)#Set the area using the spatial forest
                if not strategic.doplanning(True):
                    raise(RuntimeError("non optimal strategic at period " +str(replanning_period)+" at replicate "+str(replicate)))
                print_forest_state(strategic,replanning_period,replicate,replanning_period)
                strategic_schedule = get_strategic_schedule(strategic,replanning_period)
                print("objective value of strategic model", strategic.getobjectivevalue(),"at period",replanning_period)
                #Build the stochastic spatialy referenced model
                stochastic = Models.FMTnssmodel(models[1],0)
                stochastic.setparameter(Models.FMTintmodelparameters.LENGTH, length) #The length of the planning horizon 
                to_area(stochastic,spatial_forest,replanning_period-1) #Set the area using the spatial forest
                set_replicate(stochastic,replanning_period,replicate)
                stochastic.doplanning(True)
                print_forest_state(stochastic,replanning_period,replicate,replanning_period)
                stochastic_schedule = stochastic.getsolution(replanning_period,True)
                schedule_without_death = {}
                for action,devs in stochastic_schedule:
                    if action.getname() != "_DEATH":
                        schedule_without_death[action] = devs
                stochastic_schedule = Core.FMTschedule(1,schedule_without_death)
                #Put those stochastic disturbances on a map using a spatialy explicit model
                #Put stochastic disturbances is this FMTmodel
                spatial_stochastic = Models.FMTsesmodel(models[1])
                singletransitions = []
                for transition in spatial_stochastic.gettransitions():
                    singletransitions.append(transition.single())
                spatial_stochastic.settransitions(singletransitions)
                spatial_stochastic.setinitialmapping(spatial_forest)
                spatial_stochastic.setparameter(Models.FMTintmodelparameters.LENGTH, 1) #The length of the planning horizon is 1 period
                spatial_stochastic.setparameter(Models.FMTintmodelparameters.NUMBER_OF_ITERATIONS, 20) #Number of time the heuristics try to spatialize
                set_replicate(spatial_stochastic,replanning_period,replicate)
                spatial_stochastic.doplanning(True,[stochastic_schedule])
                for output in spatial_stochastic.getoutputs():
                    if output.getname() == "BURNEDAREA":
                        area = spatial_stochastic.getoutput(output,1,Core.FMToutputlevel.totalonly)["Total"]
                        burned_area[replicate][replanning_period-1] = area
                        break
                spatial_schedule = spatial_stochastic.getspschedule()
                print_forest_state(spatial_stochastic,replanning_period,replicate,1)
                #Here you probably also want to get the spatial disturbances out of spatial_stochastic and keep them for
                #your carbon budget model. You can use FMTareaparser::writedisturbances() to get all disturbances
                spatial_forest = spatial_schedule.getforestperiod(1) #Get the new states of the forest after natural disturbances
                spatial_forest.setperiod(0)
                #Build the tactical spatialy referenced model and solve it
                tactical = Models.FMTlpmodel(models[2], Models.FMTsolverinterface.CLP)
                tactical_constraints = get_replanning_constraints(strategic,"LOCAL",tactical.getconstraints(),1)#Make sure that you respect the constraints with the _SETFROMGLOBAL
                tactical.setconstraints(tactical_constraints)
                tactical.setparameter(Models.FMTintmodelparameters.LENGTH, 1) #The length of the planning horizon is 1 period
                tactical.setparameter(Models.FMTintmodelparameters.NUMBER_OF_THREADS,1)
                to_area(tactical,spatial_forest,0) #Set the area using the spatial forest
                tactical.doplanning(False)
                schedule_weight = tactical.getconstraints()[0].getscheduleweight()
                tactical.addscheduletoobjective(strategic_schedule,schedule_weight)#Put some weight on the decisions taken by  the strategic model  
                if not tactical.initialsolve():
                    raise(RuntimeError("non optimal tactical at period " +str(replanning_period)+" at replicate "+str(replicate)))
                print("objective value of tactical model", tactical.getobjectivevalue(),"at period",replanning_period)
                print_forest_state(tactical,replanning_period,replicate,1)
                #Put those disturbances on the map 
                spatial_tactical = Models.FMTsesmodel(tactical)
                singletransitions = []
                for transition in spatial_tactical.gettransitions():
                    singletransitions.append(transition.single())
                spatial_tactical.settransitions(singletransitions)
                tactical_schedule = tactical.getsolution(1,True)
                tactical_schedule.setperiod(1)
                print(tactical_schedule)
                spatial_tactical.setinitialmapping(spatial_forest)
                spatial_tactical.setparameter(Models.FMTintmodelparameters.LENGTH, 1) #The length of the planning horizon is 1 period
                spatial_tactical.setparameter(Models.FMTintmodelparameters.NUMBER_OF_ITERATIONS, 20) #Number of time the heuristics try to spatialize
                spatial_tactical.doplanning(True,[tactical_schedule])
                for output in spatial_tactical.getoutputs():
                    if output.getname() == "OVOLREC":
                        volume= spatial_tactical.getoutput(output,1,Core.FMToutputlevel.totalonly)["Total"]
                        harvested_volume[replicate][replanning_period-1] = volume
                    elif(output.getname() == "RECUPAREA"):
                        area = spatial_tactical.getoutput(output,1,Core.FMToutputlevel.totalonly)["Total"]
                        salvaged_area[replicate][replanning_period-1] = area
                spatial_schedule = spatial_tactical.getspschedule()
                print_forest_state(spatial_tactical,replanning_period,replicate,1)
                #Here you probably also want to get the spatial disturbances out of spatial_tactical and keep them for
                #your carbon budget model. You can use FMTareaparser::writedisturbances() to get all disturbances
                #Now update the spatial_forest for the next period...
                spatial_forest = spatial_schedule.getforestperiod(2,True)
                spatial_forest.setperiod(0)
                print("period",str(replanning_period),"at replicate",str(replicate),"done")
        print("Burned area",burned_area)
        print("Harvested volume",harvested_volume)
        print("Salvaged area ",salvaged_area)
    else:
        print("FMT needs to be compiled with OSI")
