#######################################################################################################
########Script utile pour transformer des d�veloppements .shp en raster bas� sur les forels############
#######################################################################################################
#importation de FMT
library(FMT)
#Creation du parser de model
modelparser <- new(FMTmodelparser)
#Emplacement du fichier primaire du mod�le
fichierprimaire <- "Cheminauprimaire.pri"
#Emplacement de la carte calcul
cartecalcul <- "Cheminaushapefile.shp"
#Emplacement d'ecriture des rasters
rasters <- "rasters/"
#Nom du sc�nario � utiliser
scenarios_voulus <- c("03_sc1a_bfecopt_bf")
#Les exceptions � ignorer lors de la lecture du sc�nario
exceptions<-c(FMTexc$FMTmissingyield,
                  FMTexc$FMToutput_missing_operator,
                  FMTexc$FMToutput_too_much_operator,
                  FMTexc$FMTinvalidyield_number,
                  FMTexc$FMTundefinedoutput_attribute,
                  FMTexc$FMToveridedyield, 
                  FMTexc$FMTdeathwithlock,
		  FMTexc$FMTinvalid_geometry)
#On applique les exceptions au parser
modelparser$seterrorstowarnings(exceptions)
#Lecture d'un sc�nario Woodstock
lpmodel <- modelparser$readproject(fichierprimaire,scenarios_voulus,TRUE,TRUE,TRUE)[[1]]#chemin au fichier primaire, liste de sc�narios, lecture de la section area ?, lecture de la section output ? , lecture de la section optimize?
#Creation du parser d'area'
areaparser <- new(FMTareaparser)
#Creation du repertoir d'ecriture
dir.create(rasters)
#Transformation du shapefile en rasters FMT
forest<-areaparser$vectormaptoFMTforest(cartecalcul,
										250,lpmodel$getthemes(),#Resolution des pixels de 250 m�tres X 250 m�tres
										"AGE","SUPERFICIE",1.0, #Field age, superficie, facteur AGE
										1.0,"STANLOCK",#Facteur superficie, Field STANLOCK
										0.0,rasters,#Superficie minimale du polygone, dossier de sortie
										TRUE)#Ajuster la cr�ation de raster au FOREL