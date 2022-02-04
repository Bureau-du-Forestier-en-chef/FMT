library(FMT)
if (new(FMTversion)$hasfeature("OSI"))
	{
	newparser<-new(FMTmodelparser)
	modelslist<-newparser$readproject("../Models/TWD_land/TWD_land.pri",c("Globalreplanning","Globalfire","Localreplanning"),TRUE,TRUE,TRUE)
    globalmodel <- new(FMTlpmodel, modelslist[[1]], FMTsolverinterface$CLP)
    globalmodel$setintparameter(FMTintmodelparameters$LENGTH, 10)
    globalmodel$setintparameter(FMTintmodelparameters$NUMBER_OF_THREADS, 1)
    stochastic <- new(FMTnssmodel, modelslist[[1]], 0)
    stochastic$setintparameter(FMTintmodelparameters$LENGTH,1)
    localmodel <- new(FMTlpmodel, modelslist[[3]], FMTsolverinterface$CLP)
    localmodel$setintparameter(FMTintmodelparameters$LENGTH, 1)
    localmodel$setintparameter(FMTintmodelparameters$NUMBER_OF_THREADS, 1)
    if (!file.exists("tests"))
        {
        dir.create("tests")
        }
    outputlocation <- "tests/replanning"
    layersoptions <- c("SEPARATOR=SEMICOLON")
    replanningtask <- new(FMTreplanningtask, globalmodel, stochastic, localmodel, outputlocation,"CSV",layersoptions,FMToutputlevel$totalonly)
    replanningtask$setreplicates(10)
    replanningtask$setreplanningperiods(10)
    replanningtask$setglobalweight(0)
    handler <- new(FMTtaskhandler,replanningtask, 2)
    rm(replanningtask)
    handler$setquietlogger()
    handler$conccurentrun()
    rm(handler)
    gc()
	}else{
	print("FMT needs to be compiled with OSI")
	}
