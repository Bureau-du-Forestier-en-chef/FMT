from FMT import Models
from FMT import Parser
from FMT import Core
from FMT import Exception as FMTexception
from pathlib import Path
import json,copy



    
    
def get_all_masks(THEMES,THEMES_SELECTED,FILTER):
    MASK_STR = ["?" for i in range(0,len(THEMES))]
    for key,item in FILTER.items():
        MASK_STR[key] = item
    BASE_MASK = Core.FMTmask(
                MASK_STR,
                THEMES)
    all_masks = [BASE_MASK]
    for i in THEMES_SELECTED:
        THE_THEME = THEMES[i]
        new_masks = []
        for MASK in all_masks:
            MASKS = MASK.decompose(THE_THEME)
            new_masks += MASKS
        all_masks = new_masks
    return all_masks
    
def get_categories(YIELDS_NAMES,ALL_MASKS,THEMES_SELECTED,
                YIELDS,AGE_RANGE,PERIOD_RANGE):
    target_dev=Core.FMTdevelopment()
    categories = {}
    for MASK in ALL_MASKS:
        target_dev.setmask(MASK)
        MASK_LIST = str(MASK).split(" ")
        NAME = " ".join([MASK_LIST[i] for i in THEMES_SELECTED])
        categories[NAME] = ("",0)
        for YIELD in YIELDS_NAMES:
            yield_total = 0
            for AGE in AGE_RANGE:
                target_dev.setage(AGE)
                for PERIOD in PERIOD_RANGE:
                    target_dev.setperiod(PERIOD)
                    VALUE = YIELDS.get(target_dev.getyieldrequest(),YIELD)
                    yield_total += VALUE
            
            if yield_total > categories[NAME][1]:
                categories[NAME] = (YIELD,yield_total)
        
    return categories
    
def write_categories(CATEGORIES,LOCATION,SELECTED):
    cleaner_dict = {}
    for key,item in CATEGORIES.items():
        ATTRIBUTES = key.split(' ')
        THEMES = ["THEME"+str(i+1) for i in SELECTED]
        if item[0] not in cleaner_dict:
            cleaner_dict[item[0]] = {}
            for THEME in THEMES:
                cleaner_dict[item[0]][THEME] = []
        for THEME,ATTRIBUTE in zip(THEMES,ATTRIBUTES):
            cleaner_dict[item[0]][THEME].append(ATTRIBUTE)
    with open(LOCATION, 'w', encoding='utf-8') as f:
        json.dump(cleaner_dict, f, indent=4, ensure_ascii=False)
    
   


if __name__ == "__main__":
    modelparser = Parser.FMTmodelparser()
    #PRIMARY_FILE = "../Models/TWD_land/TWD_land.pri"
    #MODEL = modelparser.readproject(PRIMARY_FILE,["Spatial"])[0]
    #THEMES_SELECTED = [1,2]
    #FILTER = {}
    #YIELDS_NAMES = ["VOLUMETOTAL"]
    PRIMARY_FILE = "D:/CC_V1/CC_modele_feu/WS_CC/Feux_2023_ouest_V01.pri"
    SCENARIO_NAME = "strategique_histo"
    #THEMES_SELECTED = [2,4,14]
    FILTER = {7:"REGTBE"}
    THEMES_SELECTED = [4]
    YIELDS_NAMES = ["YTBE_SS","YTBE_SR","YTBE_SF","YTBE_FRS"]
    modelparser.seterrorstowarnings([FMTexception.FMTexc.FMToveridedyield,FMTexception.FMTexc.FMTmissingyield,
        FMTexception.FMTexc.FMTdeathwithlock,FMTexception.FMTexc.FMToutput_too_much_operator])
    MODEL = modelparser.readproject(PRIMARY_FILE,[SCENARIO_NAME])[0]
    AGE_RANGE = range(0,30)
    PERIOD_RANGE = range(0,30)
    THEMES = MODEL.getthemes()
    ALL_MASKS = get_all_masks(THEMES,THEMES_SELECTED,FILTER)
    YIELDS = MODEL.getyields()
    CATEGORIES = get_categories(YIELDS_NAMES,ALL_MASKS,
        THEMES_SELECTED,YIELDS,AGE_RANGE,PERIOD_RANGE)
    OUTPUT = Path(__file__).parent.absolute() / Path(SCENARIO_NAME+".json")
    write_categories(CATEGORIES,OUTPUT,THEMES_SELECTED)