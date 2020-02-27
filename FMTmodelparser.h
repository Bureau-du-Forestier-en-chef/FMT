/*
MIT License

Copyright (c) [2019] [Bureau du forestier en chef]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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

class FMTmodelparser : public FMTparser
    {
	Models::FMTmodel referenceread(std::map<std::string, std::vector<int>>& common_sections,
					std::vector<Models::FMTmodel>& models,
					const std::string& con, const std::string& lan,
					const std::string& lif, const std::string& are, const std::string& yld,
					const std::string& act, const std::string& tr, const std::string& out,
					std::string opt = std::string(),bool allow_mapping=false);
    public:
        FMTmodelparser();
        FMTmodelparser(const FMTmodelparser& rhs);
        FMTmodelparser& operator = (const FMTmodelparser& rhs);
        Models::FMTmodel read(const std::string& con,const std::string& lan,
                      const std::string& lif,const std::string& are,const std::string& yld,
                      const std::string& act,const std::string& tr,const std::string& out,
						std::string opt = std::string());
		std::vector<Models::FMTmodel>readproject(const std::string& primary_location,
			std::vector<std::string>scenarios = std::vector<std::string>(),
			bool readarea = true,bool readoutputs = true, bool readoptimize = true);
		std::vector<std::vector<Core::FMTschedule>>readschedules(const std::string& primary_location,
			const std::vector<Models::FMTmodel>& models);
        bool write(const Models::FMTmodel& model,const std::string& folder);
    };
}

#endif // FMTmodelparser_H_INCLUDED
