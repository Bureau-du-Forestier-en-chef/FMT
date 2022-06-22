#importation de FMT
library(FMT)
#Creation du model
model<-new(FMTlpmodel,new(FMTmodel),FMTsolverinterface$MOSEK)
#Établir une gestion des erreurs faite par l'utilisateur R
model$setfreeexceptionhandler()
#Tous les strings doivent êtres en majuscule
#Noms des themes
nomthemes <- list()
nomthemes[[1]] <- "Groupes de strates"
nomthemes[[2]] <- "Famille de courbes"
nomthemes[[3]] <-  "Compartiments spatiaux"
#Creation des attributs des différents thèmes
attributs <- list()
attributs[[1]] <- c("GS1","GS2","GS3") #theme 1
attributs[[2]] <- c("FC4","FC6") #theme 2
attributs[[3]] <- c("COS8","COS20","COS30","COS60") #theme 3
#Creation des noms d'attributs
nomsattributs<- list()
nomsattributs[[1]] <- c("GS blabla 1","GS blabla 2","GS blabla 3")
nomsattributs[[2]] <- c("Courbe 1","Courbe 2")
nomsattributs[[3]] <- c("COS blabla 1","COS blabla 2","COS blabla 3","COS blabla 4")
#Creation des aggregats
aggregats <- list()
aggregats[[1]] <- vector(mode ="character") #aggregat du theme 1
aggregats[[2]] <- vector(mode ="character") #aggregat du theme 2
aggregats[[3]] <- c("COSSTA") #aggregat du theme 3
#Creation des éléments des aggrégats
elementsaggregats <- list()
elementsaggregats[[1]] <- list()
elementsaggregats[[2]] <- list()
elementsaggregats[[3]] <- list()
elementsaggregats[[3]][[1]] <- c("COS8","COS20") #Seulement le theme 3 possède un aggrgat (à l'index 1)
#Creation de la liste de themes nous allons maintenant créer les themes et les mettre dans une liste
#On utilise le tryCatch pour utiliser le gestionnaire de message d'erreur de FMT pour savoir si on a fait une erreur dans nos données
themes <- list()
tryCatch(
	{
	themesize <- 0
	for (themeid in c(1,2,3))
		{
			themes[[themeid]] <- new(FMTtheme,attributs[[themeid]],nomsattributs[[themeid]],aggregats[[themeid]],elementsaggregats[[themeid]],themeid-1,themesize,nomthemes[[themeid]])
			themesize <- themesize + length(attributs[[themeid]])
		}
	},
    error = function(e){ 
		print("Problème suivant lors de la création des thèmes")
        print(e$message)
		quit(save="ask")
    })
#Creation de la section area
#Nous allons créer 4 dévelopement distinct
area <- list()
#le constructeur du FMTmask requirt un string et les themes que nous avons déjà construit
#le constructeur du FMTactualdevelopment requirt un masque,l'age du developpement, le lock du developement (0 = pas de lock) et finalement la superficie
tryCatch(
	{
	area[[1]] <- new(FMTactualdevelopment,new(FMTmask,"GS1 FC6 COS30",themes),5,0,1200.00)
	area[[2]] <- new(FMTactualdevelopment,new(FMTmask,"GS2 FC4 COS20",themes),10,0,1456.00)
	area[[3]] <- new(FMTactualdevelopment,new(FMTmask,"GS3 FC6 COS60",themes),20,1,2352.00)
	area[[4]] <- new(FMTactualdevelopment,new(FMTmask,"GS1 FC4 COS30",themes),25,0,5454.00)
	},
    error = function(e){ 
		print("Problème suivant lors de la création de la section area")
        print(e$message)
		quit(save="ask")
    })
