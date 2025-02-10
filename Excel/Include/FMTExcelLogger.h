#ifndef FMTEXCELLOGGER_H_INCLUDED
#define FMTEXCELLOGGER_H_INCLUDED

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include "FMTobject.h"
#include "FMTlogger.h"


namespace Logging
{
	class FMTExcelLogger : public Logging::FMTlogger
	{
	public:
		FMTExcelLogger();
		std::string getprintout() const;
		void clearout();
	#ifdef FMTWITHOSI
		virtual FMTlogger* clone() const;
	#endif
	virtual std::unique_ptr <FMTlogger> Clone() const;
	protected:
		void cout(const char* message) const override;
	private:
		mutable std::string printout;
	};

}
#endif