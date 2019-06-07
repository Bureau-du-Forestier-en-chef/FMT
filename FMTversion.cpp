#include "FMTversion.h"
namespace Version
{

FMTversion::FMTversion()
	{

	}

int FMTversion::getmajor()
	{
	return API_MAJOR;
	}
int FMTversion::getminor()
	{
	return API_MINOR;
	}
int FMTversion::getpatch()
	{
	return API_PATCH;
	}
string FMTversion::getversion()
	{
	return to_string(getmajor()) + "." + to_string(getminor()) + "." + to_string(getpatch());
	}
bool FMTversion::isatleast(int major, int minor, int patch)
	{
	if (getmajor()>=major && getminor()>=minor && getpatch()>=patch)
		{
		return true;
		}
	return false;
	}	
bool FMTversion::hashfeature(const string& name)
	{
	return false;
	}

}