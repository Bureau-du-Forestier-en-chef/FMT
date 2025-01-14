#include "stdafx.h"
#include <string>
#include "FMTmodelparser.h"
#include "FMTexceptionhandlerwarning.h"
#include "FMTdefaultexceptionhandler.h"
#include <msclr\marshal_cppstd.h>
#include "FMTFormLogger.h"
#include "FMTmodel.h"
#include "FMTFormCache.h"

Wrapper::FMTformCache::FMTformCache() : ModelParser(), Models(), Logger()
{

}



Wrapper::FMTformCache::~FMTformCache() = default;

const Models::FMTmodel& Wrapper::FMTformCache::getmodel(const int& index) const
	{
	try {
		if (static_cast<size_t>(index)>=Models.size())
		{
			getExceptionHandler()->raise(
				Exception::FMTexc::FMTrangeerror, "no scenario for index " + std::to_string(index) + " in cache ",
				"Wrapper::FMTformCache::getmodel", __LINE__, __FILE__);
		}

	}catch (...)
		{
		getExceptionHandler()->raisefromcatch("", "Wrapper::FMTformCache::getmodel", __LINE__, __FILE__);
		}
	return *Models.at(index).get();
	}


Wrapper::FMTexceptionhandlerwarning* Wrapper::FMTformCache::getformhandler()
{
	FMTexceptionhandlerwarning* wwarptr = dynamic_cast<FMTexceptionhandlerwarning*>(getExceptionHandler());
	try {
		if (!wwarptr)
		{
			getExceptionHandler()->raise(
				Exception::FMTexc::FMTrangeerror, "no valid FMTexceptionhandlerwarning in cache ",
				"Wrapper::FMTformCache::getformhandler", __LINE__, __FILE__);
		}
	}catch (...)
		{
		getExceptionHandler()->raisefromcatch("", "Wrapper::FMTformCache::getformhandler", __LINE__, __FILE__);
		}
	return wwarptr;
}
Wrapper::FMTFormLogger*  Wrapper::FMTformCache::getformlogger()
{
	FMTFormLogger* loggerptr = dynamic_cast<FMTFormLogger*>(Logger);
	try{
		if (!loggerptr)
		{
			getExceptionHandler()->raise(
				Exception::FMTexc::FMTrangeerror, "no valid FMTexceptionhandlerwarning in cache ",
				"Wrapper::FMTformCache::getformlogger", __LINE__, __FILE__);
		}
	}catch (...)
	{
		getExceptionHandler()->raisefromcatch("", "Wrapper::FMTformCache::getformlogger", __LINE__, __FILE__);
	}
	return loggerptr;
}


void Wrapper::FMTformCache::push_back(const Models::FMTmodel& model)
{
	try {
		Models.push_back(std::move(std::unique_ptr<Models::FMTmodel>(new Models::FMTmodel(model))));
	}catch (...)
		{
			getExceptionHandler()->raisefromcatch("", "Wrapper::FMTformCache::push_back", __LINE__, __FILE__);
		}
		
}
void Wrapper::FMTformCache::erase(const int& index)
{
	try{
		Models.erase(Models.begin() + index);
}catch (...)
	{
		getExceptionHandler()->raisefromcatch("", "Wrapper::FMTformCache::erase", __LINE__, __FILE__);
	}
}
void Wrapper::FMTformCache::clear()
{
	try{
		Models.clear();
	}catch (...)
	{
		getExceptionHandler()->raisefromcatch("", "Wrapper::FMTformCache::clear", __LINE__, __FILE__);
	}
}

void Wrapper::FMTformCache::initializeexceptionhandler(const int& maxwarnings,const std::vector<Exception::FMTexc>& warnings)
	{
	try {
		Parser::FMTmodelparser modelparser;
		if (getExceptionHandler())
		{
			FMTexceptionhandlerwarning* old = dynamic_cast<FMTexceptionhandlerwarning*>(getExceptionHandler());
			old->ResetThread();
		}

		std::unique_ptr<Exception::FMTexceptionhandler> handler(new FMTexceptionhandlerwarning(maxwarnings));
		modelparser.passinexceptionhandler(handler);		
		modelparser.seterrorstowarnings(warnings);
		modelparser.setTerminateStack();
		modelparser.setAbortStack();
		ModelParser = &modelparser;
	}
	catch (...)
		{
		getExceptionHandler()->raisefromcatch("", "Wrapper::FMTformCache::initializeexceptionhandler", __LINE__, __FILE__);
		}
	}

void Wrapper::FMTformCache::initializelogger(const std::string& filename, System::IntPtr intptrptr)
	{
	try{
	std::unique_ptr<Logging::FMTlogger> logger(new Wrapper::FMTFormLogger(filename,(logfunc)(void*)intptrptr));
	Parser::FMTmodelparser Modelparser;
	Modelparser.passinlogger(logger);
	Logger = Modelparser.getLogger();
	}catch (...)
		{
		getExceptionHandler()->raisefromcatch("", "Wrapper::FMTformCache::initializelogger", __LINE__, __FILE__);
		}
	}

Exception::FMTexceptionhandler* Wrapper::FMTformCache::getExceptionHandler() const
{
	return ModelParser->getExceptionHandler();
}

bool Wrapper::FMTformCache::empty() const
	{
	return Models.empty();
	}


