#ifndef FMTEXCELEXCEPTIONHANDLER_Hm_included
#define FMTEXCELEXCEPTIONHANDLER_Hm_included

#include "FMTExceptionHandler.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>


namespace Exception
{
	// DocString: FMTExcelExceptionHandler
	/**
	@brief Exception handler used by the Excel add-in that accumulates build exceptions instead of throwing.
	*/
	class FMTEXPORT FMTExcelExceptionHandler : public FMTExceptionHandler
	{
	public:
		// DocString: FMTExcelExceptionHandler::getBuildExceptions
		/**
		@brief Return the build exceptions accumulated by the handler.
		@return a map of exception type to the list of exception messages.
		*/
		std::unordered_map<int, std::vector<std::string>> getBuildExceptions() const;
		// DocString: FMTExcelExceptionHandler::resetBuildExceptions
		/**
		@brief Clear the accumulated build exceptions.
		*/
		void resetBuildExceptions();
		// DocString: FMTExcelExceptionHandler::raise
		/**
		@brief Override the base class raise function to accumulate build exceptions. See FMTExceptionHandler::raise.
		@param[in] lexception the exception type.
		@param[in] text the message of the exception.
		@param[in] method the method where the exception occurred.
		@param[in] line the line where the exception occurred.
		@param[in] file the file where the exception occurred.
		@param[in] lsection the section in which the exception occurred.
		@param[in] throwit if true throws the exception.
		@return the raised FMTException.
		*/
		virtual FMTException raise(FMTexc lexception, std::string text,
			const std::string& method, const int& line, const std::string& file,
			Core::FMTsection lsection = Core::FMTsection::Empty, bool throwit = true);
		// DocString: FMTExcelExceptionHandler()
		/**
		@brief Default constructor for FMTExcelExceptionHandler.
		*/
		FMTExcelExceptionHandler();
		// DocString: FMTExcelExceptionHandler::clone
		/**
		@brief clone the FMTExcelExceptionHandler.
		@return a unique pointer to the cloned exception handler.
		*/
		virtual std::unique_ptr <FMTExceptionHandler> clone() const;
	private:
		std::unordered_map<int, std::vector<std::string>>m_build_exceptions;
	};

}

#endif