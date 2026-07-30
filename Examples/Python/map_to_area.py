from FMT import Models
from FMT import Parser

if __name__ == "__main__":
    modelparser = Parser.FMTmodelparser()
    primarylocation = "../Models/TWD_land/TWD_land.pri"
    model = modelparser.readproject(primarylocation,["Spatial"])[0]
    themes = model.getthemes()
    areaparser = Parser.FMTareaparser()
    areaparser.setdefaultexceptionhandler()
    area = areaparser.readvectors(themes,
                                            "../Models/TWD_land/Carte/TWD_land.shp",
                                            "AGE",
                                            "SUPERFICIE",
                                            1,
                                            1,
                                            "",
                                            0.0)
    model.setarea(area)
    areaparser.write(model.getarea(),"TWD_land.are")

