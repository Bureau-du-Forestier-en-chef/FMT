#ifndef FMTSchemeFailed_included
#define FMTSchemeFailed_included

#include "FMTWarning.h"
#include "FMTutility.h"
#include "FMTException.h"
#include <string>
#include <boost/serialization/export.hpp>

namespace Exception
	{
	class FMTEXPORT FMTSchemeFailed: public FMTWarning
		{
			public:
				// DocString: FMTSchemeFailed(Core::FMTsection,const std::string&,const std::string&,const std::string&,int)
				/**
				@brief Construct a FMTSchemeFailed from a section, a message and the location where it occurred.
				@param[in] p_section the section in which the error occurred.
				@param[in] p_message the message of the error.
				@param[in] p_method the method where the error occurred.
				@param[in] p_file the file where the error occurred.
				@param[in] p_line the line where the error occurred.
				*/
				FMTSchemeFailed(Core::FMTsection p_section,
					const std::string& p_message,
					const std::string& p_method,
					const std::string& p_file,
					int p_line);

			private:
				// DocString: FMTSchemeFailed::serialize
				/**
				@brief Serialize the FMTSchemeFailed through its base FMTWarning for multiprocessing across multiple cpus (pickle in Python).
				@tparam Archive the archive type.
				@param[in,out] ar the archive to serialize to or from.
				@param[in] version the serialization version.
				*/
				friend class boost::serialization::access;

				template<class Archive>
				void serialize(Archive& ar, const unsigned int version)
					{
						ar & boost::serialization::make_nvp(
							"parent_exception",
							boost::serialization::base_object<FMTWarning>(*this));
					}
		};
	}

#endif