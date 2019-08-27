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

#include "FMTconstantparser.h"

namespace WSParser{

FMTconstantparser::FMTconstantparser(): FMTparser()
    {
    }

FMTconstantparser::FMTconstantparser(const FMTconstantparser& rhs):FMTparser(rhs){}


FMTconstantparser& FMTconstantparser::operator = (const FMTconstantparser& rhs)
    {
        if (this!=&rhs)
        {
            FMTparser::operator=(rhs);
        }
    return *this;
    }

FMTconstants FMTconstantparser::read(string location)
    {
	FMTconstants constants;
	boost::filesystem::path pathtoconstants(location);
	if (boost::filesystem::is_regular_file(pathtoconstants))//Constants is not a needed component
		{
		ifstream CONstream(location);
		string line;
		vector<FMTtheme>themes;
		if (FMTparser::tryopening(CONstream, location))
			{
			while (CONstream.is_open())
			{
				//line = FMTparser::getcleanline(CONstream);
				line = FMTparser::getcleanlinewfor(CONstream, themes, constants);
				if (!line.empty())
				{
					vector<string>splited = FMTparser::spliter(line, FMTparser::rxseparator);
					string key = splited[0];
					vector<string>values;
					for (size_t id = 1; id < splited.size(); ++id)
					{
						int period = int((id - 1));
						if (splited[id].find("#") != string::npos)
						{
							string strid = splited[id];
							strid.erase(0, 1);
							//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) <<key <<" "<< constants.getstr(strid, period) << "\n";
							values.push_back(constants.getstr(strid, period));
						}
						else {
							values.push_back(splited[id]);
						}
					}
					constants.set(key, values);
				}
			}
			}
		}
    return constants;
    }

bool FMTconstantparser::write(const FMTconstants& constants,string location)
    {
    ofstream constantstream;
    constantstream.open(location);
    if (tryopening(constantstream,location))
        {
        constantstream<<string(constants);
        constantstream.close();
        return true;
        }
    return false;
    }
}
