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

#include "FMTutility.h"

namespace Core
{

	const char* FMTsection_str(FMTsection section)
	{
		switch (section)
		{
		case FMTsection::Control:   return "Control";
		case FMTsection::Landscape: return "Landscape";
		case FMTsection::Area: return "Area";
		case FMTsection::Action: return "Action";
		case FMTsection::Transition: return "Transition";
		case FMTsection::Yield: return "Yield";
		case FMTsection::Schedule: return "Schedule";
		case FMTsection::Outputs: return "Outputs";
		case FMTsection::Optimize: return "Optimize";
		case FMTsection::Constants: return "Constants";
		default:      return "[Unknown FMTsection]";
		}
	}
}

