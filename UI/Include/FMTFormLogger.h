#pragma once

#include "FMTlogger.h"
#include <fstream>
#include <iostream>
#include <memory>

namespace Wrapper
{
	typedef void(__stdcall* logfunc)(const char* sts);

	class __declspec(dllexport) FMTFormLogger final :
		public Logging::FMTlogger
	{
	private:
		bool keepprint;
		mutable std::string lastprint;
		logfunc sendfeedback;
	protected:
		void cout(const char* message) const;
	public:
		FMTFormLogger() = default;
		~FMTFormLogger() = default;
		void dokeepprint();
		std::string getlastprint() const;
		FMTFormLogger(const std::string& nomFichierLogger,logfunc feed);
		void settasklogginglevel(int taskLogLevel);
		void setdefaultlogginglevel();
		virtual std::unique_ptr <FMTlogger> Clone() const;
		#ifdef FMTWITHOSI
			virtual FMTlogger* clone() const;
		#endif
	};
}

