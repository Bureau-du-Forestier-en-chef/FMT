#include "FMTutility.h"


const char* FMTwssect_str(FMTwssect section)
{
    switch (section)
    {
        case FMTwssect::Control:   return "Control";
        case FMTwssect::Landscape: return "Landscape";
        case FMTwssect::Area: return "Area";
        case FMTwssect::Action: return "Action";
        case FMTwssect::Transition: return "Transition";
        case FMTwssect::Yield: return "Yield";
        case FMTwssect::Outputs: return "Outputs";
        case FMTwssect::Optimize: return "Optimize";
        case FMTwssect::Constants: return "Constants";
        default:      return "[Unknown FMTwssect]";
    }
}

