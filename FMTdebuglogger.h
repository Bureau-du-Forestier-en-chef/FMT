/*
MIT License

Copyright (c) [2019] [Bureau du forestier en chef]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef FMTdebuglogger_H_INCLUDED
#define FMTdebuglogger_H_INCLUDED
#include "FMTlogger.h"
namespace Logging
{
	// DocString: FMTdebuglogger 
	/**
	FMTdebuglogger is derived class from FMTlogger standing has the debug
	level of log used by FMT. It's suppose to print more stuff than the FMTdefaultlogger.
	*/
	class FMTdebuglogger final : public FMTlogger
	{
	public:
		// DocString: FMTdebuglogger()
		/**
		FMTdebuglogger default constructor.
		*/
		FMTdebuglogger();
		// DocString: FMTdebuglogger::operator=
		/**
		FMTdebuglogger default copy assignment operator.
		*/
		FMTdebuglogger & operator = (const FMTdebuglogger & rhs) = default;
		// DocString: FMTdebuglogger(const FMTdebuglogger&)
		/**
		FMTdebuglogger default copy constructor.
		*/
		FMTdebuglogger(const FMTdebuglogger& rhs) = default;
		#ifdef FMTWITHOSI
			// DocString: FMTdebuglogger::print
			/**
			FMTdebuglogger print for osisolverinterface is the debug print level used by FMT.
			See FMTlogger print function.
			*/
			int print() override;
			// DocString: FMTdebuglogger::checkSeverity
			/**
			FMTdebuglogger checkseverity for osisolverinterface is the debug severity check used by FMT.
			See FMTlogger checkSeverity function.
			*/
			void checkSeverity() override;
			// DocString: FMTdebuglogger::clone
			/**
			See FMTlogger clone function.
			*/
			CoinMessageHandler* clone() const override;
		#endif
		// DocString: FMTdebuglogger()
		/**
		FMTdebuglogger default destructor.
		*/
		~FMTdebuglogger() = default;
	};
}

#endif

