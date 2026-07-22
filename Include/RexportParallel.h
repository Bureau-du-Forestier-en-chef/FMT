/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef REXPORTPARALLEL_H_INCLUDED
#define REXPORTPARALLEL_H_INCLUDED

#include "FMTTask.h"
#include "FMTTaskHandler.h"
#include "FMTReplanningTask.h"
#include "FMTPlanningTask.h"
#if defined FMTWITHOSI
	#include "FMTOpAreaSchedulerTask.h"
#endif
#include "FMTModel.h"
#include "FMTOutput.h"
#include "FMTutility.h"
#include <Rcpp.h>
#include <vector>

RCPP_EXPOSED_WRAP(Parallel::FMTTask);
RCPP_EXPOSED_AS(Parallel::FMTTask);
RCPP_DEFINEVECTOR(Parallel::FMTTask);//For vector

RCPP_EXPOSED_WRAP(Parallel::FMTReplanningTask);
RCPP_EXPOSED_AS(Parallel::FMTReplanningTask);
RCPP_DEFINEVECTOR(Parallel::FMTReplanningTask);//For vector

RCPP_EXPOSED_WRAP(Parallel::FMTPlanningTask);
RCPP_EXPOSED_AS(Parallel::FMTPlanningTask);
RCPP_DEFINEVECTOR(Parallel::FMTPlanningTask);//For vector
#if defined FMTWITHOSI
	RCPP_EXPOSED_WRAP(Parallel::FMTOpAreaSchedulerTask);
	RCPP_EXPOSED_AS(Parallel::FMTOpAreaSchedulerTask);
	RCPP_DEFINEVECTOR(Parallel::FMTOpAreaSchedulerTask);//For vector
#endif

RCPP_EXPOSED_WRAP(Parallel::FMTTaskHandler);
RCPP_EXPOSED_AS(Parallel::FMTTaskHandler);
RCPP_DEFINEVECTOR(Parallel::FMTTaskHandler);//For vector


namespace R
{

	void exportParallel()
	{

		Rcpp::class_<Parallel::FMTTask>("FMTtask", "@DocString(FMTTask)")
			.derives<Core::FMTObject>("FMTobject")
			.constructor("@DocString(FMTTask())");

		Rcpp::class_<Parallel::FMTReplanningTask>("FMTreplanningtask", "@DocString(FMTReplanningTask)")
			.derives<Parallel::FMTTask>("FMTtask")
			.constructor("@DocString(FMTReplanningTask::FMTReplanningTask(...))")
			.constructor<const Models::FMTModel&,
			const Models::FMTModel&,
			const Models::FMTModel&,
			std::string,
			std::string,
			std::vector<std::string>,
			Core::FMToutputlevel>("@DocString(FMTReplanningTask())")
			.method("setreplicates", static_cast<void(Parallel::FMTReplanningTask::*)(const int&)>(&Parallel::FMTReplanningTask::setReplicates),"@DocString(FMTReplanningTask::setreplicates)")
			.method("setreplanningperiods", &Parallel::FMTReplanningTask::setReplanningPeriods,
				"@DocString(FMTReplanningTask::setreplanningperiods)");


		Rcpp::class_<Parallel::FMTPlanningTask>("FMTplanningtask", "@DocString(FMTPlanningTask)")
			.derives<Parallel::FMTTask>("FMTtask")
			.constructor("@DocString(FMTPlanningTask::FMTPlanningTask())")
			.constructor<int,
			int,
			std::string,
			std::string,
			std::vector<std::string>,
			Core::FMToutputlevel,
			std::string>("@DocString(FMTPlanningTask::FMTPlanningTask(...))")
			.method("push_back", &Parallel::FMTPlanningTask::push_back,
				"@DocString(FMTPlanningTask::push_back");
#if defined FMTWITHOSI
		Rcpp::class_<Parallel::FMTOpAreaSchedulerTask>("FMTopareaschedulertask", "@DocString(FMTOpAreaSchedulerTask)")
			.derives<Parallel::FMTTask>("FMTtask")
			.constructor("@DocString(FMTOpAreaSchedulerTask::FMTOpAreaSchedulerTask())")
			.constructor<Models::FMTLpModel,
			std::vector<Heuristics::FMTOperatingAreaScheme>,
			Core::FMTOutputNode,std::string,
			std::string,unsigned int,double>("@DocString(FMTOpAreaSchedulerTask::FMTOpAreaSchedulerTask(...))");
#endif

		Rcpp::class_<Parallel::FMTTaskHandler>("FMTtaskhandler", "@DocString(FMTTaskHandler)")
			.derives<Core::FMTObject>("FMTobject")
			.constructor("@DocString(FMTTaskHandler())")
			.constructor<const Parallel::FMTTask&,unsigned int>("@DocString(FMTTaskHandler(const Parallel::FMTTask&,unsigned int))")
			.method("conccurentrun", &Parallel::FMTTaskHandler::conccurentRun,
				"@DocString(FMTTaskHandler::conccurentrun)")
			.method("ondemandrun", &Parallel::FMTTaskHandler::onDemandRun,
				"@DocString(FMTTaskHandler::ondemandrun)");

	}
}
#endif // PYEXPORTPARALLEL_H_INCLUDED
