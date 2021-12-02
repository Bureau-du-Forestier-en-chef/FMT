#ifndef FMTparallelwriter_H_INCLUDED
#define FMTparallelwriter_H_INCLUDED
#include "FMTmodelparser.hpp"
#include <vector>
#include <string>
#include <memory>
#include <boost/thread/recursive_mutex.hpp>

class GDALDataset;
class OGRLayer;

namespace Models {
	class FMTmodel;
}


namespace Parallel
{
	// DocString: FMTparallelwriter
	/**
	Writer to use in concurrency when you want to write multiple results in the same layer.
	*/
	class FMTEXPORT FMTparallelwriter : public Parser::FMTmodelparser
	{
		// DocString: FMTparallelwriter::outputstowrite
		///Outputs that the writer will write for different models.
		std::vector<Core::FMToutput>outputstowrite;
		// DocString: FMTparallelwriter::resultsdataset
		///The dataset of the results.
		GDALDataset* resultsdataset;
		// DocString: FMTparallelwriter::resultslayer
		///The layer of the results.
		OGRLayer* resultslayer;
		// DocString: FMTparallelwriter::mtx
		///The recursive mutex used to control the usage of the writer by the thread.
		mutable boost::recursive_mutex mtx;
	public:
		// DocString: FMTparallelwriter::FMTparallelwriter()
		/**
		Default constructor for FMTparallelwriter.
		*/
		FMTparallelwriter() = default;
		// DocString: FMTparallelwriter::FMTparallelwriter(const FMTparallelwriter&)
		/**
		Default copy constructor for FMTparallelwriter.
		*/
		FMTparallelwriter(const FMTparallelwriter& rhs) = default;
		// DocString: FMTparallelwriter::operator=()
		/**
		Default copy assignement for FMTparallelwriter.
		*/
		FMTparallelwriter& operator = (const FMTparallelwriter& rhs) = default;
		// DocString: FMTparallelwriter::FMTparallelwriter(const std::string&,const std::string&,const std::vector<Core::FMToutput>&,const Models::FMTmodel&)
		/**
		Constructor with the location and the driver of the outputs and the base model that we want to write.
		*/
		FMTparallelwriter(const std::string& location,
			const std::string& driver,
			const std::vector<Core::FMToutput>& outputs,
			const Models::FMTmodel& model);
		// DocString: FMTparallelwriter::write()
		/**
		Write the modelptr results from the firstperiod to the lastperiod for a given iteration (replicate).
		*/
		void write(const std::unique_ptr<Models::FMTmodel>& modelptr, const int& firstperiod,
			const int& lastperiod, const int& itertion) const;
	};
}
#endif
