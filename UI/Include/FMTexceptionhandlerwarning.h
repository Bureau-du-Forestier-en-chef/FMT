#pragma once

#include "FMTExceptionHandler.h"

namespace Wrapper
{
	public class FMTexceptionhandlerwarning :
		public virtual Exception::FMTExceptionHandler
	{
	public:
		FMTexceptionhandlerwarning(const size_t& maxnumberofwarnings);
		virtual void printExceptions(std::string text,
			const std::string& method, const int& line, const std::string& fil,
			Core::FMTsection lsection) override;
		void tryfileopener(const std::string& fullerrorstr) const;
		std::string geterrorstack(std::string text,
			const std::string& method, const int& line, const std::string& fil);
		virtual Exception::FMTException FMTexceptionhandlerwarning::raise(Exception::FMTexc lexception, std::string text, const std::string& method, const int& line, const std::string& file, Core::FMTsection lsection, bool throwit) override;
		//void throwNested(const std::exception& texception, int level = 0, bool rethrow = true) override;
		FMTexceptionhandlerwarning() = default;
		~FMTexceptionhandlerwarning() = default;
		std::unique_ptr <Exception::FMTExceptionHandler> clone() const;
		void ResetThread();
	};
}