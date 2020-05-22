/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTtransitionparser_H_INCLUDED
#define FMTtransitionparser_H_INCLUDED

#include "FMTparser.h"
#include "FMTconstants.h"
#include "FMTtheme.h"
#include "FMTaction.h"
#include "FMTtransition.h"
#include "FMTtransitionmask.h"
#include "FMTyields.h"
#include <regex>
#include "string"
#include <vector>
#include "FMTmask.h"

namespace Parser
{

class FMTtransitionparser : public FMTparser
    {
		std::regex rxsection;
		std::regex rxlock;
		std::regex rxage;
		std::regex rxreplace;
		std::regex rxtyld;
    public:
        FMTtransitionparser();
		~FMTtransitionparser() = default;
        FMTtransitionparser(const FMTtransitionparser& rhs);
        FMTtransitionparser& operator = (const FMTtransitionparser& rhs);
		Core::FMTmask getsource(std::string& line, Core::FMTspec& spec,const std::vector<Core::FMTtheme>& themes,
							Core::FMTsection section,const Core::FMTconstants& constant,
                          const Core::FMTyields& ylds);
        std::vector<Core::FMTtransitionmask> getmasktran(const std::string& line,const std::vector<Core::FMTtheme>& themes,
                                        const Core::FMTconstants& constants, const Core::FMTyields& ylds,
                                        const Core::FMTmask& sourcemask, int& replaced);
        std::vector<Core::FMTtransition>read(const std::vector<Core::FMTtheme>& themes,
                           const std::vector<Core::FMTaction>& actions,
                           const Core::FMTyields& ylds,const Core::FMTconstants& constants,
						   const std::string& location);
        void write(const std::vector<Core::FMTtransition>& transitions,const std::string& location) const;
    };
}
#endif // FMTtransitionparser_H_INCLUDED
