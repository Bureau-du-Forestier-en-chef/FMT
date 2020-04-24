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

#ifndef FMTquiettlogger_H_INCLUDED
#define FMTquiettlogger_H_INCLUDED

#include "FMTlogger.h"

namespace Logging
{
// DocString: FMTquietlogger
/**
FMTquietlogger will not print any solver informations from osisolverinterface.
*/
class FMTquietlogger final : public FMTlogger
	{
	public:
		// DocString: FMTquietlogger()
		/**
		FMTquietlogger default constructor.
		*/
		FMTquietlogger();
		// DocString: FMTquietlogger(const FMTquietlogger&)
		/**
		FMTquietlogger default copy constructor.
		*/
		FMTquietlogger(const FMTquietlogger&) = default;
		// DocString: FMTquietlogger::operator=
		/**
		FMTquietlogger default copy assignment.
		*/
		FMTquietlogger& operator = (const FMTquietlogger&) = default;
		// DocString: FMTquietlogger::~FMTquietlogger
		/**
		FMTquietlogger default destructor.
		*/
		~FMTquietlogger()=default;
		#ifdef FMTWITHOSI
			// DocString: FMTquietlogger::print
			/**
			FMTquietlogger print nothing with osisolverinterface.
			See FMTlogger print function.
			*/
			int print() override;
			// DocString: FMTquietlogger::checkSeverity
			/**
			See FMTlogger checkSeverity function.
			*/
			void checkSeverity() override;
			// DocString: FMTquietlogger::clone
			/**
			See FMTlogger clone function.
			*/
			CoinMessageHandler * clone() const override;
	#endif
	};
}



#endif