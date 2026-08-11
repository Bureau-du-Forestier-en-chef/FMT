#ifndef FMTWIN32Error_included
#define FMTWIN32Error_included

#include "FMTError.h"
#include "FMTutility.h"
#include "FMTException.h"
#include <string>
#include <boost/serialization/export.hpp>


namespace Exception
	{
	#if defined _MSC_VER
		class FMTSeException;
	#endif
	class FMTEXPORT FMTWIN32Error: public FMTError
		{
			public:
				// DocString: FMTWIN32Error(Core::FMTsection,const std::string&,const std::string&,const std::string&,int)
				/**
				@brief Construct a FMTWIN32Error from a section, a message and the location where it occurred.
				@param[in] p_section the section in which the error occurred.
				@param[in] p_message the message of the error.
				@param[in] p_method the method where the error occurred.
				@param[in] p_file the file where the error occurred.
				@param[in] p_line the line where the error occurred.
				*/
				FMTWIN32Error(Core::FMTsection p_section,
					const std::string& p_message,
					const std::string& p_method,
					const std::string& p_file,
					int p_line);

				// DocString: FMTWIN32Error(const FMTSeException& p_seError)
				/**
				@brief Construct a FMTWIN32Error from a seError
				@param[in] p_seError the se error
				*/
				#if defined _MSC_VER
					FMTWIN32Error(const FMTSeException& p_seError);
				#endif
			private:
				// DocString: FMTWIN32Error::serialize
				/**
				@brief Serialize the FMTWIN32Error through its base FMTError for multiprocessing across multiple cpus (pickle in Python).
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