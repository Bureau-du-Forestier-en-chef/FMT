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
	class FMTExcelLogger : public Logging::FMTLogger
	{
	public:
		FMTExcelLogger();
		std::string getPrintOut() const;
		void clearOut();
	#ifdef FMTWITHOSI
		virtual FMTLogger* clone() const;
	#endif
	virtual std::unique_ptr <FMTLogger> Clone() const;
	protected:
		void cout(const char* message) const override;
	private:
		mutable std::string printout;
	};

}
#endif