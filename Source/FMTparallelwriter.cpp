/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTparallelwriter.hpp"
#include "FMTmodel.hpp"

#ifdef FMTWITHGDAL
	#include "gdal.h"
	#include "gdal_priv.h"
#endif 

namespace Parallel
{

	FMTparallelwriter::FMTparallelwriter(const std::string& location,
		const std::string& driver,
		const std::vector<Core::FMToutput>& outputs,
		const Models::FMTmodel& model) :
		outputstowrite(outputs),
		resultsdataset(createOGRdataset(location, driver)),
		resultslayer(createresultslayer(model,resultsdataset)),
		mtx()

	{

	}
	void FMTparallelwriter::write(const std::unique_ptr<Models::FMTmodel>& modelptr,
		const int& firstperiod, const int& lastperiod, const int& iteration) const
	{
		boost::lock_guard<boost::recursive_mutex> lock(mtx);
		try {
			if (modelptr)
				{
				const std::map<std::string, std::vector<std::vector<double>>> outputvalues = modelptr->getoutputsfromperiods(outputstowrite, firstperiod, lastperiod);
				writefeatures(resultslayer, firstperiod, iteration, outputstowrite, outputvalues);
			}else {
				fillupinfeasibles(resultslayer, outputstowrite, iteration, firstperiod, lastperiod);
				}
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTparallelwriter::write", __LINE__, __FILE__);
		}

	}

}