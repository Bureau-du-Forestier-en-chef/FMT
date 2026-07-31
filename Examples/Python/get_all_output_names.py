from FMT import Models
from FMT import Parser
from pathlib import Path
from FMT import Exception as FMTexception

    
def get_output_names(MODEL):
    return [name.getname() for name in MODEL.getoutputs()]
    
def write(location,OUTPUTS):
    Path(location).write_text(";".join([OUTPUT for OUTPUT in OUTPUTS]))
    
    

if __name__ == "__main__":
    modelparser = Parser.FMTmodelparser()
    PRIMARY_FILE = "../Models/TWD_land/TWD_land.pri"
    SCENARIO_NAME = "Spatial"
    modelparser.seterrorstowarnings([FMTexception.FMTexc.FMToveridedyield,FMTexception.FMTexc.FMTmissingyield,
        FMTexception.FMTexc.FMTdeathwithlock,FMTexception.FMTexc.FMToutput_too_much_operator])
    MODEL = modelparser.readproject(PRIMARY_FILE,[SCENARIO_NAME])[0]
    OUTPUTS = get_output_names(MODEL)
    OUTPUT = Path(__file__).parent.absolute() / Path(SCENARIO_NAME +".txt")
    write(OUTPUT,OUTPUTS)