/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTfreeexceptionhandler_H_INCLUDED
#define FMTfreeexceptionhandler_H_INCLUDED

#include "FMTexceptionhandler.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <string>

namespace Exception
{
// DocString: FMTfreeexceptionhandler
/**
The FMTfreeexceptionhandler is derived from the FMTexceptionhandler base class.
This class is realy usefull if you want to pass the exception thrown by FMT directly to the user,
So he can do whatever he wants to do with it.
*/
class FMTfreeexceptionhandler final : public FMTexceptionhandler
{
	// DocString: FMTfreeexceptionhandler::serialize
	/**
	Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("FMTexceptionhandler", boost::serialization::base_object<FMTexceptionhandler>(*this));
	}
public:
	// DocString: FMTfreeexceptionhandler()
	/**
	Default constructor for FMTfreeexceptionhandler
	*/
	FMTfreeexceptionhandler();
	// DocString: ~FMTfreeexceptionhandler()
	/**
	Default destructor for FMTfreeexceptionhandler
	*/
	~FMTfreeexceptionhandler() = default;
	// DocString: FMTfreeexceptionhandler::raise
	/**
	The function overide the base class raise function.
	See raise function of FMTexceptionhandler class.
	*/
	FMTexception raise(FMTexc lexception, std::string text,
		const std::string& method, const int& line, const std::string& file,
		Core::FMTsection lsection = Core::FMTsection::Empty, bool throwit = true) override;
#if defined FMTWITHGDAL
	// DocString: FMTfreeexceptionhandler::getCPLdata
	/**
	Used in the handelCPLerror called back by GDAL function reutnr a abstract copy of itselft.
	See getCPLdata of FMTexceptionhandler class.
	*/
	FMTexceptionhandler* getCPLdata() override;
	// DocString: FMTfreeexceptionhandler::handelCPLerror
	/**
	Used has call back in gdal.
	See handelCPLerror of FMTexceptionhandler class.
	*/
	void handelCPLerror(CPLErr eErrClass, CPLErrorNum nError, const char * pszErrorMsg) override;
#endif
};
}

BOOST_CLASS_EXPORT_KEY(Exception::FMTfreeexceptionhandler)

#endif

