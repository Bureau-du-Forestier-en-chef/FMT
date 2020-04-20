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

#ifndef FMTscheduleparser_H_INCLUDED
#define FMTscheduleparser_H_INCLUDED

#include "FMTparser.h"
#include "FMTtheme.h"
#include "FMTaction.h"
#include "FMTschedule.h"
#include "FMTutility.h"
#include <vector>
#include <string>

namespace Parser
{
// DocString: FMTscheduleparser
/**
FMTscheduleparser implement the reading and writing of a vector of FMTschedules.
This parser is also used by the FMTmodelparser.
*/
class FMTscheduleparser: public FMTparser
    {
		// DocString: FMTscheduleparser::getvariable
		/**
		This function uses the comment present in the schedule section representing the
		variable index of the scheduled area to harvest and return it has a int value.
		*/
		int getvariable() const;
    public:
		// DocString: FMTscheduleparser()
		/**
		Default constructor for FMTscheduleparser
		*/
        FMTscheduleparser();
		// DocString: ~FMTscheduleparser()
		/**
		Default destructor for FMTscheduleparser
		*/
		~FMTscheduleparser() = default;
		// DocString: FMTscheduleparser(const FMTscheduleparser&)
		/**
		Default copy constructor for FMTscheduleparser
		*/
        FMTscheduleparser(const FMTscheduleparser& rhs);
		// DocString: FMTscheduleparser::operator=
		/**
		Default copy assignment operator for FMTscheduleparser
		*/
        FMTscheduleparser& operator = (const FMTscheduleparser& rhs);
		// DocString: FMTscheduleparser::read
		/**
		This function reads a schedule section at a given (location) and turns it into a vector of FMTschedule,
		using a complete vector of FMTtheme (themes), a vector of FMTaction (action), and a optional (tolerance).
		If area harvested for a given development is <= to tolerance parameter then it wont be selected. 
		*/
        std::vector<Core::FMTschedule>read(const std::vector<Core::FMTtheme>& themes,
			const  std::vector<Core::FMTaction>& actions,const std::string& location,double tolerance = FMT_DBL_TOLERANCE);
		// DocString: FMTscheduleparser::write
		/**
		This function write a vector of FMTschedules (schedules) into a (location) schedule file.
		*/
        void write(const  std::vector<Core::FMTschedule>& schedules, const std::string& location) const;
    };

}


#endif // FMTscheduleparser_H_INCLUDED
