#pragma once
#include "stdafx.h"
#include "FMTexceptionhandler.hpp"
#include <unordered_map>
#include <vector>
#include <string>


namespace Exception
{
	class FMTexcelexceptionhandler : public FMTexceptionhandler
	{
		std::unordered_map<int,std::vector<std::string>>build_exceptions;
	public:
		std::unordered_map<int, std::vector<std::string>> getbuildexceptions() const;
		void resetbuildexceptions();
		virtual FMTexception raise(FMTexc lexception, std::string text,
			const std::string& method, const int& line, const std::string& file,
			Core::FMTsection lsection = Core::FMTsection::Empty, bool throwit = true);
		FMTexcelexceptionhandler();
	};

}