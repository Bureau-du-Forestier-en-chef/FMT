/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTcplhandler.hpp"
#include "FMTexceptionhandler.hpp"

namespace Exception
{
	// DocString: FMTCPLErrorHandler
	/**
	This function is used by has a callback in GDAL for handling GDAL errors.
	*/
#if defined FMTWITHGDAL
	void CPL_STDCALL FMTCPLErrorHandler(CPLErr eErrClass, CPLErrorNum nError, const char * pszErrorMsg)
	{
		//CPLErr eErrClass, CPLErrorNum nError,
		FMTexceptionhandler* handler = reinterpret_cast<FMTexceptionhandler*>(CPLGetErrorHandlerUserData());
		if (handler)
		{
			handler->handelCPLerror(eErrClass, nError, pszErrorMsg);
		}
	}
#endif

	
}