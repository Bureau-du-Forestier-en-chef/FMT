#ifndef FMTGCBMTRANSITION_H_INCLUDED
#define FMTGCBMTRANSITION_H_INCLUDED

#include <map>
#include <string>

using namespace std;

namespace Core
{
	class FMTGCBMtransition
	{
	public:
		int ageafter;
		map<string,string>themes;
		string name;
		FMTGCBMtransition();
		FMTGCBMtransition(const int& lageafter, const map<string, string>& lthemes, const string& lname);
		FMTGCBMtransition(const FMTGCBMtransition& rhs);
		FMTGCBMtransition& operator = (const FMTGCBMtransition& rhs);
		~FMTGCBMtransition()=default;
	};
}
#endif
