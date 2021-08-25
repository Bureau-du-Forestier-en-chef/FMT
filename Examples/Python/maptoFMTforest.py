import sys,os
sys.path.append("../../")
sys.path.append("../../../x64/Release")
sys.path.append("../../Release")
print(sys.path)
from FMT import Models
from FMT import Parser

if __name__ == "__main__":
    modelparser = Parser.FMTmodelparser()
    primarylocation = "../Models/TWD_land/TWD_land.pri"
    models = modelparser.readproject(primarylocation,["Spatial"])
    sesmodel = Models.FMTsesmodel(models[0])
    themes = sesmodel.getthemes()
    areaparser = Parser.FMTareaparser()
    areaparser.setdefaultexceptionhandler()
    forest = areaparser.vectormaptoFMTforest("../Models/TWD_land/Carte/TWD_land.shp",
                                            380,
                                            themes,
                                            "AGE",
                                            "SUPERFICIE",
                                            1,
                                            0.0001,
                                            "",
                                            0.0,
                                            "../Models/TWD_land/rasters/",
                                            False)

