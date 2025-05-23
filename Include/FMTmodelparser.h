/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTmodelparser_H_INCLUDED
#define FMTmodelparser_H_INCLUDED


#include <map>
#include <string>
#include <vector>
#include "FMTparser.h"
#include "FMToutputnode.h"
#include "FMTconstants.h"


namespace Models {
	class FMTmodel;
}


namespace Core {
	class FMTschedule;
	class FMTconstraint;
	class FMToutput;
}


namespace Parser
{
class FMTconstantparser;
class FMTlandscapeparser;
class FMTareaparser;
class FMTyieldparser;
class FMTactionparser;
class FMTlifespanparser;
class FMToutputparser;
class FMToptimizationparser;
class FMTscheduleparser;
// DocString: FMTmodelparser
/**
FMTmodelparser is the main parser class used for reading and writing models files.
This class can read single model scenario and multiple model scenarios at the same time.
The class make use of all other parser class of FMT to read and write a model.
*/
class FMTEXPORT FMTmodelparser : public FMTparser
    {
    public:
		// DocString: FMTmodelparser()
		/**
		Default constructor for FMTmodelparser.
		*/
        FMTmodelparser();
		// DocString: ~FMTmodelparser()
		/**
		Default destructor for FMTmodelparser.
		*/
		virtual ~FMTmodelparser() = default;
		// DocString: FMTmodelparser(const FMTmodelparser&)
		/**
		Default copy constructor for FMTmodelparser.
		*/
        FMTmodelparser(const FMTmodelparser& rhs);
		// DocString: FMTmodelparser::operator=
		/**
		Default copy assignment for FMTmodelparser.
		*/
        FMTmodelparser& operator = (const FMTmodelparser& rhs);
		// DocString: FMTmodelparser::read
		/**
		This function aim at reading a single model and return it as a FMTmodel.
		using the path to the constants (cont), landscape (lan), lifespan(lif),area(are),
		yields(yld),actions(act),transitions(tr),outputs (out) and optional optimize (opt).
		*/
        Models::FMTmodel read(const std::string& con,const std::string& lan,
                      const std::string& lif,const std::string& are,const std::string& yld,
                      const std::string& act,const std::string& tr,const std::string& out,
						std::string opt = std::string());
		// DocString: FMTmodelparser::readtemplate
		/**
		The readtemplate function read the base model from primary_location and then load all everything from templatefolder and 
		use each folder in templatefolder has scenarios of the base model. Each folders within the templatefolder will will gave their names
		to the FMTmodel. So within a given subfolder it will read *._act, *._yld and so on...
		*/
		std::vector<Models::FMTmodel>readtemplates(const std::string& primary_location,const std::string& templatefolder);
		// DocString: FMTmodelparser::readproject
		/**
		The readproject function reads multiple scenarios based on a primary file location (.pri).
		If the scenarios vector is provided then FMTmodelparser will only read the scenarios provied by the user.
		For reading the base scenario the user needs to add the string "ROOT" to the scenarios vector. If the scenarios
		vector is empty then all scenarios including the ROOT are going to be read. If readarea is false then the area section
		wont be read. If readoutputs is false then output section wont be read. If readoptimize is false then the optimize section
		wonht be read.
		*/
		std::vector<Models::FMTmodel>readproject(const std::string& primary_location,
			std::vector<std::string>scenarios = std::vector<std::string>(),
			bool readarea = true,bool readoutputs = true, bool readoptimize = true);
		// DocString: FMTmodelparser::writetoproject
		/**
		The function will create a primary file if it does not exist and write the model in has a root scenario.
		If the primary file exist it will read the scenario writen and compare each section with the model and write the model has a 
		new scenario...
		*/
		void writetoproject(const std::string& primary_location,
			const Models::FMTmodel& model);
		// DocString: FMTmodelparser::readschedules
		/**
		The readschedules function read the schedule files of the specified (models) based on a primary file
		(primary_location). It returns a vector of vector of FMTschedule ordered the same way as the models.
		If validate file date is true then it will validate that the parser most recent file is older than the
		schedule parser most recent file, if it is not it will return a empty schedule.
		*/
		std::vector<std::vector<Core::FMTschedule>>readschedules(const std::string& primary_location,
			const std::vector<Models::FMTmodel>& models);
		// DocString: FMTmodelparser::getconstraintsfromstring
		/**
		Get constraints from string.
		*/
		std::vector<Core::FMTconstraint>getconstraintsfromstring(std::string constraintstr,
			const Models::FMTmodel& model,Core::FMTconstants constants = Core::FMTconstants());
		// DocString: FMTmodelparser::write
		/**
		The write function write a FMTmodel (model) in to a folder (folder) all the files are going to be named
		based on the name of the FMTmodel.
		*/
        void write(const Models::FMTmodel& model,const std::string& folder) const;
		#ifdef FMTWITHGDAL
		// DocString: FMTmodelparser::createResultsLayer
		/**
		@brief Create a results layer on a dataset for a given model name.
		@param[in] p_modelName the name of the model
		@param[in] p_dataset the dataset of the layer
		@return the newly created layer
		*/
		OGRLayer* createResultsLayer(
			const std::string& p_modelName,
			GDALDataset* p_dataset,
			std::vector<std::string> p_creationOptions = std::vector<std::string>()) const;
		// DocString: FMTmodelparser::fillupinfeasibles
		/**
		Fill up the OGRlayer of infeasible values.
		*/
		void fillupinfeasibles(OGRLayer* layer,
			const std::vector<Core::FMToutput>&theoutputs,
			const int& iteration,const int& firstperiod, const int&lastperiod) const;
		// DocString: FMTmodelparser::writefeatures
		/**
		Write multiple features (outputs)
		*/
		void writefeatures(OGRLayer* layer,const int& firstperiod, const int& iteration,
			const std::vector<Core::FMToutput>&theoutputs,
			const std::map<std::string,std::vector<std::vector<double>>>& values, bool writeNaN = false)const;
		// DocString: FMTmodelparser::writeresults
		/**
		The function writeresults for any kind of FMTmodel (optimized or simulated), starting from the first period to the last period, for a vector of outputs.
		Write the results down in a gdaldriver (gdaldrivername), You can also specify the iteration will be 0 by default.
		*/
		void writeresults(const Models::FMTmodel& model,
			const std::vector<Core::FMToutput>&theoutputs,
			const int& firstperiod, const int& lastperiod,
			const std::string& location,
			Core::FMToutputlevel level = Core::FMToutputlevel::standard,
			std::string gdaldrivername= "CSV") const;
		#endif 
	protected:
	#ifdef FMTWITHGDAL
		// DocString: FMTmodelparser::getiterationsvalues()
		/**
		Go back in the layer and get all values per iteration and periods.
		*/
		std::map<std::string, std::vector<std::vector<double>>>getiterationsvalues(OGRLayer* layer) const;
		// DocString: FMTmodelparser::createdriftlayer()
		/**
		Create a output drift layer.
		*/
		OGRLayer* createdriftlayer(GDALDataset* dataset, std::vector<std::string> creationoptions = std::vector<std::string>()) const;
		// DocString: FMTmodelparser::writedriftr()
		/**
		Go back in the layer and get all values per iteration and periods.
		*/
		void writedrift(OGRLayer* layer, const std::map<std::string, std::map<double, std::vector<double>>>& lowervalues,
			const std::map<std::string, std::map<double, std::vector<double>>>& uppervalues) const;
	#endif
		// DocString: FMTmodelparser::writeprimary()
		/**
		Write the primary file with all the other file name.
		*/
		void writeprimary(
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
		// DocString: FMTmodelparser::writemodel
		/**
		Base function to write model section if string non empty then write the corresponding section of the model with the schedules also if not empty
		*/
		void writemodel(const Models::FMTmodel& model,
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
		// DocString: FMTmodelparser::referenceread
		/**
		When reading multiple scenarios at the same time, sometime some scenarios share the same section.
		Instead of reading multiple time the same section this function read it once or reference it to
		a section of a other model in the (models) vector.
		*/
		Models::FMTmodel referenceread(std::map<std::string, std::vector<int>>& common_sections,
			std::vector<Models::FMTmodel>& models,
			const std::string& con, const std::string& lan,
			const std::string& lif, const std::string& are, const std::string& yld,
			const std::string& act, const std::string& tr, const std::string& out,
			std::string opt = std::string(), bool allow_mapping = false);
		// DocString: FMTmodelparser::readfromfolder
		/**
		Read models from a folder (can be scenarios), based on a primary_location, if scenarios is empty it will read all models in the
		folder, if validatescenarioname is true it will only read model's section with name matching the primary name else it will read anything.
		*/
		std::vector<Models::FMTmodel>readfromfolder(const std::string& primary_location,
			const std::string& folder,
			std::vector<std::string>scenarios,
			bool validatescenarioname,
			bool readarea = true, bool readoutputs = true, bool readoptimize = true);
		// DocString: FMTmodelparser::setSolverParameters
		/**
		@brief based on the ColdStart.* file and WarmStart.* file in the same directory of p_optimize_file read and set the parameters to FMTmodel
		@p_model the model for which we want to set the solver parameters.
		@p_optimize_file the string location of the optimization file.
		*/
		void setSolverParameters(Models::FMTmodel& p_model, const std::string& p_optimize_file) const;
    };
}

#endif // FMTmodelparser_H_INCLUDED
