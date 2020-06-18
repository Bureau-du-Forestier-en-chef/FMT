/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTlifespanparser_H_INCLUDED
#define FMTlifespanparser_H_INCLUDED

#include "FMTlist.h"
#include "FMTparser.h"
#include "FMTlifespans.h"
#include "FMTtheme.h"
#include "FMTconstants.h"
#include <string>

namespace Parser

{
class FMTlifespanparser : public FMTparser
    {
    public:
        FMTlifespanparser();
		~FMTlifespanparser() = default;
        FMTlifespanparser(const FMTlifespanparser& rhs);
        FMTlifespanparser& operator = (const FMTlifespanparser& rhs);
        Core::FMTlifespans read(const std::vector<Core::FMTtheme>& themes,const Core::FMTconstants& constants,const std::string& location);
        void write(const Core::FMTlifespans& lifespan,const std::string& location) const;
    };

}


#endif // FMTlifespanparser_H_INCLUDED
