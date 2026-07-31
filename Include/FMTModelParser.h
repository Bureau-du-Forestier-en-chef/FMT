/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTmodelparser_Hm_included
#define FMTmodelparser_Hm_included


#include <map>
#include <string>
#include <vector>
#include "FMTParser.h"
#include "FMTOutputNode.h"
#include "FMTConstants.h"


namespace Models {
	class FMTModel;
}


namespace Core {
	class FMTSchedule;
	class FMTConstraint;
	class FMTOutput;
}


namespace Parser
{
class FMTConstantParser;
class FMTLandscapeParser;
class FMTAreaParser;
class FMTYieldParser;
class FMTActionParser;
class FMTLifespanParser;
class FMTOutputParser;
class FMTOptimizationParser;
class FMTScheduleParser;
// DocString: FMTModelParser
/**
@brief Main parser reading and writing model files, for single or multiple scenarios, using all the other FMT parsers.
*/
class FMTEXPORT FMTModelParser : public FMTParser
    {
    public:
		// DocString: FMTModelParser()
		/**
		@brief Default constructor for FMTModelParser.
		*/
        FMTModelParser();
		// DocString: ~FMTModelParser()
		/**
		@brief Default virtual destructor for FMTModelParser.
		*/
		virtual ~FMTModelParser() = default;
		// DocString: FMTModelParser(const FMTModelParser&)
		/**
		@brief Copy constructor for FMTModelParser.
		@param[in] rhs the FMTModelParser to copy.
		*/
        FMTModelParser(const FMTModelParser& rhs);
		// DocString: FMTModelParser::operator=
		/**
		@brief Copy assignment operator for FMTModelParser.
		@param[in] rhs the FMTModelParser to copy.
		@return a reference to this FMTModelParser.
		*/
        FMTModelParser& operator = (const FMTModelParser& rhs);
		// DocString: FMTModelParser::read
		/**
		@brief Read a single model and return it as a FMTModel.
		@param[in] con the constants file path.
		@param[in] lan the landscape file path.
		@param[in] lif the lifespan file path.
		@param[in] are the area file path.
		@param[in] yld the yields file path.
		@param[in] act the actions file path.
		@param[in] tr the transitions file path.
		@param[in] out the outputs file path.
		@param[in] opt the optional optimize file path.
		@return the model.
		*/
        Models::FMTModel read(const std::string& con,const std::string& lan,
                      const std::string& lif,const std::string& are,const std::string& yld,
                      const std::string& act,const std::string& tr,const std::string& out,
						std::string opt = std::string());
		// DocString: FMTModelParser::readtemplate
		/**
		@brief Read the base model then load each folder in the template folder as a scenario of the base model.
		@param[in] primarym_location the primary file location.
		@param[in] templatefolder the template folder.
		@return the models.
		*/
		std::vector<Models::FMTModel>readTemplates(const std::string& primarym_location,const std::string& templatefolder);
		// DocString: FMTModelParser::readproject
		/**
		@brief Read multiple scenarios based on a primary file location.
		@param[in] primarym_location the primary file location.
		@param[in] scenarios the scenarios to read, all if empty, add ROOT for the base scenario.
		@param[in] readarea if true reads the area section.
		@param[in] readoutputs if true reads the output section.
		@param[in] readoptimize if true reads the optimize section.
		@return the models.
		*/
		std::vector<Models::FMTModel>readproject(const std::string& primarym_location,
			std::vector<std::string>scenarios = std::vector<std::string>(),
			bool readarea = true,bool readoutputs = true, bool readoptimize = true);
		// DocString: FMTModelParser::writeToProject
		/**
		@brief Create a primary file if needed and write the model as a root scenario, or as a new scenario if the primary file exists.
		@param[in] primarym_location the primary file location.
		@param[in] model the model.
		*/
		void writeToProject(const std::string& primarym_location,
			const Models::FMTModel& model);
		// DocString: FMTModelParser::readSchedules
		/**
		@brief Read the schedule files of the models based on a primary file.
		@param[in] primarym_location the primary file location.
		@param[in] models the models.
		@return a vector of schedules per model.
		*/
		std::vector<std::vector<Core::FMTSchedule>>readSchedules(const std::string& primarym_location,
			const std::vector<Models::FMTModel>& models);
		// DocString: FMTModelParser::getConstraintsFromString
		/**
		@brief Get constraints from a string.
		@param[in] constraintstr the constraint string.
		@param[in] model the model.
		@param[in] constants the constants.
		@return the constraints.
		*/
		std::vector<Core::FMTConstraint>getConstraintsFromString(std::string constraintstr,
			const Models::FMTModel& model,Core::FMTConstants constants = Core::FMTConstants());
		// DocString: FMTModelParser::write
		/**
		@brief Write a model into a folder, naming the files based on the model name.
		@param[in] model the model.
		@param[in] folder the folder.
		*/
        void write(const Models::FMTModel& model,const std::string& folder) const;
		#ifdef FMTWITHGDAL
		// DocString: FMTModelParser::createResultsLayer
		/**
		@brief Create a results layer on a dataset for a given model name.
		@param[in] p_modelName the model name.
		@param[in] p_dataset the dataset.
		@param[in] p_creationOptions the creation options.
		@return the created layer.
		*/
		OGRLayer* createResultsLayer(
			const std::string& p_modelName,
			GDALDataset* p_dataset,
			std::vector<std::string> p_creationOptions = std::vector<std::string>()) const;
		// DocString: FMTModelParser::fillUpInfeasibles
		/**
		@brief Fill up an OGR layer with infeasible values.
		@param[in,out] layer the layer.
		@param[in] theoutputs the outputs.
		@param[in] iteration the iteration.
		@param[in] firstPeriod the first period.
		@param[in] lastPeriod the last period.
		*/
		void fillUpInfeasibles(OGRLayer* layer,
			const std::vector<Core::FMTOutput>&theoutputs,
			const int& iteration,const int& firstPeriod, const int&lastPeriod) const;
		// DocString: FMTModelParser::writeFeatures
		/**
		@brief Write multiple features to a layer.
		@param[in,out] layer the layer.
		@param[in] firstPeriod the first period.
		@param[in] iteration the iteration.
		@param[in] theoutputs the outputs.
		@param[in] values the values.
		@param[in] writeNaN if true writes NaN values.
		*/
		void writeFeatures(OGRLayer* layer,const int& firstPeriod, const int& iteration,
			const std::vector<Core::FMTOutput>&theoutputs,
			const std::map<std::string,std::vector<std::vector<double>>>& values, bool writeNaN = false)const;
		// DocString: FMTModelParser::writeResults
		/**
		@brief Write the results of a model from the first to the last period for a vector of outputs into a GDAL driver.
		@param[in] model the model.
		@param[in] theoutputs the outputs.
		@param[in] firstPeriod the first period.
		@param[in] lastPeriod the last period.
		@param[in] location the location.
		@param[in] level the output level.
		@param[in] gdaldrivername the GDAL driver name.
		*/
		void writeResults(const Models::FMTModel& model,
			const std::vector<Core::FMTOutput>&theoutputs,
			const int& firstPeriod, const int& lastPeriod,
			const std::string& location,
			Core::FMToutputlevel level = Core::FMToutputlevel::standard,
			std::string gdaldrivername= "CSV") const;
		#endif 
	protected:
	#ifdef FMTWITHGDAL
		// DocString: FMTModelParser::getIterationsValues()
		/**
		@brief Return all the values per iteration and period from a layer.
		@param[in] layer the layer.
		@return a map of values per iteration and period.
		*/
		std::map<std::string, std::vector<std::vector<double>>>getIterationsValues(OGRLayer* layer) const;
		// DocString: FMTModelParser::createDriftLayer()
		/**
		@brief Create an output drift layer.
		@param[in] dataset the dataset.
		@param[in] creationoptions the creation options.
		@return the drift layer.
		*/
		OGRLayer* createDriftLayer(GDALDataset* dataset, std::vector<std::string> creationoptions = std::vector<std::string>()) const;
		// DocString: FMTModelParser::writedriftr()
		/**
		@brief Write the drift lower and upper values to a layer.
		@param[in,out] layer the layer.
		@param[in] lowervalues the lower values.
		@param[in] uppervalues the upper values.
		*/
		void writeDrift(OGRLayer* layer, const std::map<std::string, std::map<double, std::vector<double>>>& lowervalues,
			const std::map<std::string, std::map<double, std::vector<double>>>& uppervalues) const;
	#endif
		// DocString: FMTModelParser::writePrimary()
		/**
		@brief Write the primary file with the section file names.
		@param[in] location the location.
		@param[in] lanfile the landscape file.
		@param[in] arefile the area file.
		@param[in] yldfile the yields file.
		@param[in] actfile the actions file.
		@param[in] trnfile the transitions file.
		@param[in] outfile the outputs file.
		@param[in] optfile the optimize file.
		@param[in] liffile the lifespan file.
		@param[in] seqfile the schedule file.
		*/
		void writePrimary(
			const std::string& location,
			const std::string& lanfile,
			const std::string& arefile,
			const std::string& yldfile,
			const std::string& actfile,
			const std::string& trnfile,
			const std::string& outfile,
			const std::string& optfile,
			const std::string& liffile,
			std::string seqfile = std::string()) const;
		// DocString: FMTModelParser::writeModel
		/**
		@brief Write the sections of a model, and the schedules if the schedule file is not empty.
		@param[in] model the model.
		@param[in] lanfile the landscape file.
		@param[in] arefile the area file.
		@param[in] yldfile the yields file.
		@param[in] actfile the actions file.
		@param[in] trnfile the transitions file.
		@param[in] outfile the outputs file.
		@param[in] optfile the optimize file.
		@param[in] liffile the lifespan file.
		@param[in] seqfile the schedule file.
		*/
		void writeModel(const Models::FMTModel& model,
			const std::string& lanfile,
			const std::string& arefile,
			const std::string& yldfile,
			const std::string& actfile,
			const std::string& trnfile,
			const std::string& outfile,
			const std::string& optfile,
			const std::string& liffile,
			const std::string& seqfile) const;
	private:
		// DocString: FMTModelParser::referenceRead
		/**
		@brief Read a section once or reference it to a section of another model when reading multiple scenarios sharing sections.
		@param[in,out] commonm_sections the common sections.
		@param[in,out] models the models.
		@param[in] con the constants file path.
		@param[in] lan the landscape file path.
		@param[in] lif the lifespan file path.
		@param[in] are the area file path.
		@param[in] yld the yields file path.
		@param[in] act the actions file path.
		@param[in] tr the transitions file path.
		@param[in] out the outputs file path.
		@param[in] opt the optimize file path.
		@param[in] allow_mapping if true allows mapping.
		@return the model.
		*/
		Models::FMTModel referenceRead(std::map<std::string, std::vector<int>>& commonm_sections,
			std::vector<Models::FMTModel>& models,
			const std::string& con, const std::string& lan,
			const std::string& lif, const std::string& are, const std::string& yld,
			const std::string& act, const std::string& tr, const std::string& out,
			std::string opt = std::string(), bool allow_mapping = false);
		// DocString: FMTModelParser::readFromFolder
		/**
		@brief Read models from a folder based on a primary location.
		@param[in] primarym_location the primary file location.
		@param[in] folder the folder.
		@param[in] scenarios the scenarios to read, all if empty.
		@param[in] validatescenarioname if true only reads sections matching the primary name.
		@param[in] readarea if true reads the area section.
		@param[in] readoutputs if true reads the output section.
		@param[in] readoptimize if true reads the optimize section.
		@return the models.
		*/
		std::vector<Models::FMTModel>readFromFolder(const std::string& primarym_location,
			const std::string& folder,
			std::vector<std::string>scenarios,
			bool validatescenarioname,
			bool readarea = true, bool readoutputs = true, bool readoptimize = true);
		// DocString: FMTModelParser::setSolverParameters
		/**
		@brief Read and set the solver parameters to a model from the ColdStart and WarmStart files in the optimize file directory.
		@param[in,out] p_model the model.
		@param[in] p_optimize_file the optimize file location.
		*/
		void setSolverParameters(Models::FMTModel& p_model, const std::string& p_optimize_file) const;
    };
}

#endif // FMTmodelparser_Hm_included
