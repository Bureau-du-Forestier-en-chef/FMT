library(FMT)
if (new(FMTversion)$hasfeature("OSI"))
	{
	newparser<-new(FMTmodelparser)
	modelslist<-newparser$readproject("../Models/TWD_land/TWD_land.pri",c("LP"),TRUE,TRUE,TRUE)
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
	print(paste0("Objective value of ",lpmodel$initialsolve()))
	}else{
	print("FMT needs to be compiled with OSI")
	}
