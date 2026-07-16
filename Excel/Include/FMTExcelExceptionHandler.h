#include "stdafx.h"

#ifndef FMTEXCELEXCEPTIONHANDLER_H_INCLUDED
#define FMTEXCELEXCEPTIONHANDLER_H_INCLUDED

#include "FMTexceptionhandler.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>


namespace Exception
{
	class FMTExcelExceptionHandler : public FMTExceptionHandler
	{
	public:
		std::unordered_map<int, std::vector<std::string>> getBuildExceptions() const;
		void resetBuildExceptions();
		virtual FMTException raise(FMTexc lexception, std::string text,
			const std::string& method, const int& line, const std::string& file,
			Core::FMTsection lsection = Core::FMTsection::Empty, bool throwit = true);
		FMTExcelExceptionHandler();
		virtual std::unique_ptr <FMTExceptionHandler> Clone() const;
	private:
		std::unordered_map<int, std::vector<std::string>>build_exceptions;
	};

}

#endif