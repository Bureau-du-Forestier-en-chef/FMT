/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTmodelparser_H_INCLUDED
#define FMTmodelparser_H_INCLUDED


#include "FMTparser.h"
#include "FMTconstantparser.h"
#include "FMTlandscapeparser.h"
#include "FMTareaparser.h"
#include "FMTyieldparser.h"
#include "FMTactionparser.h"
#include "FMTtransitionparser.h"
#include "FMTlifespanparser.h"
#include "FMToutputparser.h"
#include "FMToptimizationparser.h"
#include "FMTscheduleparser.h"
#include "FMTmodel.h"
#include "boost/filesystem.hpp"
#include <map>
#include <string>
#include <vector>
#include "FMTschedule.h"

namespace Parser
{
// DocString: FMTmodelparser
/**
FMTmodelparser is the main parser class used for reading and writing models files.
This class can read single model scenario and multiple model scenarios at the same time.
The class make use of all other parser class of FMT to read and write a model.
*/
class FMTmodelparser : public FMTparser
    {
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
					std::string opt = std::string(),bool allow_mapping=false);
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
		~FMTmodelparser() = default;
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
    };
}

#endif // FMTmodelparser_H_INCLUDED
