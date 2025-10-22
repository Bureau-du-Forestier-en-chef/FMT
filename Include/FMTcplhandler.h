/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTcplhandler_Hm_included
#define FMTcplhandler_Hm_included

#if defined FMTWITHGDAL
	#include "cpl_error.h"
#endif

namespace Exception
{
	// DocString: FMTCPLErrorHandler
	/**
	This function is used by has a callback in GDAL for handling GDAL errors.
	*/
	#if defined FMTWITHGDAL
		void CPL_STDCALL FMTCPLErrorHandler(CPLErr eErrClass, CPLErrorNum nError, const char * pszErrorMsg);
	#endif
}
#endif