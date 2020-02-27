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

#ifndef FMTUTILS_H_INCLUDED
#define FMTUTILS_H_INCLUDED

#define FMT_DBL_TOLERANCE 1.e-08
#define FMT_STR_SEPARATOR "\t "

namespace Core
{
enum FMTwssect
    {
    Control= 1,
    Landscape=2,
    Area=3,
    Action = 4,
    Transition =5,
    Yield = 6,
    Outputs = 7,
    Optimize =8,
    Constants = 9,
    Schedule = 10,
	Lifespan = 11,
    Empty=12
    };

enum class FMTwskwor
    {
    Source =1,
    Target =2
    };

enum FMTyldwstype
    {
    FMTageyld = 1,
    FMTtimeyld = 2,
    FMTcomplexyld = 3
    };

enum class FMTyieldparserop
    {
    FMTwsnone = 0,
    FMTwsrange = 1,
    FMTwsmultiply = 2,
    FMTwssum = 3,
    FMTwssubstract = 4,
    FMTwsytp = 5,
    FMTwsmai = 6,
    FMTwscai= 7,
    FMTwsdivide= 8,
	FMTwsequation = 9,
	FMTwsendpoint = 10,
	FMTwsdiscountfactor = 11
    };

enum FMTotar
    {
    inventory = 1,
    actual = 2,
    val = 3,
	timeyld = 4,
	level = 5
    };

const char* FMTwssect_str(FMTwssect section);

}

#endif // FMTUTILS_H_INCLUDED
