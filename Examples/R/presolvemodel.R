#######################################################################################################
#####################Script utile pour simplifier un modele sans changer les resultats#################
#######################################################################################################
#importation de FMT
library(FMT)
#Creation du parser de model
modelparser <- new(FMTmodelparser)
#Emplacement du fichier primaire du mod�le
fichierprimaire <- "cheminverslefichierprimaire.pri"
#Nom du sc�nario � utiliser
scenarios_voulus <- c("03_sc1a_bfecopt_bf")
#Les exceptions � ignorer lors de la lecture du sc�nario
exceptions<-c(FMTexc$FMTmissingyield,
                  FMTexc$FMToutput_missing_operator,
                  FMTexc$FMToutput_too_much_operator,
                  FMTexc$FMTinvalidyield_number,..
                  FMTexc$FMTundefinedoutput_attribute,
                  FMTexc$FMToveridedyield, 
                  FMTexc$FMTdeathwithlock)
#On applique les exceptions au parser
modelparser$seterrorstowarnings(exceptions)
#Lecture d'un sc�nario Woodstock
lpmodel <- modelparser$readproject(fichierprimaire,scenarios_voulus,TRUE,TRUE,TRUE)[[1]]#chemin au fichier primaire, liste de sc�narios, lecture de la section area ?, lecture de la section output ? , lecture de la section optimize?
lpmodel$setboolparameter(FMTboolmodelparameters$POSTSOLVE,FALSE) #On dit au mod�le de pas faire de postsolve pour qu'il garde le modele sous forme presolve
#Nombre de p�riodes du mod�le
lpmodel$setintparameter(FMTintmodelparameters$LENGTH,1)
#Presolve sans r�solution
emptyschedules<-list()
lpmodel$doplanning(FALSE,emptyschedules) #FAlSE = pas de r�solution emptyschedules = pas de playback
#�criture du mod�le presolve (simplifi�)
dir.create("presolve")
modelparser$write(lpmodel,"presolve/")
