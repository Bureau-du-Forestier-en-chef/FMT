#pragma once
#include "stdafx.h"
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include "FMTobject.h"
#include "FMTlogger.h"


namespace Logging
{
	class FMTexcellogger : public Logging::FMTlogger
	{
		mutable std::string printout;
	protected:
		void cout(const char* message) const override;
	public:
		FMTexcellogger();
		std::string getprintout() const;
		void clearout();
	#ifdef FMTWITHOSI
		virtual FMTlogger* clone() const;
	#endif
	virtual std::unique_ptr <FMTlogger> Clone() const;
	};

}