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
	@brief Callback used in GDAL for handling GDAL errors.
	@param[in] eErrClass the error class.
	@param[in] nError the error number.
	@param[in] pszErrorMsg the error message.
	*/
	#if defined FMTWITHGDAL
		void CPL_STDCALL FMTCPLErrorHandler(CPLErr eErrClass, CPLErrorNum nError, const char * pszErrorMsg);
	#endif
}
#endif