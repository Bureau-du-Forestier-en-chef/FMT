from FMT import Models
from FMT import Parser

if __name__ == "__main__":
    modelparser = Parser.FMTmodelparser()
    PRIMARY_LOCATION = "../Models/TWD_land/TWD_land.pri"
    SCENARIO = "Spatial"
    STANLOCK=""
    MAP="D:/CC_V1/CC_modele_feu/WS_CC/Carte/Mod_cc_v1R10.xml"
    PRIMARY_LOCATION = "D:/CC_V1/CC_modele_feu/WS_CC/Feux_2023_ouest_V01.pri"
    SCENARIO = "PlayBack_Histo"
    STANLOCK="STANLOCK"
    #MAP="../Models/TWD_land/Carte/TWD_land.shp"
    model = modelparser.readproject(PRIMARY_LOCATION,[SCENARIO])[0]
    themes = model.getthemes()
    areaparser = Parser.FMTareaparser()
    areaparser.setdefaultexceptionhandler()
    area = areaparser.readvectors(themes,
                                            MAP,
                                            "AGE",
                                            "SUPERFICIE",
                                            1,
                                            1,
                                            STANLOCK,
                                            0.0)
    model.setarea(area)
    areaparser.write(model.getarea(),"TWD_land.are")

