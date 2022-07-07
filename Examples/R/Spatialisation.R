#######################################################################################################
########Script utile pour spatialiser la récolte sur le territoire en format raster####################
#######################################################################################################
#importation de FMT
library(FMT)
#Creation du parser de model
modelparser <- new(FMTmodelparser)
#Emplacement du fichier primaire du modèle
fichierprimaire <- "Cheminverslefichierprimaire.pri"
#Emplacement de la carte calcul
cartecalcul <- "Cheminverslacartecalcul.shp"
#Emplacement d'ecriture des rasters
rasters <- "rasters/"
#Nom du scénario à utiliser
scenarios_voulus <- c("03_sc1a_bfecopt_bf")
#Les exceptions à ignorer lors de la lecture du scénario
exceptions<-c(FMTexc$FMTmissingyield,
                  FMTexc$FMToutput_missing_operator,
                  FMTexc$FMToutput_too_much_operator,
                  FMTexc$FMTinvalidyield_number,..
                  FMTexc$FMTundefinedoutput_attribute,
                  FMTexc$FMToveridedyield, 
                  FMTexc$FMTdeathwithlock)
#On applique les exceptions au parser
modelparser$seterrorstowarnings(exceptions)
#Lecture d'un scénario Woodstock
model<-modelparser$readproject(fichierprimaire,scenarios_voulus,TRUE,TRUE,TRUE)[[1]]#Lecture 
lpmodel<-new(FMTlpmodel,model,FMTsolverinterface$MOSEK)#Création d'un modèle de programmation linéaire
#Réaliser l'optimisation sur 5 périodes
lpmodel$setintparameter(FMTintmodelparameters$LENGTH,5)
emptyschedules<-list()
lpmodel$doplanning(TRUE,emptyschedules)
#Aller chercher la schedule pour les 5 périodes de planification
schedules <- list()
for (period in 1:5)
    {
    schedules[[period]]<-lpmodel$getsolution(period,TRUE) #Prendre la solution avec les lock (TRUE)
    }
#Creation du parser d'area'
areaparser <- new(FMTareaparser)
#Transformation du shapefile en rasters FMT
forest<-areaparser$vectormaptoFMTforest(cartecalcul,
										250,lpmodel$getthemes(),#Resolution des pixels de 250 mètres X 250 mètres
										"AGE","SUPERFICIE",1.0, #Field age, superficie, facteur AGE
										1.0,"STANLOCK",#Facteur superficie, Field STANLOCK
										0.0,"",#Superficie minimale du polygone, dossier de sortie vide
										TRUE)#Ajuster la création de raster au FOREL
sesmodel<-new(FMTsesmodel,model)#Création d'un modèle de simulation spatialement explicit
#Transformation des transitions multiple à des transitions simple
transitionsimple<-list()
transitionid <- 1
for (transition in sesmodel$gettransitions())
    {
    transitionsimple[[transitionid]]<-transition$single()
    transitionid <- transitionid + 1
    }
sesmodel$settransitions(transitionsimple)
sesmodel$setinitialmapping(forest)
sesmodel$doplanning(TRUE,schedules)
#Creation du repertoir d'ecriture
dir.create(rasters)
#Écriture de la spatialisation pour chaque période
for (period in 1:5)
    {
    GCBMtransitions<-areaparser$writedisturbances(
            rasters, #Localisation
            sesmodel$getspschedule(), #Solution spatialement explicit
            sesmodel$getactions(),#actions
            sesmodel$getthemes(),#themes
            period)#Période
    }