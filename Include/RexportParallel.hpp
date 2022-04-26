/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef REXPORTPARALLEL_H_INCLUDED
#define REXPORTPARALLEL_H_INCLUDED

#include "FMTtask.hpp"
#include "FMTtaskhandler.hpp"
#include "FMTreplanningtask.hpp"
#include "FMTplanningtask.hpp"
#include "FMTopareaschedulertask.hpp"
#include "FMTmodel.hpp"
#include "FMToutput.hpp"
#include "FMTutility.hpp"
#include <Rcpp.h>
#include <vector>

RCPP_EXPOSED_WRAP(Parallel::FMTtask);
RCPP_EXPOSED_AS(Parallel::FMTtask);
RCPP_DEFINEVECTOR(Parallel::FMTtask);//For vector

RCPP_EXPOSED_WRAP(Parallel::FMTreplanningtask);
RCPP_EXPOSED_AS(Parallel::FMTreplanningtask);
RCPP_DEFINEVECTOR(Parallel::FMTreplanningtask);//For vector

RCPP_EXPOSED_WRAP(Parallel::FMTplanningtask);
RCPP_EXPOSED_AS(Parallel::FMTplanningtask);
RCPP_DEFINEVECTOR(Parallel::FMTplanningtask);//For vector

RCPP_EXPOSED_WRAP(Parallel::FMTopareaschedulertask);
RCPP_EXPOSED_AS(Parallel::FMTopareaschedulertask);
RCPP_DEFINEVECTOR(Parallel::FMTopareaschedulertask);//For vector

RCPP_EXPOSED_WRAP(Parallel::FMTtaskhandler);
RCPP_EXPOSED_AS(Parallel::FMTtaskhandler);
RCPP_DEFINEVECTOR(Parallel::FMTtaskhandler);//For vector


namespace R
{

	void exportParallel()
	{

		Rcpp::class_<Parallel::FMTtask>("FMTtask", "@DocString(FMTtask)")
			.derives<Core::FMTobject>("FMTobject")
			.constructor("@DocString(FMTtask())");

		Rcpp::class_<Parallel::FMTreplanningtask>("FMTreplanningtask", "@DocString(FMTreplanningtask)")
			.derives<Parallel::FMTtask>("FMTtask")
			.constructor("@DocString(FMTreplanningtask::FMTreplanningtask(...))")
			.constructor<const Models::FMTmodel&,
			const Models::FMTmodel&,
			const Models::FMTmodel&,
			std::string,
			std::string,
			std::vector<std::string>,
			Core::FMToutputlevel>("@DocString(FMTreplanningtask())")
			.method("setreplicates", &Parallel::FMTreplanningtask::setreplicates,
				"@DocString(FMTreplanningtask::setreplicates)")
			.method("setreplanningperiods", &Parallel::FMTreplanningtask::setreplanningperiods,
				"@DocString(FMTreplanningtask::setreplanningperiods)");


		Rcpp::class_<Parallel::FMTplanningtask>("FMTplanningtask", "@DocString(FMTplanningtask)")
			.derives<Parallel::FMTtask>("FMTtask")
			.constructor("@DocString(FMTplanningtask::FMTplanningtask())")
			.constructor<int,
			int,
			std::string,
			std::string,
			std::vector<std::string>,
			Core::FMToutputlevel,
			std::string>("@DocString(FMTplanningtask::FMTplanningtask(...))")
			.method("push_back", &Parallel::FMTplanningtask::push_back,
				"@DocString(FMTplanningtask::push_back");

		Rcpp::class_<Parallel::FMTopareaschedulertask>("FMTopareaschedulertask", "@DocString(FMTopareaschedulertask)")
			.derives<Parallel::FMTtask>("FMTtask")
			.constructor("@DocString(FMTopareaschedulertask::FMTopareaschedulertask())")
			.constructor<Models::FMTlpmodel,
			std::vector<Heuristics::FMToperatingareascheme>,
			Core::FMToutputnode,std::string,
			std::string,unsigned int,double>("@DocString(FMTopareaschedulertask::FMTopareaschedulertask(...))");


		Rcpp::class_<Parallel::FMTtaskhandler>("FMTtaskhandler", "@DocString(FMTtaskhandler)")
			.derives<Core::FMTobject>("FMTobject")
			.constructor("@DocString(FMTtaskhandler())")
			.constructor<const Parallel::FMTtask&,unsigned int>("@DocString(FMTtaskhandler(const Parallel::FMTtask&,unsigned int))")
			.method("conccurentrun", &Parallel::FMTtaskhandler::conccurentrun,
				"@DocString(FMTtaskhandler::conccurentrun)")
			.method("ondemandrun", &Parallel::FMTtaskhandler::ondemandrun,
				"@DocString(FMTtaskhandler::ondemandrun)");

	}
}
#endif // PYEXPORTPARALLEL_H_INCLUDED
