/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#ifdef FMTWITHOSI

#ifndef FMTlpheuristicmthandler_H_INCLUDED
#define FMTlpheuristicmthandler_H_INCLUDED

#include "FMTlpheuristic.h"
#include "FMToperatingareascheduler.h"
#include "FMToperatingareaclusterer.h"
#if defined FMTWITHPYTHON
	#include "boost/python.hpp"
#endif


namespace Heuristics
{

	class FMTlpheuristicmthandler
		{
		private:
		std::vector<FMTlpheuristic*> heuristics;
		public:
		FMTlpheuristicmthandler();
		FMTlpheuristicmthandler(std::vector<FMToperatingareascheduler>& lheuristics);
		FMTlpheuristicmthandler(std::vector<FMToperatingareaclusterer>& lheuristics);
		#if defined FMTWITHPYTHON
			FMTlpheuristicmthandler(boost::python::list& lheuristics);
		#endif
		size_t initialsolve() const;
	    };
}
BOOST_CLASS_EXPORT_KEY(Heuristics::FMTlpheuristicmthandler)
#endif
#endif
