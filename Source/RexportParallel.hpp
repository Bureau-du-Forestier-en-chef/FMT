/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef REXPORTPARALLEL_H_INCLUDED
#define REXPORTPARALLEL_H_INCLUDED

#include "FMTtask.hpp"
#include "FMTtaskhandler.hpp"
#include "FMTreplanningtask.hpp"
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
			.constructor("@DocString(FMTreplanningtask())")
			.constructor<const Models::FMTmodel&,
						const Models::FMTmodel&,
						const Models::FMTmodel&,
						std::string,
						std::string,
						std::vector<std::string>,
						Core::FMToutputlevel>("@DocString(Parallel::FMTreplanningtask())")
			.method("setreplicates", &Parallel::FMTreplanningtask::setreplicates,
				"@DocString(FMTreplanningtask::setreplicates)")
			.method("setreplanningperiods", &Parallel::FMTreplanningtask::setreplanningperiods,
				"@DocString(FMTreplanningtask::setreplanningperiods)")
			.method("setglobalweight", &Parallel::FMTreplanningtask::setglobalweight,
				"@DocString(FMTreplanningtask::setglobalweight)");

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