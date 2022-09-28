
#include "FMTtransitionparser.hpp"
#include "FMTGCBMtransition.hpp"
#include "FMTfreeexceptionhandler.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <streambuf>


int main()
{
Logging::FMTlogger().logstamp();
const std::string testinput = "22 serialization::archive 17 0 0 2 0 0 0 11 0 0 1 0 0 0 6 THEME5 6 FC8332 15 ACPFAIBL-FC8332 10 1 0 6 THEME5 6 FC8202 14 ACPFORT-FC8202";
Parser::FMTtransitionparser transitionparser;
std::vector<Core::FMTGCBMtransition>transitions;
std::map<std::string, std::string>m1;
m1["THEME5"] = "FC8332";
transitions.push_back(Core::FMTGCBMtransition(11,m1,"ACPFAIBL-FC8332"));
std::map<std::string, std::string>m2;
m2["THEME5"] = "FC8202";
transitions.push_back(Core::FMTGCBMtransition(10,m2, "ACPFORT-FC8202"));
const std::string outdir = "tests/testGCBMtransitionparser/gcbmtransitiontest.txt";
transitionparser.writeGCBM(transitions, outdir);
const std::vector<Core::FMTGCBMtransition> outtr = transitionparser.readGCBM(outdir);
if (outtr!= transitions)
    {
    Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Non equal read/write transitions",
        "testGMTtransitionparser", __LINE__, outdir);
    }
std::ifstream inputstream(outdir);
const std::string testoutput((std::istreambuf_iterator<char>(inputstream)),
    std::istreambuf_iterator<char>());
inputstream.close();
if (testinput!= testoutput)
    {
    Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Non equal string transitions",
        "testGMTtransitionparser", __LINE__, outdir);
    }

return 0;
}

