pathtopackage<-"C:\\MrnMicro\\Applic\\rtools40\\home\\FMT"
Sys.setenv(PATH=paste(Sys.getenv("PATH"),pathtopackage,sep=";"))
library(Rcpp)
FMT<-Module("FMT",dyn.load(paste(pathtopackage,"\\FMT.dll",sep="")))
newparser<-new(FMT$FMTmodelparser)
modelslist<-newparser$readproject("D:/MrnMicro/PC_9135_U09751_4_Vg2_2023_vRP1/PC_9135_U09751_4_Vg2_2023_vRP1.pri",c("03_Sc1b_Regl_prov_avsp_bfecopt"),TRUE,TRUE,TRUE)
solverinterface<-list(CLP=1,MOSEK=2,CPLEX=3,GUROBI=4)
lpmodel<-new(FMT$FMTlpmodel,modelslist[[1]],solverinterface$MOSEK)
emptyschedule<-new(FMT$FMTschedule)
for (period in 1:10)
	{
	print(lpmodel$buildperiod(emptyschedule,FALSE)$str())
	}
allmodelconstraints<-lpmodel$getconstraints()
modelobjective<-allmodelconstraints[[1]]
print(lpmodel$setobjective(modelobjective)$str())
modelconstraints<-allmodelconstraints[2:length(allmodelconstraints)]
for (constraint in modelconstraints)
	{
	print(lpmodel$setconstraint(constraint)$str())
	}
lpmodel$initialsolve()
