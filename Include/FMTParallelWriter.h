#ifndef FMTparallelwriter_Hm_included
#define FMTparallelwriter_Hm_included
#include "FMTModelParser.h"
#include <vector>
#include <string>
#include <memory>
#include <boost/thread/recursive_mutex.hpp>
#include "FMTutility.h"

#ifdef FMTWITHGDAL
class GDALDataset;
class OGRLayer;
#endif

namespace Models {
	class FMTModel;
}


namespace Parallel
{
	// DocString: FMTParallelWriter
	/**
	Writer to use in concurrency when you want to write multiple results in the same layer.
	*/
	class FMTEXPORT FMTParallelWriter : public Parser::FMTModelParser
	{
	public:
		// DocString: FMTParallelWriter::FMTParallelWriter()
		/**
		Default constructor for FMTParallelWriter.
		*/
		FMTParallelWriter() = default;
		// DocString: ~FMTparallelwrite()
		/**
		Default destructor for FMTparallelwrite.
		*/
		virtual ~FMTParallelWriter();
		// DocString: FMTParallelWriter::FMTParallelWriter(const FMTParallelWriter&)
		/**
		Default copy constructor for FMTParallelWriter.
		*/
		FMTParallelWriter(const FMTParallelWriter& rhs) = default;
		// DocString: FMTParallelWriter::operator=()
		/**
		Default copy assignement for FMTParallelWriter.
		*/
		FMTParallelWriter& operator = (const FMTParallelWriter& rhs) = default;
		// DocString: FMTParallelWriter::FMTParallelWriter(const std::string&,const std::string&,const std::vector<Core::FMTOutput>&,const Models::FMTModel&)
		/**
		Constructor with the location and the driver of the outputs and the base model that we want to write.
		*/
		FMTParallelWriter(const std::string& location,
			const std::string& driver,
			const std::vector<Core::FMTOutput>& outputs,
			const std::vector<Models::FMTModel*>& allmodels,
			std::vector<std::string>layersoptions = std::vector<std::string>(),
			double minimaldrift = 0.5,
			Core::FMToutputlevel outputlevel = Core::FMToutputlevel::totalonly);
		// DocString: FMTParallelWriter::FMTParallelWriter(const std::string&,const std::string&,std::vector<std::string>,Core::FMToutputlevel)
		/**
		Constructor that will build the parser without layers and outputs.
		*/
		FMTParallelWriter(const std::string& location,
			const std::string& driver,
			Core::FMToutputlevel outputlevel = Core::FMToutputlevel::totalonly,
			std::vector<std::string>layersoptions = std::vector<std::string>(),
			int firstPeriod=1,
			int lastPeriod=1,
			std::string primaryfilelocation = std::string());
		// DocString: FMTParallelWriter::setLayer
		/**
		@brief Add a new layer for a given model.
		@param[in] p_name the name of the layer.
		*/
		void setLayer(const std::string& p_name);
		// DocString: FMTParallelWriter::write()
		/**
		Write the modelptr results from the firstPeriod to the lastPeriod for a given iteration (replicate).
		*/
		void write(const std::string& modelname,
			const std::map<std::string, std::vector<std::vector<double>>>& results,
			const int& firstPeriod, const int& lastPeriod, const int& iteration) const;
		// DocString: FMTParallelWriter::getResults()
		/**
		Write a schedules.
		*/
		void writeSchedules(const std::string seqName, const std::vector<Core::FMTSchedule> scheduleList, bool append) const;
		// DocString: FMTParallelWriter::getResults()
		/**
		Get the results of a model.
		*/
		std::map<std::string, std::vector<std::vector<double>>> getResults(const std::unique_ptr<Models::FMTModel>& modelptr, const int& firstPeriod, const int& lastPeriod) const;
		// DocString: FMTParallelWriter::setDriftProbability()
		/**
		Get the results of a model With the global model and the localmodel starting from a minimum drift proportion.
		*/
		void setDriftProbability(const std::string& globalmodel, const std::string& localmodel) const;
		// DocString: FMTParallelWriter::getAndWrite()
		/**
		Get the results of a model. and write it down.
		*/
		void getAndWrite(const std::unique_ptr<Models::FMTModel>& modelptr, const std::vector<Core::FMTOutput>& loutputs);
		// DocString: FMTParallelWriter::close()
		/**
		Close the dataset and destroy the layers after this it wont be usable
		*/
		void close() noexcept;
		protected:
			// DocString: FMTParallelWriter::getDriftProbability()
			/**
			Calculate the drift probabilities.
			*/
			const std::map<std::string, std::map<double, std::vector<double>>>getDriftProbability(
				const std::map<std::string, std::vector<std::vector<double>>>& globalvalues,
				const std::map<std::string, std::vector<std::vector<double>>>& localvalues,
				const bool lower = true) const;
		private:
			// DocString: FMTParallelWriter::outputstowrite
			///Outputs that the writer will write for different models.
			std::vector<Core::FMTOutput>outputstowrite;
			#ifdef FMTWITHGDAL
					// DocString: FMTParallelWriter::resultsdataset
					///The dataset of the results.
					GDALDataset* resultsdataset;
					// DocString: FMTParallelWriter::resultslayer
					///The layer of the results. the key is the model name.
					std::map<std::string, OGRLayer*> resultslayer;
					// DocString: FMTParallelWriter::driftlayer
					///The drift layer probability.
					OGRLayer* driftlayer;
			#endif
			// DocString: FMTParallelWriter::mtx
			///The recursive mutex used to control the usage of the writer by the thread.
			mutable boost::recursive_mutex mtx;
			// DocString: FMTParallelWriter::resultsminimaldrift;
			///Used when writing drift probability layer
			double resultsminimaldrift;
			// DocString: FMTParallelWriter::outputslevel
			///The output level of detail
			Core::FMToutputlevel outputslevel;
			// DocString: FMTParallelWriter::alllayeroptions
			///The layers options
			std::vector<std::string> alllayeroptions;
			// DocString: FMTParallelWriter::outputfirstperiod
			///First period for the output value.
			int outputfirstperiod;
			// DocString: FMTParallelWriter::outputlastperiod;
			///Lastperiod for the output value.
			int outputlastperiod;
			// DocString: FMTParallelWriter::projectdirectory;
			///The directory of the primary file.
			std::string projectdirectory;
			// DocString: FMTParallelWriter::projectname;
			///The name of the project file
			std::string projectname;
			// DocString: FMTParallelWriter::projectname;
			///The directory output path.
			std::string m_outputLocationPath;
		};
}
#endif
