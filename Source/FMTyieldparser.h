/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTyieldparser_H_INCLUDED
#define FMTyieldparser_H_INCLUDED

#include "FMTlist.h"
#include "FMTparser.h"
#include "FMTyields.h"
#include "FMTutility.h"
#include "FMTconstants.h"
#include "FMTtheme.h"
#include "FMTdata.h"
#include <boost/tokenizer.hpp>
#include <iterator>
#include <regex>
#include <string>
#include <vector>
#include <utility>

namespace Parser
{

class FMTyieldparser : public FMTparser
    {
     private:
		std::regex rxyieldtype;
		std::regex rxcomplex;
		std::regex rxeqs;
		std::regex rxdiscount;
		Core::FMTyldtype getyldtype(const std::string& value) const;
		Core::FMTyieldparserop getyldctype(const std::string& value) const;
		std::vector<std::string> getylduse(Core::FMTyields& yielddata,
			std::vector<std::pair<Core::FMTmask, Core::FMTyieldhandler>>::iterator actualyield,
                                   const std::vector<std::string>& values) const;
        void checkpreexisting(const std::vector<std::string>& preexists) const;
		bool isfunction(const std::string& strfunction) const;
		double getnumwithproportion(const std::string& value,const Core::FMTconstants& constants,
			const std::vector<double>& proportions, const int& location);
		Core::FMTdata geteq(const std::string& basestr,
                const Core::FMTconstants& constants,
                const Core::FMTyields& ylds,
                const std::vector<Core::FMTtheme>& themes);
		void cleanup(Core::FMTyields& yields, const std::vector<Core::FMTtheme>& themes) const;
		std::map<std::string, double>getindexvalues(const Core::FMTmask& mask,
			const std::vector<Core::FMTtheme>& themes,
			const std::vector<std::string>&indexvalues) const;
    public:
        FMTyieldparser();
		~FMTyieldparser() = default;
        FMTyieldparser(const FMTyieldparser& rhs);
        FMTyieldparser& operator = (const FMTyieldparser& rhs);
        Core::FMTyields read(const std::vector<Core::FMTtheme>& themes,const Core::FMTconstants& constants,const std::string& location);
        void write(const Core::FMTyields& yields,const std::string& location) const;
    };
}
#endif // FMTyieldparser_H_INCLUDED
