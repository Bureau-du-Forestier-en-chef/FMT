/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTscheduleparser_Hm_included
#define FMTscheduleparser_Hm_included

#include "FMTParser.h"
#include "FMTutility.h"
#include <vector>
#include <string>

namespace Core {
	class FMTTheme;
	class FMTAction;
	class FMTSchedule;
}

namespace Parser
{
// DocString: FMTScheduleParser
/**
FMTScheduleParser implement the reading and writing of a vector of FMTschedules.
This parser is also used by the FMTModelParser.
*/
class FMTEXPORT FMTScheduleParser: public FMTParser
    {
	public:
		// DocString: FMTScheduleParser()
		/**
		Default constructor for FMTScheduleParser
		*/
        FMTScheduleParser();
		// DocString: ~FMTScheduleParser()
		/**
		Default destructor for FMTScheduleParser
		*/
		~FMTScheduleParser() = default;
		// DocString: FMTScheduleParser(const FMTScheduleParser&)
		/**
		Default copy constructor for FMTScheduleParser
		*/
        FMTScheduleParser(const FMTScheduleParser& rhs);
		// DocString: FMTScheduleParser::getSchedulePath
		/**
		Take a primary path and a scenario name and return the path to the schedule file (in Scenario or in root).
		*/
		std::string getSchedulePath(const std::string& p_primary_path, const std::string& p_output_scenario_name);
		// DocString: FMTScheduleParser::operator=
		/**
		Default copy assignment operator for FMTScheduleParser
		*/
        FMTScheduleParser& operator = (const FMTScheduleParser& rhs);
		// DocString: FMTScheduleParser::read
		/**
		This function reads a schedule section at a given (location) and turns it into a vector of FMTSchedule,
		using a complete vector of FMTTheme (themes), a vector of FMTAction (action), and a optional (tolerance).
		If area harvested for a given development is <= to tolerance parameter then it wont be selected. 
		*/
        std::vector<Core::FMTSchedule>read(const std::vector<Core::FMTTheme>& themes,
			const  std::vector<Core::FMTAction>& actions,const std::string& location,double tolerance = FMT_DBL_TOLERANCE);
		// DocString: FMTScheduleParser::write
		/**
		This function write a vector of FMTschedules (schedules) into a (location) schedule file.
		*/
        void write(const std::vector<Core::FMTSchedule>& schedules, const std::string& location, bool append = false) const;

	private:
		// DocString: FMTScheduleParser::getVariable
		/**
		This function uses the comment present in the schedule section representing the
		variable index of the scheduled area to harvest and return it has a int value.
		*/
		int getVariable() const;
		// DocString: FMTScheduleParser::
		/**

		*/
		static void _writeSchedule(std::ofstream& p_stream, const std::vector<Core::FMTSchedule>& p_schedules);
		// DocString: FMTScheduleParser::
		/**

		*/
		static std::vector<Core::FMTSchedule>::const_iterator _getFirstEmptySchedule(const std::vector<Core::FMTSchedule>& p_schedules);

    };
}


#endif // FMTscheduleparser_Hm_included
