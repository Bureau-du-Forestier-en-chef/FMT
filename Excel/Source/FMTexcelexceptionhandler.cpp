#include "stdafx.h"
#include "FMTexcelexceptionhandler.hpp"
#include "FMTwarning.hpp"
#include "FMTerror.hpp"
#include <boost/thread.hpp>

namespace Exception
{

	FMTexcelexceptionhandler::FMTexcelexceptionhandler():FMTexceptionhandler(), build_exceptions()
		{
		std::vector<Exception::FMTexc>errors;
		errors.push_back(Exception::FMTexc::FMTmissingyield);
		errors.push_back(Exception::FMTexc::FMToutput_missing_operator);
		errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
		errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
		errors.push_back(Exception::FMTexc::FMTundefinedoutput_attribute);
		errors.push_back(Exception::FMTexc::FMToveridedyield);
		errors.push_back(Exception::FMTexc::FMTinvalid_geometry);
		errors.push_back(Exception::FMTexc::FMTsourcetotarget_transition);
		errors.push_back(Exception::FMTexc::FMTsame_transitiontargets);
		errors.push_back(Exception::FMTexc::FMTdeathwithlock);
		seterrorstowarnings(errors);
		}

	std::unordered_map<int, std::vector<std::string>> FMTexcelexceptionhandler::getbuildexceptions() const
		{
		return build_exceptions;
		}
	void FMTexcelexceptionhandler::resetbuildexceptions()
		{
		build_exceptions.clear();
		}
	FMTexception FMTexcelexceptionhandler::raise(FMTexc lexception, std::string text,
		const std::string& method, const int& line, const std::string& file,
		Core::FMTsection lsection, bool throwit)
		{
		if (build_exceptions.find(static_cast<int>(lexception))== build_exceptions.end())
			{
			build_exceptions[static_cast<int>(lexception)] = std::vector<std::string>();
			}
		build_exceptions[static_cast<int>(lexception)].push_back(text);
		boost::lock_guard<boost::recursive_mutex> guard(mtx);
		FMTexception excp = FMTexception(lexception, updatestatus(lexception, text));
		if (lsection != Core::FMTsection::Empty)
		{
			excp = FMTexception(lexception, lsection, updatestatus(lexception, text));
		}
		if (_level != FMTlev::FMT_Warning)
		{
			if (lsection == Core::FMTsection::Empty)
			{
				excp = FMTexception(lexception, updatestatus(lexception, text), method, file, line);
			}
			else {
				excp = FMTexception(lexception, lsection, updatestatus(lexception, text), method, file, line);
			}
		}
		if (throwit && !needtorethrow())
		{
			if (_level == FMTlev::FMT_Warning)
			{
				FMTwarning(excp).warn(_logger, _specificwarningcount, maxwarningsbeforesilenced);
			}
			else if (_level == FMTlev::FMT_logic || _level == FMTlev::FMT_range)
			{
				std::throw_with_nested(FMTerror(excp));
			}
		}

		return excp;
		}

}