#La résultante est une liste de developement (section area)
#Creation des yields
yields <- new(FMTyields)
tryCatch(
	{
	fc6yield <- new(FMTageyieldhandler,new(FMTmask,"? FC6 ?",themes))
	fc4yield <- new(FMTageyieldhandler,new(FMTmask,"? FC4 ?",themes))
	fc6j <- 0
	fc4j <- 0
	for (age in 1:50)
		{
		fc6yield$push_base(age)
		fc4yield$push_base(age)
		fc6yield$push_data("YVTOT", fc6j)
		fc4yield$push_data("YVTOT", fc4j)
		fc6j <- fc6j + 10
		fc4j <- fc4j + 5
		}
	yields$push_back(new(FMTmask,"? FC6 ?",themes),fc6yield)
	yields$push_back(new(FMTmask,"? FC4 ?",themes),fc4yield)
	yields$update() #Ne pas oublier d'appeller update
	},
    error = function(e){ 
		print("Problème suivant lors de la création de la section yield")
        print(e$message)
		quit(save="ask")
    })
#Creation des actions
actions <- list()
tryCatch(
	{
	actions[[1]] <- new(FMTaction,"ACT",TRUE,TRUE)#Nom,agereset,respect lock
	actionspec <- new(FMTspec)
	#On met un minimum de 3 et maximum de 100 d'âge pour l'action
	actionspec$addbounds(new(FMTagebounds,FMTsection$Action,100,3))
	actions[[1]]$push_back(new(FMTmask,"? ? ?",themes),actionspec)
	actions[[1]]$update()
	},
    error = function(e){ 
		print("Problème suivant lors de la création de la section action")
        print(e$message)
		quit(save="ask")
    })
#Creation des transitions
transitions <- list()
tryCatch(
	{
	transitions[[1]] <- new(FMTtransition,"ACT")#nom de l'action
	fork <- new(FMTfork)
	fork$add(new(FMTtransitionmask,"? ? ?", themes, 100.0))#proportion de transition de 100
	transitions[[1]]$push_back(new(FMTmask,"? ? ?", themes),fork)
	transitions[[1]]$update()
	},
    error = function(e){ 
		print("Problème suivant lors de la création de la section transition")
        print(e$message)
		quit(save="ask")
    })
#Creation du lifespan
lifespan <- new(FMTlifespans)
tryCatch(
	{
	lifespan$push_back(new(FMTmask,"? ? ?",themes),80)
	lifespan$update();
	},
    error = function(e){ 
		print("Problème suivant lors de la création de la section lifespan")
        print(e$message)
		quit(save="ask")
    })
#Creation d'un output
outputs<-list()
tryCatch(
	{
	sources<-list()
	operators<-list()
	sources[[1]]<-new(FMToutputsource,new(FMTspec),new(FMTmask,"? ? ?", themes),FMTotar$actual,"YVTOT", "ACT", 0, -1)
	outputs[[1]]<-new(FMToutput,"OVOLTOTREC","VOLUME RECOLTE",sources,operators)
	},
	error = function(e){ 
			print("Problème suivant lors de la création de la section output")
			print(e$message)
			quit(save="ask")
		})
#Creation de l'objectif et des contraintes
constraints <- list()
tryCatch(
	{
	#Objective
	constraints[[1]] <- new(FMTconstraint,FMTconstrainttype$FMTMAXMINobjective,outputs[[1]])
	constraints[[1]]$setlength(1,20)
	#even flow
	constraints[[2]] <- new(FMTconstraint,FMTconstrainttype$FMTevenflow,outputs[[1]])
	constraints[[2]]$setlength(1,20)
	},
	error = function(e){ 
			print("Problème suivant lors de la création de la section optimize")
			print(e$message)
			quit(save="ask")
	})
#Remplir le model deja existant
model$setname("TEST")
model$setthemes(themes)
model$setyields(yields)
model$setarea(area)
model$setlifespan(lifespan)
model$settransitions(transitions)
model$setactions(actions)
model$setoutputs(outputs)
model$setconstraints(constraints)
#Nombre de périodes du modèle
model$setintparameter(FMTintmodelparameters$LENGTH,10)
mparser <- new(FMTmodelparser)
#Écriture du modèle
mparser$write(model,"tests/")
#Résolution du modèle
emptyschedules<-list()
model$doplanning(TRUE,emptyschedules)
