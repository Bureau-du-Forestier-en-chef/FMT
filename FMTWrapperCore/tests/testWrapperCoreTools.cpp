#include "Tools.h"
#include <string>
#include <vector>
#include <iostream>
#include "FMTException.h"


int testChangeLog()
{

	std::string log;
	try{
		log = FMTWrapperCore::Tools::getChangeLog();
		}catch (...)
			{
			return 1;
			}
	if (!log.empty())
		{
		std::cout << "test ChangeLog passed!" << std::endl;
		return 0;
	}else {
		return 1;
		}
}

int testGetErrorsToIgnore()
{
	std::vector<int>toIgnore;
	try {
		toIgnore = FMTWrapperCore::Tools::getErrorsToIgnore();
	}
	catch (...)
	{
		return 1;
	}
	if (!toIgnore.empty())
	{
		std::cout << "test GetErrorsToIgnore passed!" << std::endl;
		return 0;
	}
	else {
		return 1;
	}
}

int testGetExceptionDescription()
{
	bool failed = false;
	for (int i = 1; i < Exception::FMTexc::FMTmaxnumberofexception;++i)
	{
		const std::string DESCRIPTION = FMTWrapperCore::Tools::getExceptionDescription(i);
		if (DESCRIPTION.empty())
			{
			std::cout << "Missing exception description for exception id " << i << std::endl;
			failed = true;
			}
	}
	if (!failed)
	{
		std::cout << "test GetExceptionDescription passed!" << std::endl;
		return 0;
	}else{
		return 1;
	}
}

int main(int argc, char* argv[])
{

	return testChangeLog()+testGetExceptionDescription()+ testGetErrorsToIgnore();
}