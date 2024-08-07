#######################################################################################################
########Script utile pour transformer des d�veloppements .shp en raster bas� sur les forels############
#######################################################################################################
#importation de FMT
library(FMT)
#Shapefile d'intrant
carte <- "D:/rtools40/home/FMT/Examples/Models/TWD_land/Carte/TWD_land.shp"
#Colonne � exporter en raster
colonne <- "THEME1"
#Fichier de sortie
raster <- "THEME1.tif"
#Taille du cote de la pixel en m�tres
resolution <- 250
#Creation du parser d'area'
areaparser <- new(FMTareaparser)
#Les exceptions � ignorer
exceptions<-c(FMTexc$FMTmissingyield,
                  FMTexc$FMToutput_missing_operator,
                  FMTexc$FMToutput_too_much_operator,
                  FMTexc$FMTinvalidyield_number,
                  FMTexc$FMTundefinedoutput_attribute,
                  FMTexc$FMToveridedyield, 
                  FMTexc$FMTdeathwithlock,
		  FMTexc$FMTinvalid_geometry)
#On applique les exceptions au parser
areaparser$seterrorstowarnings(exceptions)
#Transformation du field theme1 en raster
areaparser$vectorfieldtoraster(carte,
								raster,
								resolution,
								colonne,
								TRUE)