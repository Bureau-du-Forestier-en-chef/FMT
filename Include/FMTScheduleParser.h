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
@brief Parser reading and writing vectors of schedules, also used by the model parser.
*/
class FMTEXPORT FMTScheduleParser: public FMTParser
    {
	public:
		// DocString: FMTScheduleParser()
		/**
		@brief Default constructor for FMTScheduleParser.
		*/
        FMTScheduleParser();
		// DocString: ~FMTScheduleParser()
		/**
		@brief Default destructor for FMTScheduleParser.
		*/
		~FMTScheduleParser() = default;
		// DocString: FMTScheduleParser(const FMTScheduleParser&)
		/**
		@brief Copy constructor for FMTScheduleParser.
		@param[in] rhs the FMTScheduleParser to copy.
		*/
        FMTScheduleParser(const FMTScheduleParser& rhs);
		// DocString: FMTScheduleParser::getSchedulePath
		/**
		@brief Return the path to the schedule file from a primary path and a scenario name.
		@param[in] p_primary_path the primary path.
		@param[in] p_output_scenario_name the scenario name.
		@return the schedule file path.
		*/
		std::string getSchedulePath(const std::string& p_primary_path, const std::string& p_output_scenario_name);
		// DocString: FMTScheduleParser::operator=
		/**
		@brief Copy assignment operator for FMTScheduleParser.
		@param[in] rhs the FMTScheduleParser to copy.
		@return a reference to this FMTScheduleParser.
		*/
        FMTScheduleParser& operator = (const FMTScheduleParser& rhs);
		// DocString: FMTScheduleParser::read
		/**
		@brief Read a schedule section and turn it into a vector of schedules.
		@param[in] themes the themes.
		@param[in] actions the actions.
		@param[in] location the file location.
		@param[in] tolerance the area tolerance below which a development is not selected.
		@return the schedules.
		*/
        std::vector<Core::FMTSchedule>read(const std::vector<Core::FMTTheme>& themes,
			const  std::vector<Core::FMTAction>& actions,const std::string& location,double tolerance = FMT_DBL_TOLERANCE);
		// DocString: FMTScheduleParser::write
		/**
		@brief Write a vector of schedules to a schedule file.
		@param[in] schedules the schedules.
		@param[in] location the file location.
		@param[in] append if true appends to the file.
		*/
        void write(const std::vector<Core::FMTSchedule>& schedules, const std::string& location, bool append = false) const;

	private:
		// DocString: FMTScheduleParser::getVariable
		/**
		@brief Return the variable index of the scheduled area to harvest from the section comment.
		@return the variable index.
		*/
		int getVariable() const;
		// DocString: FMTScheduleParser::
		/**
		@brief Write the schedules to a stream.
		@param[in,out] p_stream the output stream.
		@param[in] p_schedules the schedules.
		*/
		static void _writeSchedule(std::ofstream& p_stream, const std::vector<Core::FMTSchedule>& p_schedules);
		// DocString: FMTScheduleParser::
		/**
		@brief Return an iterator to the first empty schedule.
		@param[in] p_schedules the schedules.
		@return an iterator to the first empty schedule.
		*/
		static std::vector<Core::FMTSchedule>::const_iterator _getFirstEmptySchedule(const std::vector<Core::FMTSchedule>& p_schedules);

    };
}


#endif // FMTscheduleparser_Hm_included
