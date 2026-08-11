#ifndef FMTBoostGraphError_included
#define FMTBoostGraphError_included

#include "FMTError.h"
#include "FMTutility.h"
#include "FMTException.h"
#include <string>
#include <boost/serialization/export.hpp>
#include <boost/graph/exception.hpp>

namespace Exception
	{
	class FMTEXPORT FMTBoostGraphError: public FMTError
		{
			public:
				// DocString: FMTBoostGraphError(Core::FMTsection,const std::string&,const std::string&,const std::string&,int)
				/**
				@brief Construct a FMTBoostGraphError from a section, a message and the location where it occurred.
				@param[in] p_section the section in which the error occurred.
				@param[in] p_message the message of the error.
				@param[in] p_method the method where the error occurred.
				@param[in] p_file the file where the error occurred.
				@param[in] p_line the line where the error occurred.
				*/
				FMTBoostGraphError(Core::FMTsection p_section,
					const std::string& p_message,
					const std::string& p_method,
					const std::string& p_file,
					int p_line);

				// DocString: FMTBoostGraphError(const boost::bad_graph&)
				/**
				@brief Construct a FMTBoostGraphError from a section, a message and the location where it occurred.
				@param[in] p_GraphError the boost graph error
				*/
				FMTBoostGraphError(const boost::bad_graph& p_GraphError);
			private:
				// DocString: FMTBoostGraphError::serialize
				/**
				@brief Serialize the FMTBoostGraphError through its base FMTError for multiprocessing across multiple cpus (pickle in Python).
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
							boost::serialization::base_object<FMTError>(*this));
					}
		};
	}

#endif