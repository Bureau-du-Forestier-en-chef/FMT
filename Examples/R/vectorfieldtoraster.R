#######################################################################################################
########Script utile pour transformer des développements .shp en raster basé sur les forels############
#######################################################################################################
#importation de FMT
library(FMT)
#Shapefile d'intrant
carte <- "D:/rtools40/home/FMT/Examples/Models/TWD_land/Carte/TWD_land.shp"
#Colonne à exporter en raster
colonne <- "THEME1"
#Fichier de sortie
raster <- "THEME1.tif"
#Taille du cote de la pixel en mètres
resolution <- 250
#Creation du parser d'area'
areaparser <- new(FMTareaparser)
#Transformation du field theme1 en raster
areaparser$vectorfieldtoraster(carte,
								raster,
								resolution,
								colonne,
								TRUE)