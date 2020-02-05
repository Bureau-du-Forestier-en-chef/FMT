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

#include "FMTlifespanparser.h"

namespace WSParser{

FMTlifespanparser::FMTlifespanparser():FMTparser()
    {

    }

FMTlifespanparser::FMTlifespanparser(const FMTlifespanparser& rhs):FMTparser(rhs)
    {

    }
FMTlifespanparser& FMTlifespanparser::operator = (const FMTlifespanparser& rhs)
    {
    if (this!=&rhs)
        {
        FMTparser::operator=(rhs);
        }
    return *this;
    }

Core::FMTlifespans FMTlifespanparser::read(const std::vector<Core::FMTtheme>& themes,const Core::FMTconstants& constants, std::string location)
    {
	std::ifstream LIFstream(location);
    Core::FMTlifespans lifespan;
    if (FMTparser::tryopening(LIFstream,location))
        {
        while(LIFstream.is_open())
            {
            const std::string line = FMTparser::getcleanline(LIFstream);
            if (!line.empty())
                {
				std::vector<std::string>splited = FMTparser::spliter(line,FMTparser::rxseparator);
				std::string page = splited[splited.size()-1];
                const int age = getnum<int>(page,constants);
                splited.pop_back();
				std::string mask = boost::algorithm::join(splited, " ");
                if (!validate(themes, mask)) continue;
				lifespan.push_back(Core::FMTmask(mask,themes), age);
                }
            }
        }
    lifespan.shrink();
    return lifespan;
    }

bool FMTlifespanparser::write(const Core::FMTlifespans& lifespan, std::string location)
    {
	std::ofstream lifespanstream;
    lifespanstream.open(location);
    if (tryopening(lifespanstream,location))
        {
        lifespanstream<< std::string(lifespan);
        lifespanstream.close();
        return true;
        }
    return false;
    }

}
