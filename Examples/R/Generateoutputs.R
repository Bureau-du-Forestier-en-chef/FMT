library(FMT)
#Les exceptions � ignorer
tryCatch({
    if (new(FMTversion)$hasfeature("OSI")) {
        newparser <- new(FMTmodelparser)
        exceptions<-c(FMTexc$FMTmissingyield,
            FMTexc$FMToutput_missing_operator,
            FMTexc$FMToutput_too_much_operator,
            FMTexc$FMTinvalidyield_number,
            FMTexc$FMTundefinedoutput_attribute,
            FMTexc$FMToveridedyield, 
            FMTexc$FMTdeathwithlock,
            FMTexc$FMTinvalid_geometry)
        #On applique les exceptions au parser
        newparser$seterrorstowarnings(exceptions)
        primarylocation <- "C:/Users/Admlocal/Documents/SCRAP/01171/PC_7719_U01171_4_2018_VPF04.pri"
        modelslist <- newparser$readproject(primarylocation, c("scenario_4"), TRUE, TRUE, TRUE)
        lpmodel <- new(FMTlpmodel, modelslist[[1]], FMTsolverinterface$MOSEK)
        tolerance <- 0.01
        schedules <- newparser$readschedules(primarylocation, modelslist)[[1]]
        
        lpmodel$setdblparameter(FMTdblmodelparameters$TOLERANCE, 0.01)
        lpmodel$setboolparameter(FMTboolmodelparameters$FORCE_PARTIAL_BUILD, TRUE)

        for (period in 1:length(schedules)) {
            print(lpmodel$buildperiod(schedules[[period]], TRUE, 1)$str())
        }
        
        for (period in 1:length(schedules)) {
            if (lpmodel$setsolution(period, schedules[[period]], tolerance)) {
                print(paste("Solution set at period", period))
            }
        }
        lpmodel$doplanning(FALSE, schedules)
        outputstocalculate <- list()
        for (output in lpmodel$getoutputs()) {
            if (all(output$getname() == "OVOLTOTREC") || all(output$getname() == "OSUPREC")) {
                outputstocalculate[[length(outputstocalculate) + 1]] <- output
            }
        }
        
        outdataframe <- lpmodel$getoutputsdataframe(outputstocalculate, 1, length(schedules))
        aggregatesdata <- list()
        attributesdata <- list()
        
        for (theme in lpmodel$getthemes()) {
            aggregatesdata[[length(aggregatesdata) + 1]] <- theme$getaggregatesasdataframe()
            attributesdata[[length(attributesdata) + 1]] <- theme$getattributesasdataframe()
        }
    } else {
        print("FMT needs to be compiled with OSI")
    }
}, error = function(e) {
    message("An error occurred: ", e$message)
})