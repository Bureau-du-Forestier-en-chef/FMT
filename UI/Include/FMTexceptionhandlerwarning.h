#pragma once

#include "FMTexceptionhandler.h"

namespace Wrapper
{
	public class FMTexceptionhandlerwarning :
		public virtual Exception::FMTexceptionhandler
	{
	public:
		FMTexceptionhandlerwarning(const size_t& maxnumberofwarnings);
		virtual void printexceptions(std::string text,
			const std::string& method, const int& line, const std::string& fil,
			Core::FMTsection lsection) override;
		void tryfileopener(const std::string& fullerrorstr) const;
		std::string geterrorstack(std::string text,
			const std::string& method, const int& line, const std::string& fil);
		virtual Exception::FMTexception FMTexceptionhandlerwarning::raise(Exception::FMTexc lexception, std::string text, const std::string& method, const int& line, const std::string& file, Core::FMTsection lsection, bool throwit) override;
		//void throw_nested(const std::exception& texception, int level = 0, bool rethrow = true) override;
		FMTexceptionhandlerwarning() = default;
		~FMTexceptionhandlerwarning() = default;
		std::unique_ptr <Exception::FMTexceptionhandler> Clone() const;
		void ResetThread();
	};
}