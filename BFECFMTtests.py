import sys,os,csv
#rep="T:/Donnees/Courant/Provincial/Python_modules/FMT/Windows/x64-test"
#sys.path.append(rep)
from FMT import WSParser
from FMT import Exception

def getmodel(location,name,debug=False):
        modelparser = WSParser.FMTmodelparser()
        if debug:
                modelparser.setdebugexceptionhandler()
        else:
                modelparser.setfreeexceptionhandler()
        return modelparser.readproject(location,[name])
        
def sortdirs(base_PC,sublocation):
    primaries = []
    for uafolder in os.listdir(base_PC):
            if os.path.isdir(os.path.join(base_PC,uafolder,sublocation)):
                getout = False
                for root,dirs,files in os.walk(os.path.join(base_PC,uafolder,sublocation)):
                        for xfile in files:
                                if xfile.endswith(".pri") and xfile[:2]=="PC":
                                        primaries.append(os.path.join(root,xfile))
                                        getout = True
                                        break
                        if getout:
                                break
    return primaries


def find_scenario(xfiles,scnname):
    for scnfolder in os.listdir(os.path.join(os.path.dirname(xfiles),"Scenarios")):
        if scnname in scnfolder.lower():
                return scnfolder
    return None
                                                   

def getallerrors(location,scenario):
        try:
                getmodel(location,scenario)
        except Exception.FMTerror as error:
                return (error.gettype(),location,scenario,error.what())
        return None
                    


def writeall(notpassed,location_err):
        if os.path.exists(location_err):
                os.remove(location_err)
        with open(location_err ,'wb') as errors_file:
                writer = csv.writer(errors_file)
                writer.writerow(["Type","PC","Description"])
                for errortype,location,scenario,error in notpassed:
                        writer.writerow([errortype,location,error])

if __name__ == "__main__":
        location_err = "C:/Users/cyrgu3/source/repos/FMT/x64/Release/errors.csv"
        #Determination 2013
        sublocation2013 = 'resultats\determination\projet_ws'
        location2013 = "T:/Donnees/Archives/CPF_2013-2018/Sources/Territoires"
        scnname2013 = "scenario_6"
        #Determination 2018
        sublocation2018 = '08_resultats\diffusion\projet_ws'
        location2018= "T:/Donnees/Courant/CPF_2018-2023/Territoires"
        scnname2018 = "scenario_6"
        #Vague 01
        sublocationvague01 = '40_phase2//49_mod_ws_rp2'
        locationvague01 = "T:/Donnees/Courant/CPF_2023-2028/Territoires"
        scnnamevague01 = 'sc1_regl_prov_apsp'
        #Vague 02
##        sublocationvague02 = '40_phase2\49_mod_ws_rp2'
##        locationvague02 = "T:/Donnees/Courant/CPF_2023-2028/Territoires"
##        scnnamevague02 = 'sc1_regl_prov_apsp'
        
        errors = []
        for title,location,keywords,scnname in zip(["determination-2013","determination-2018","Vague-01",],
                                        [location2013,location2018,locationvague01,],
                                        [sublocation2013,sublocation2018,sublocationvague01,],
                                        [scnname2013,scnname2018,scnnamevague01,]):
                tested = 0
                failed = 0
                for primary in sortdirs(location,keywords):
                        scenario = find_scenario(primary,scnname)
                        if scenario is not None:
                                error = getallerrors(primary,scenario)
                                if error is not None:
                                        failed +=1
                                        errors.append(error)
                                tested += 1
                print("Tested "+title+" "+ str((float(failed)/float(max(tested,1)))*100)+"% "+" Failed "+" on "+str(tested)+ " tests")
        writeall(errors,location_err)

                
        

        
        
                                                
