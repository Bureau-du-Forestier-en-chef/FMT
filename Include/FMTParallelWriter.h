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
	@brief Writer used in concurrency to write multiple results in the same layer.
	*/
	class FMTEXPORT FMTParallelWriter : public Parser::FMTModelParser
	{
	public:
		// DocString: FMTParallelWriter::FMTParallelWriter()
		/**
		@brief Default constructor for FMTParallelWriter.
		*/
		FMTParallelWriter() = default;
		// DocString: ~FMTparallelwrite()
		/**
		@brief Default destructor for FMTParallelWriter.
		*/
		virtual ~FMTParallelWriter();
		// DocString: FMTParallelWriter::FMTParallelWriter(const FMTParallelWriter&)
		/**
		@brief Default copy constructor for FMTParallelWriter.
		@param[in] rhs the FMTParallelWriter to copy.
		*/
		FMTParallelWriter(const FMTParallelWriter& rhs) = default;
		// DocString: FMTParallelWriter::operator=()
		/**
		@brief Default copy assignment operator for FMTParallelWriter.
		@param[in] rhs the FMTParallelWriter to copy.
		@return a reference to this FMTParallelWriter.
		*/
		FMTParallelWriter& operator = (const FMTParallelWriter& rhs) = default;
		// DocString: FMTParallelWriter::FMTParallelWriter(const std::string&,const std::string&,const std::vector<Core::FMTOutput>&,const Models::FMTModel&)
		/**
		@brief Construct a FMTParallelWriter from the location, driver and outputs and the base models to write.
		@param[in] location the output location.
		@param[in] driver the output driver.
		@param[in] outputs the outputs.
		@param[in] allmodels the models.
		@param[in] layersoptions the layer options.
		@param[in] minimaldrift the minimal drift.
		@param[in] outputlevel the output level.
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
		@brief Construct a FMTParallelWriter without layers and outputs.
		@param[in] location the output location.
		@param[in] driver the output driver.
		@param[in] outputlevel the output level.
		@param[in] layersoptions the layer options.
		@param[in] firstPeriod the first period.
		@param[in] lastPeriod the last period.
		@param[in] primaryfilelocation the primary file location.
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
		@brief Write the model results from the first to the last period for a given iteration (replicate).
		@param[in] modelname the model name.
		@param[in] results the results.
		@param[in] firstPeriod the first period.
		@param[in] lastPeriod the last period.
		@param[in] iteration the iteration.
		*/
		void write(const std::string& modelname,
			const std::map<std::string, std::vector<std::vector<double>>>& results,
			const int& firstPeriod, const int& lastPeriod, const int& iteration) const;
		// DocString: FMTParallelWriter::writeSchedules
		/**
		@brief Write schedules to a file.
		@param[in] seqName the schedule file name.
		@param[in] scheduleList the schedules.
		@param[in] append if true appends to the file.
		*/
		void writeSchedules(const std::string seqName, const std::vector<Core::FMTSchedule> scheduleList, bool append) const;
		// DocString: FMTParallelWriter::getResults()
		/**
		@brief Get the results of a model.
		@param[in] modelptr the model.
		@param[in] firstPeriod the first period.
		@param[in] lastPeriod the last period.
		@return the results.
		*/
		std::map<std::string, std::vector<std::vector<double>>> getResults(const std::unique_ptr<Models::FMTModel>& modelptr, const int& firstPeriod, const int& lastPeriod) const;
		// DocString: FMTParallelWriter::setDriftProbability()
		/**
		@brief Compute the drift probability from the global and local models starting from a minimal drift proportion.
		@param[in] globalmodel the global model name.
		@param[in] localmodel the local model name.
		*/
		void setDriftProbability(const std::string& globalmodel, const std::string& localmodel) const;
		// DocString: FMTParallelWriter::getAndWrite()
		/**
		@brief Get the results of a model and write them down.
		@param[in] modelptr the model.
		@param[in] loutputs the outputs.
		*/
		void getAndWrite(const std::unique_ptr<Models::FMTModel>& modelptr, const std::vector<Core::FMTOutput>& loutputs);
		// DocString: FMTParallelWriter::close()
		/**
		@brief Close the dataset and destroy the layers, after which the writer is no longer usable.
		*/
		void close() noexcept;
		protected:
			// DocString: FMTParallelWriter::getDriftProbability()
			/**
			@brief Compute the drift probabilities from the global and local values.
			@param[in] globalvalues the global values.
			@param[in] localvalues the local values.
			@param[in] lower if true computes the lower drift.
			@return the drift probabilities.
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
