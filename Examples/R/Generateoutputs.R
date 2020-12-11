library(FMT)
if (new(FMTversion)$hasfeature("OSI"))
	{
    newparser <- new(FMTmodelparser)
    primarylocation<-"../Models/TWD_land/TWD_land.pri"
    modelslist <- newparser$readproject(primarylocation, c("LP"), TRUE, TRUE, TRUE)
	solverinterface<-list(CLP=1,MOSEK=2,CPLEX=3,GUROBI=4)
    lpmodel <- new(FMTlpmodel, modelslist[[1]], solverinterface$CLP)
    tolerance <- 0.0001
    schedules <- newparser$readschedules(primarylocation, modelslist)[[1]]
    for (period in 1:length(schedules))
		{
        print(lpmodel$buildperiod(schedules[[period]],TRUE)$str())
        }
    for (period in 1:length(schedules))
        {
        if (lpmodel$setsolution(period, schedules[[period]], tolerance))
            {
            print(paste("Solution set at period ",period))
            }
        }
    outputstocalculate <- list()
    for (output in lpmodel$getoutputs())
        {
        if (all(output$getname() == "OVOLREC") || all(output$getname() == "OSUPREC"))
            {
            outputstocalculate[[length(outputstocalculate) + 1]] <- output
            }
        }
    outdataframe <- lpmodel$getoutputsdataframe(outputstocalculate, 1, length(schedules))
	}else{
	print("FMT needs to be compiled with OSI")
	}
