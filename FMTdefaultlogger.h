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

#ifndef FMTdefaultlogger_H_INCLUDED
#define FMTdefaultlogger_H_INCLUDED

#include "FMTlogger.h"

namespace Logging
{
	// DocString: FMTdefaultlogger 
	/**
	FMTdefaultlogger is derived class from FMTlogger standing has the default
	level of log used by FMT.
	*/
	class FMTdefaultlogger final: public FMTlogger
	{
	public:
		// DocString: FMTdefaultlogger()
		/**
		FMTdefaultlogger default constructor.
		*/
		FMTdefaultlogger();
		// DocString: ~FMTdefaultlogger()
		/**
		FMTdefaultlogger default destructor.
		*/
		~FMTdefaultlogger() = default;
		// DocString: FMTdefaultlogger::operator=
		/**
		FMTdefaultlogger default copy assignment operator.
		*/
		FMTdefaultlogger & operator = (const FMTdefaultlogger & rhs) = default;
		// DocString: FMTdefaultlogger(const FMTdefaultlogger&)
		/**
		FMTdefaultlogger default copy constructor.
		*/
		FMTdefaultlogger(const FMTdefaultlogger& rhs) = default;
		#ifdef FMTWITHOSI
			// DocString: FMTdefaultlogger::print
			/**
			FMTdefaultlogger print for osisolverinterface is the default print level used by FMT.
			See FMTlogger print function.
			*/
			int print() override;
			// DocString: FMTdefaultlogger::checkSeverity
			/**
			FMTdefaultlogger checkseverity for osisolverinterface is the default severity check used by FMT.
			See FMTlogger checkSeverity function.
			*/
			void checkSeverity() override;
			// DocString: FMTdefaultlogger::clone
			/**
			See FMTlogger clone function.
			*/
			CoinMessageHandler * clone() const override;
		#endif
	};

}



#endif