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
	class FMTEXPORT FMTparallelwriter : public Parser::FMTmodelparser
	{
		std::vector<Core::FMToutput>outputstowrite;
		GDALDataset* resultsdataset;
		OGRLayer* resultslayer;
		mutable boost::recursive_mutex mtx;
	public:
		FMTparallelwriter() = default;
		FMTparallelwriter(const FMTparallelwriter& rhs) = default;
		FMTparallelwriter& operator = (const FMTparallelwriter& rhs) = default;
		FMTparallelwriter(const std::string& location,
			const std::string& driver,
			const std::vector<Core::FMToutput>& outputs,
			const Models::FMTmodel& model);
		void write(const std::unique_ptr<Models::FMTmodel>& modelptr, const int& firstperiod,
			const int& lastperiod, const size_t& itertion) const;
	};
}
#endif
