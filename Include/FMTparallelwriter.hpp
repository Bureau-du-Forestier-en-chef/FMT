#ifndef FMTparallelwriter_H_INCLUDED
#define FMTparallelwriter_H_INCLUDED
#include "FMTmodelparser.hpp"
#include <vector>
#include <string>
#include <memory>
#include <boost/thread/recursive_mutex.hpp>
#include "FMTutility.hpp"

#ifdef FMTWITHGDAL
class GDALDataset;
class OGRLayer;
#endif

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
		#ifdef FMTWITHGDAL
		// DocString: FMTparallelwriter::resultsdataset
		///The dataset of the results.
		GDALDataset* resultsdataset;
		// DocString: FMTparallelwriter::resultslayer
		///The layer of the results. the key is the model name.
		std::map<std::string,OGRLayer*> resultslayer;
		// DocString: FMTparallelwriter::driftlayer
		///The drift layer probability.
		OGRLayer* driftlayer;
		#endif
		// DocString: FMTparallelwriter::mtx
		///The recursive mutex used to control the usage of the writer by the thread.
		mutable boost::recursive_mutex mtx;
		// DocString: FMTparallelwriter::resultsminimaldrift;
		///Used when writing drift probability layer
		double resultsminimaldrift;
		// DocString: FMTparallelwriter::outputslevel
		///The output level of detail
		Core::FMToutputlevel outputslevel;
		// DocString: FMTparallelwriter::alllayeroptions
		///The layers options
		std::vector<std::string> alllayeroptions;
		// DocString: FMTparallelwriter::outputfirstperiod
		///First period for the output value.
		int outputfirstperiod;
		// DocString: FMTparallelwriter::outputlastperiod;
		///Lastperiod for the output value.
		int outputlastperiod;
		// DocString: FMTparallelwriter::projectdirectory;
		///The directory of the primary file.
		std::string projectdirectory;
		// DocString: FMTparallelwriter::projectname;
		///The name of the project file
		std::string projectname;
	protected:
		// DocString: FMTparallelwriter::getdriftprobability()
		/**
		Calculate the drift probabilities.
		*/
		const std::map<std::string, std::map<double, std::vector<double>>>getdriftprobability(
			const std::map<std::string, std::vector<std::vector<double>>>& globalvalues,
			const std::map<std::string, std::vector<std::vector<double>>>& localvalues,
			const bool lower = true) const;
	public:
		// DocString: FMTparallelwriter::FMTparallelwriter()
		/**
		Default constructor for FMTparallelwriter.
		*/
		FMTparallelwriter() = default;
		// DocString: ~FMTparallelwrite()
		/**
		Default destructor for FMTparallelwrite.
		*/
		virtual ~FMTparallelwriter();
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
			const std::vector<Models::FMTmodel*>& allmodels,
			std::vector<std::string>layersoptions = std::vector<std::string>(),
			double minimaldrift = 0.5,
			Core::FMToutputlevel outputlevel = Core::FMToutputlevel::totalonly);
		// DocString: FMTparallelwriter::FMTparallelwriter(const std::string&,const std::string&,std::vector<std::string>,Core::FMToutputlevel)
		/**
		Constructor that will build the parser without layers and outputs.
		*/
		FMTparallelwriter(const std::string& location,
			const std::string& driver,
			Core::FMToutputlevel outputlevel = Core::FMToutputlevel::totalonly,
			std::vector<std::string>layersoptions = std::vector<std::string>(),
			int firstperiod=1,
			int lastperiod=1,
			std::string primaryfilelocation = std::string());
		// DocString: FMTparallelwriter::setlayer
		/**
		Add a new layer for a given model.
		*/
		void setlayer(const Models::FMTmodel* model);
		// DocString: FMTparallelwriter::write()
		/**
		Write the modelptr results from the firstperiod to the lastperiod for a given iteration (replicate).
		*/
		void write(const std::string& modelname,
			const std::map<std::string, std::vector<std::vector<double>>>& results,
			const int& firstperiod, const int& lastperiod, const int& iteration) const;
		// DocString: FMTparallelwriter::getresults()
		/**
		Get the results of a model.
		*/
		std::map<std::string, std::vector<std::vector<double>>> getresults(const std::unique_ptr<Models::FMTmodel>& modelptr, const int& firstperiod, const int& lastperiod) const;
		// DocString: FMTparallelwriter::setdriftprobability()
		/**
		Get the results of a model With the global model and the localmodel starting from a minimum drift proportion.
		*/
		void setdriftprobability(const std::string& globalmodel, const std::string& localmodel) const;
		// DocString: FMTparallelwriter::getandwrite()
		/**
		Get the results of a model. and write it down.
		*/
		void getandwrite(const std::unique_ptr<Models::FMTmodel>& modelptr, const std::vector<Core::FMToutput>& loutputs);
		};
}
#endif
