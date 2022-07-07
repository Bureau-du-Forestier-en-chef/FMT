#######################################################################################################
#####################Script utile pour simplifier un modele sans changer les resultats#################
#######################################################################################################
#importation de FMT
library(FMT)
#Creation du parser de model
modelparser <- new(FMTmodelparser)
#Emplacement du fichier primaire du modèle
fichierprimaire <- "cheminverslefichierprimaire.pri"
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
lpmodel <- modelparser$readproject(fichierprimaire,scenarios_voulus,TRUE,TRUE,TRUE)[[1]]#chemin au fichier primaire, liste de scénarios, lecture de la section area ?, lecture de la section output ? , lecture de la section optimize?
lpmodel$setboolparameter(FMTboolmodelparameters$POSTSOLVE,FALSE) #On dit au modèle de pas faire de postsolve pour qu'il garde le modele sous forme presolve
#Nombre de périodes du modèle
lpmodel$setintparameter(FMTintmodelparameters$LENGTH,1)
#Presolve sans résolution
emptyschedules<-list()
lpmodel$doplanning(FALSE,emptyschedules) #FAlSE = pas de résolution emptyschedules = pas de playback
#Écriture du modèle presolve (simplifié)
dir.create("presolve")
modelparser$write(lpmodel,"presolve/")
