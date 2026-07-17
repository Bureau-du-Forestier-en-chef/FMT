/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTfreeexceptionhandler_Hm_included
#define FMTfreeexceptionhandler_Hm_included

#include "FMTExceptionHandler.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <string>

namespace Exception
{
// DocString: FMTFreeExceptionHandler
/**
The FMTFreeExceptionHandler is derived from the FMTExceptionHandler base class.
This class is realy usefull if you want to pass the exception thrown by FMT directly to the user,
So he can do whatever he wants to do with it.
*/
class FMTEXPORT FMTFreeExceptionHandler final : public FMTExceptionHandler
{
public:
	// DocString: FMTFreeExceptionHandler()
	/**
	Default constructor for FMTFreeExceptionHandler
	*/
	FMTFreeExceptionHandler();
	// DocString: ~FMTFreeExceptionHandler()
	/**
	Default destructor for FMTFreeExceptionHandler
	*/
	~FMTFreeExceptionHandler() = default;
	// DocString: FMTFreeExceptionHandler::raise
	/**
	The function overide the base class raise function.
	See raise function of FMTExceptionHandler class.
	*/
	FMTException raise(FMTexc lexception, std::string text,
		const std::string& method, const int& line, const std::string& file,
		Core::FMTsection lsection = Core::FMTsection::Empty, bool throwit = true) override;
#if defined FMTWITHGDAL
	// DocString: FMTFreeExceptionHandler::getCPLdata
	/**
	Used in the handelCPLerror called back by GDAL function reutnr a abstract copy of itselft.
	See getCPLdata of FMTExceptionHandler class.
	*/
	FMTExceptionHandler* getCPLdata() override;
	// DocString: FMTFreeExceptionHandler::handelCPLerror
	/**
	Used has call back in gdal.
	See handelCPLerror of FMTExceptionHandler class.
	*/
	void handelCPLerror(int eErrClass, int nError, const char * pszErrorMsg) override;
#endif
	// DocString: FMTFreeExceptionHandler::Clone
	/**
	@brief clone the FMTFreeExceptionHandler
	@return a valid cloned FMTFreeExceptionHandler
	*/
	virtual std::unique_ptr <FMTExceptionHandler> Clone() const;
private:
	// DocString: FMTFreeExceptionHandler::serialize
	/**
	Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& boost::serialization::make_nvp("FMTexceptionhandler", boost::serialization::base_object<FMTExceptionHandler>(*this));
	}
};
}

BOOST_CLASS_EXPORT_KEY(Exception::FMTFreeExceptionHandler)

#endif

