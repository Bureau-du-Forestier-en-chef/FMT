/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMToutputparser_H_INCLUDED
#define FMToutputparser_H_INCLUDED

#include "FMTparser.h"
#include "FMTtheme.h"
#include "FMTaction.h"
#include "FMTyields.h"
#include "FMTconstants.h"
#include "FMToutput.h"
#include "FMTutility.h"
#include <regex>
#include "FMTconstants.h"
#include "FMTtheme.h"
#include "FMTaction.h"
#include <string>

namespace Parser

{

class FMToutputparser : public FMTparser
    {
	std::regex rxoutput;
	std::regex rxsource;
	std::regex rxtar;
	std::regex rxgrp;
	std::regex rxoutputconstant;
    public:
        FMToutputparser();
		~FMToutputparser() = default;
        FMToutputparser(const FMToutputparser& rhs);
        FMToutputparser& operator = (const FMToutputparser& rhs);
		std::vector<Core::FMToutput> read(const std::vector<Core::FMTtheme>& themes,
                            const std::vector<Core::FMTaction>& actions,
                            const Core::FMTyields& ylds,const Core::FMTconstants& constants,
							const std::string& location);
        void write(const std::vector<Core::FMToutput>& outputs,const std::string& location) const;
    };

}


#endif // FMToutputparser_H_INCLUDED
