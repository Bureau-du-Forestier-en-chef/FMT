#ifndef FMTEXCELLOGGER_Hm_included
#define FMTEXCELLOGGER_Hm_included

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include "FMTObject.h"
#include "FMTLogger.h"


namespace Logging
{
	// DocString: FMTExcelLogger
	/**
	@brief Logger used by the Excel add-in that stores the output in a string instead of printing it.
	*/
	class FMTEXPORT FMTExcelLogger : public Logging::FMTLogger
	{
	public:
		// DocString: FMTExcelLogger()
		/**
		@brief Default constructor for FMTExcelLogger.
		*/
		FMTExcelLogger();
		// DocString: FMTExcelLogger::getPrintOut
		/**
		@brief Return the stored output.
		@return the accumulated output string.
		*/
		std::string getPrintOut() const;
		// DocString: FMTExcelLogger::clearOut
		/**
		@brief Clear the stored output.
		*/
		void clearOut();
	#ifdef FMTWITHOSI
		// DocString: FMTExcelLogger::clone
		/**
		@brief Clone the FMTExcelLogger.
		@return a pointer to the cloned logger.
		*/
		virtual FMTLogger* clone() const;
	#endif
	// DocString: FMTExcelLogger::Clone
	/**
	@brief Clone the FMTExcelLogger.
	@return a unique pointer to the cloned logger.
	*/
	virtual std::unique_ptr <FMTLogger> Clone() const;
	protected:
		// DocString: FMTExcelLogger::_cout
		/**
		@brief Append a message to the stored output.
		@param[in] message the message to append.
		*/
		void _cout(const char* message) const override;
	private:
		mutable std::string printout;
	};

}
#endif