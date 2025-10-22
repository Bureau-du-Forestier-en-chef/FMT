/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTdebugexceptionhandler_Hm_included
#define FMTdebugexceptionhandler_Hm_included
#include "FMTexceptionhandler.h"
#include <string>


namespace Exception
{
	// DocString: FMTdebugexceptionhandler
	/**
	The FMTdebugexceptionhandler is derived from the FMTexceptionhandler base class and is usefull when
	you want to know in which line/file of the source code the exception is comming from.
	*/
	class FMTEXPORT FMTdebugexceptionhandler final : public FMTexceptionhandler
	{
	public:
		// DocString: FMTdebugexceptionhandler()
		/**
		Default constructor for FMTdebugexceptionhandler.
		*/
		FMTdebugexceptionhandler();
		// DocString: ~FMTdebugexceptionhandler()
		/**
		Default destructor for FMTdebugexceptionhandler.
		*/
		~FMTdebugexceptionhandler() = default;
		// DocString: FMTdebugexceptionhandler::raise
		/**
		The function overide the base class raise function to give it a more "debug" style.
		See raise function of FMTexceptionhandler class.
		*/
		FMTexception raise(FMTexc lexception, std::string text,
			const std::string& method, const int& line, const std::string& file,
			Core::FMTsection lsection = Core::FMTsection::Empty, bool throwit = true) override;
	#if defined FMTWITHGDAL
        // DocString: FMTdebugexceptionhandler::getCPLdata
		/**
		Used in the handelCPLerror called back by GDAL function reutnr a abstract copy of itselft.
		See getCPLdata of FMTexceptionhandler class.
		*/
		FMTexceptionhandler* getCPLdata() override;
		// DocString: FMTdebugexceptionhandler::handelCPLerror
		/**
		Used has call back in gdal.
		See handelCPLerror of FMTexceptionhandler class.
		*/
		void handelCPLerror(int eErrClass, int nError, const char * pszErrorMsg) override;
	#endif
		// DocString: FMTdebugexceptionhandler::Clone
		/**
		@brief clone the FMTdebugexceptionhandler
		@return a valid cloned FMTdebugexceptionhandler
		*/
		virtual std::unique_ptr <FMTexceptionhandler> Clone() const;
	private:
		// DocString: FMTdebugexceptionhandler::serialize
		/**
		Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& boost::serialization::make_nvp("FMTexceptionhandler", boost::serialization::base_object<FMTexceptionhandler>(*this));
		}
	};

}

BOOST_CLASS_EXPORT_KEY(Exception::FMTdebugexceptionhandler)

#endif

