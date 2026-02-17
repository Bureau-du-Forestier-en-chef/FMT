#include "stdafx.h"
#include <string>
#include "FMTexceptionhandlerwarning.h"
#include "FMTdefaultexceptionhandler.h"
#include <msclr\marshal_cppstd.h>
#include "FMTFormLogger.h"
#include "FMTmodel.h"
#include "FMTFormCache.h"

namespace Wrapper
{
std::unique_ptr<FMTFormCache> FMTFormCache::m_Instance = std::unique_ptr<FMTFormCache>(nullptr);

FMTFormCache* FMTFormCache::GetInstance()
	{
	if (!m_Instance)
		{
		m_Instance = std::unique_ptr<FMTFormCache>(new FMTFormCache());
		}
	return m_Instance.get();
	}


const Models::FMTmodel& FMTFormCache::getmodel(const int& index) const
	{
	try {
		if (static_cast<size_t>(index)>= m_Models.size())
		{
			GetExceptionHandler()->raise(
				Exception::FMTexc::FMTrangeerror, "no scenario for index " + std::to_string(index) + " in cache ",
				"FMTFormCache::getmodel", __LINE__, __FILE__);
		}

	}catch (...)
		{
		GetExceptionHandler()->raisefromcatch("", "FMTFormCache::getmodel", __LINE__, __FILE__);
		}
	return *m_Models.at(index).get();
	}


FMTexceptionhandlerwarning* FMTFormCache::GetFormHandler()
{
	FMTexceptionhandlerwarning* wwarptr = dynamic_cast<FMTexceptionhandlerwarning*>(GetExceptionHandler());
	try {
		if (!wwarptr)
		{
			GetExceptionHandler()->raise(
				Exception::FMTexc::FMTrangeerror, "no valid FMTexceptionhandlerwarning in cache ",
				"FMTFormCache::GetFormHandler", __LINE__, __FILE__);
		}
	}catch (...)
		{
		GetExceptionHandler()->raisefromcatch("", "FMTFormCache::GetFormHandler", __LINE__, __FILE__);
		}
	return wwarptr;
}
FMTFormLogger*  FMTFormCache::GetFormLogger()
{
	FMTFormLogger* loggerptr = dynamic_cast<FMTFormLogger*>(Models::FMTmodel::getLogger());
	try{
		if (!loggerptr)
		{
			GetExceptionHandler()->raise(
				Exception::FMTexc::FMTrangeerror, "no valid FMTexceptionhandlerwarning in cache ",
				"FMTFormCache::GetFormLogger", __LINE__, __FILE__);
		}
	}catch (...)
	{
		GetExceptionHandler()->raisefromcatch("", "FMTFormCache::GetFormLogger", __LINE__, __FILE__);
	}
	return loggerptr;
}


void FMTFormCache::push_back(const Models::FMTmodel& model)
{
	try {
		m_Models.push_back(std::move(std::unique_ptr<Models::FMTmodel>(new Models::FMTmodel(model))));
	}catch (...)
		{
			GetExceptionHandler()->raisefromcatch("", "FMTFormCache::push_back", __LINE__, __FILE__);
		}
		
}
void FMTFormCache::erase(const int& index)
{
	try{
		m_Models.erase(m_Models.begin() + index);
}catch (...)
	{
		GetExceptionHandler()->raisefromcatch("", "FMTFormCache::erase", __LINE__, __FILE__);
	}
}
void FMTFormCache::clear()
{
	try{
		m_Models.clear();
	}catch (...)
	{
		GetExceptionHandler()->raisefromcatch("", "FMTFormCache::clear", __LINE__, __FILE__);
	}
}

void FMTFormCache::InitializeExceptionHandler(const int& maxwarnings,const std::vector<Exception::FMTexc>& warnings)
	{
	try {
		if (GetExceptionHandler())
		{
			FMTexceptionhandlerwarning* old = dynamic_cast<FMTexceptionhandlerwarning*>(GetExceptionHandler());
			old->ResetThread();
		}

		std::unique_ptr<Exception::FMTexceptionhandler> handler(new FMTexceptionhandlerwarning(maxwarnings));
		Models::FMTmodel useLessModel;
		useLessModel.passinexceptionhandler(handler);
		useLessModel.seterrorstowarnings(warnings);
		useLessModel.setTerminateStack();
		useLessModel.setAbortStack();
	}
	catch (...)
		{
		GetExceptionHandler()->raisefromcatch("", "FMTFormCache::InitializeExceptionHandler", __LINE__, __FILE__);
		}
	}

void FMTFormCache::InitializeLogger(const std::string& filename, System::IntPtr intptrptr)
	{
	try{
	std::unique_ptr<Logging::FMTlogger> logger(new FMTFormLogger(filename,(logfunc)(void*)intptrptr));
	Models::FMTmodel useLessModel;
	useLessModel.passinlogger(logger);
	}catch (...)
		{
		GetExceptionHandler()->raisefromcatch("", "FMTFormCache::InitializeLogger", __LINE__, __FILE__);
		}
	}

Exception::FMTexceptionhandler* FMTFormCache::GetExceptionHandler() const
{
	return Models::FMTmodel::GetExceptionHandler();
}

bool FMTFormCache::empty() const
	{
	return m_Models.empty();
	}

}
