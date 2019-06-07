#ifndef FMTVERSION_H_INCLUDED
#define FMTVERSION_H_INCLUDED

#include <string>

#define API_MAJOR 0
#define API_MINOR 0
#define API_PATCH 0

using namespace std;

namespace Version
{
class FMTversion
	{
	public:
		FMTversion();
		~FMTversion()=default;
		static int getmajor();
		static int getminor();
		static int getpatch();
		static string getversion();
		static bool isatleast(int major, int minor, int patch);
		static bool hashfeature(const string& name);
	};
}

#endif