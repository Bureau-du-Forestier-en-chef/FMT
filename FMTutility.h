#ifndef FMTUTILS_H_INCLUDED
#define FMTUTILS_H_INCLUDED



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

enum class FMTyldwstype
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
	//COUNT = 10
	//FMTwsdiscountfactor = 10 //need implementation
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

#endif // FMTUTILS_H_INCLUDED
