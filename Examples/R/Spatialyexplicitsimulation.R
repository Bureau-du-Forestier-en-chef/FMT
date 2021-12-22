library(FMT)
if (new(FMTversion)$hasfeature("GDAL") && new(FMTversion)$hasfeature("OSI"))
	{
	newparser<-new(FMTmodelparser)
    modelslist <- newparser$readproject("../Models/TWD_land/TWD_land.pri", c("Spatial"), TRUE, TRUE, TRUE)
    lpmodel <- new(FMTlpmodel, modelslist[[1]], FMTsolverinterface$CLP)
	emptyschedule<-new(FMTschedule)
	for (period in 1:10)
		{
		print(lpmodel$buildperiod(emptyschedule,FALSE,1)$str())
		}
	allmodelconstraints<-lpmodel$getconstraints()
	modelobjective<-allmodelconstraints[[1]]
	modelconstraints<-allmodelconstraints[2:length(allmodelconstraints)]
	for (constraint in modelconstraints)
		{
		print(lpmodel$setconstraint(constraint)$str())
		}
    print(lpmodel$setobjective(modelobjective)$str())
    if (lpmodel$initialsolve())
        {
        simulationmodel <- new(FMTsesmodel,lpmodel)
        singletransitions <- list()
        for (transition in simulationmodel$gettransitions())
            {
            singletransitions[[length(singletransitions) + 1]] <- transition$single()
            }
        simulationmodel$settransitions(singletransitions)
        areaparser <- new(FMTareaparser)
        rasterslocation <- "../Models/TWD_land/rasters"
        ageraster <- file.path(rasterslocation,"AGE.tif")
        themesrasters <- list()
        themeid <- 1
        for (theme in simulationmodel$getthemes())
            {
            themesrasters[[length(themesrasters) + 1]] <- file.path(rasterslocation, paste("THEME",paste0(themeid),".tif",sep = ""))
            themeid<-themeid+1
            }
        initialforestmap <- areaparser$readrasters(simulationmodel$getthemes(), unlist(themesrasters), ageraster, 1, 0.0001,"")
        simulationmodel$setinitialmapping(initialforestmap)
        #Stop aftet 10 search and no decrease into infeasibility and objective
        for (period in 1:10)
            {
            print(simulationmodel$greedyreferencebuild(lpmodel$getsolution(period,FALSE), 10,0,0.0001))
            }
        }

	}else{
	print("FMT needs to be compiled with OSI OR GDAL")
	}
